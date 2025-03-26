#include "ChatGameMode.h"
#include "ChatPlayerController.h"
#include "ChatPlayerState.h"
#include "BnCBPFuncLibrary.h"
#include "ChatGameState.h"
#include "Kismet/GameplayStatics.h"

AChatGameMode::AChatGameMode():
	PlayerAttemptCount(3),
	ConnectedPlayerCount(0),
	bIsOnGame(false)
{
}

void AChatGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AChatGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ConnectedPlayerCount++;

	AChatPlayerState* ChatPlayerState = NewPlayer->GetPlayerState<AChatPlayerState>();
	if (ChatPlayerState)
	{
		//ChatPlayerState->PlayerName = *FString::Printf(TEXT("Player%d"), ConnectedPlayerCount);
		FString NewPlayerName = FString::Printf(TEXT("Player%d"), ConnectedPlayerCount);
		ChatPlayerState->PlayerName = FName(*NewPlayerName);

		UE_LOG(LogTemp, Warning, TEXT("Connected Player : %s"), *NewPlayerName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Player State is nullptr"));
	}
}

void AChatGameMode::StartGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Game"));

	// 게임 시작 메시지 broadcast
	BroadcastChatToClients("------- Start Game -------", "");

	bIsOnGame = true;

	// 모든 플레이어들 기회 초기화
	InitPlayerAttemptCount();

	// GameState 초기화 및 턴 시작
	AChatGameState* ChatGameState = GetGameState<AChatGameState>();
	if (ChatGameState)
	{
		ChatGameState->Answer = UBnCBPFuncLibrary::GenerateRandomNumber();
		UE_LOG(LogTemp, Warning, TEXT("Answer : %s"), *(ChatGameState->Answer));

		ChatGameState->DecideFirstTurn();	// 첫 순서 랜덤
		ChatGameState->StartTurn();
	}
}

void AChatGameMode::InitPlayerAttemptCount()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AChatPlayerController* ChatPlayerController = Cast<AChatPlayerController>(It->Get());
		if (ChatPlayerController)
		{
			AChatPlayerState* ChatPlayerState =	ChatPlayerController->GetPlayerState<AChatPlayerState>();
			if (ChatPlayerState)
			{
				ChatPlayerState->SetAttemptCount(PlayerAttemptCount);
				UpdateAttemptCountUI(ChatPlayerController);
			}
		}
	}
}

void AChatGameMode::EndGame()
{
	UE_LOG(LogTemp, Warning, TEXT("End Game"));

	bIsOnGame = false;

	AChatGameState* ChatGameState = GetGameState<AChatGameState>();
	if (ChatGameState)
	{
		ChatGameState->EndTurn();
	}

	BroadcastChatToClients("-------- End Game -------", "");
}

void AChatGameMode::ReceiveChatFromClient(APlayerController* SenderPlayerController, const FString& Message, const FName& SenderName)
{
	UE_LOG(LogTemp, Warning, TEXT("Server Receive Chat From Client"));

	// 일단 서버에서 받은 채팅들은 기본적으로 broadcast 하기
	// => 수정 : 기본 채팅들만 broadcast
	if (Message[0] != '/')
	{
		BroadcastChatToClients(Message, SenderName);
	}

	// 받은 채팅이 응답 패턴을 포함하는지 확인
	ParseChat(SenderPlayerController, Message, SenderName);
}

void AChatGameMode::DecreaseAttemptCount(APlayerController* SenderPlayerController)
{
	if (SenderPlayerController)
	{
		AChatPlayerState* ChatPlayerState = SenderPlayerController->GetPlayerState<AChatPlayerState>();
		if (ChatPlayerState)
		{
			// 기회 소모
			ChatPlayerState->DecreaseAttemptCount();
			UE_LOG(LogTemp, Warning, TEXT("Attempt Count : %d"), ChatPlayerState->AttemptCount);

			// UI 업데이트
			UpdateAttemptCountUI(SenderPlayerController);
		}
	}
}

void AChatGameMode::UpdateAttemptCountUI(APlayerController* SenderPlayerController)
{
	if (SenderPlayerController)
	{
		AChatPlayerController* ChatPlayerController = Cast<AChatPlayerController>(SenderPlayerController);
		if (!ChatPlayerController)
		{
			return;
		}

		// UI 업데이트
		// 서버는 자기가 곧 클라이언트이자 서버니까 리플리케이션 딜레이가 없는데
		// 클라이언트는 리플리케이션 딜레이가 좀 있을 수 있음
		// => 리플리케이션 되기 전에 UI Update를 먼저 할 수도 있음 => UI에 남은 기회가 제대로 반영 안됨
		// ==> 서버에서 바꾼 값을 직접 넘겨줌
		AChatPlayerState* ChatPlayerState = SenderPlayerController->GetPlayerState<AChatPlayerState>();
		if (ChatPlayerState)
		{
			ChatPlayerController->Client_UpdateAttemptCountUI(ChatPlayerState->AttemptCount);
		}
	}
}

void AChatGameMode::ParseChat(APlayerController* SenderPlayerController, const FString& Message, const FName& SenderName)
{
	// '/'는 응답 패턴
	if (Message[0] == '/')
	{
		AChatGameState* ChatGameState = GetGameState<AChatGameState>();
		if (!ChatGameState || !SenderPlayerController)
		{
			return;
		}

		// 서버에서만 "/start"로 게임 실행할 수 있도록
		if (!bIsOnGame && Message == "/start" && GetWorld()->GetFirstPlayerController() == SenderPlayerController)
		{
			StartGame();
			return;
		}

		// 게임이 시작되지 않았으면 입력 무시
		if (!bIsOnGame)
		{
			UE_LOG(LogTemp, Warning, TEXT("Game didn't start"));
			return;
		}

		// 현재 자신의 턴인지 확인하고 아니면 무시	
		if (ChatGameState->PlayerArray[ChatGameState->CurrentTurnPlayerIndex] != SenderPlayerController->GetPlayerState<AChatPlayerState>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Not your Turn"));
			return;
		}

		// 플레이어가 입력한 예측 broadcast
		BroadcastChatToClients(Message, SenderName);

		// 정답 비교
		UBnCBPFuncLibrary::EvaluateBullsAndCows(Message, ChatGameState->Answer, ChatGameState->Bulls, ChatGameState->Cows, ChatGameState->Out);
		FString Msg = "";

		if (ChatGameState->Out)
		{
			Msg = "OUT";
		}
		else
		{
			Msg = FString::Printf(TEXT("%d Bulls, %d Cows"), ChatGameState->Bulls, ChatGameState->Cows);
		}

		// 시도 횟수 차감
		DecreaseAttemptCount(SenderPlayerController);

		// 비교 결과 broadcast
		BroadcastChatToClients(Msg, "");

		// 턴 변경
		if (IsGameOver(SenderName))
		{
			EndGame();
		}
		else
		{
			ChatGameState->PassTurn();
		}
	}
}

bool AChatGameMode::IsGameOver(const FName& SenderName)
{
	AChatGameState* ChatGameState = GetGameState<AChatGameState>();
	if (ChatGameState)
	{
		if (ChatGameState->Bulls == 3)
		{
			BroadcastChatToClients("Victory!!", SenderName);
			return true;
		}
		else if (IsDraw())
		{
			BroadcastChatToClients("---------- Draw ----------", "");
			return true;
		}
	}

	return false;
}

bool AChatGameMode::IsDraw()
{
	AChatGameState* ChatGameState = GetGameState<AChatGameState>();
	if (!ChatGameState)
	{
		return false;
	}

	// PlayerArray는 AGameStateBase에 기본적으로 들어있는 배열,
	// 현재 게임에 참가 중인 모든 플레이어들의 APlayerState들을 모아놓은 배열
	for (APlayerState* PlayerState : ChatGameState->PlayerArray)
	{
		AChatPlayerState* ChatPlayerState = Cast<AChatPlayerState>(PlayerState);
		if (ChatPlayerState && ChatPlayerState->AttemptCount > 0)
		{
			return false;
		}
	}
	
	return true;
}

void AChatGameMode::BroadcastChatToClients(const FString& Message, const FName& SenderName)
{
	UE_LOG(LogTemp, Warning, TEXT("Broadcast Chat To Clients"));
	TArray<AActor*> FoundPCs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AChatPlayerController::StaticClass(), FoundPCs);

	if (FoundPCs.Num() > 0)
	{
		for (AActor* Actor : FoundPCs)
		{
			if (Actor)
			{
				AChatPlayerController* ChatPlayerController = Cast<AChatPlayerController>(Actor);
				if (ChatPlayerController)
				{
					ChatPlayerController->Client_ReceiveChatFromServer(Message, SenderName);
				}
			}
		}
	}
}
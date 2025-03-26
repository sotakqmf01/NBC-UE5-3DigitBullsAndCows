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

	// ���� ���� �޽��� broadcast
	BroadcastChatToClients("------- Start Game -------", "");

	bIsOnGame = true;

	// ��� �÷��̾�� ��ȸ �ʱ�ȭ
	InitPlayerAttemptCount();

	// GameState �ʱ�ȭ �� �� ����
	AChatGameState* ChatGameState = GetGameState<AChatGameState>();
	if (ChatGameState)
	{
		ChatGameState->Answer = UBnCBPFuncLibrary::GenerateRandomNumber();
		UE_LOG(LogTemp, Warning, TEXT("Answer : %s"), *(ChatGameState->Answer));

		ChatGameState->DecideFirstTurn();	// ù ���� ����
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

	// �ϴ� �������� ���� ä�õ��� �⺻������ broadcast �ϱ�
	// => ���� : �⺻ ä�õ鸸 broadcast
	if (Message[0] != '/')
	{
		BroadcastChatToClients(Message, SenderName);
	}

	// ���� ä���� ���� ������ �����ϴ��� Ȯ��
	ParseChat(SenderPlayerController, Message, SenderName);
}

void AChatGameMode::DecreaseAttemptCount(APlayerController* SenderPlayerController)
{
	if (SenderPlayerController)
	{
		AChatPlayerState* ChatPlayerState = SenderPlayerController->GetPlayerState<AChatPlayerState>();
		if (ChatPlayerState)
		{
			// ��ȸ �Ҹ�
			ChatPlayerState->DecreaseAttemptCount();
			UE_LOG(LogTemp, Warning, TEXT("Attempt Count : %d"), ChatPlayerState->AttemptCount);

			// UI ������Ʈ
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

		// UI ������Ʈ
		// ������ �ڱⰡ �� Ŭ���̾�Ʈ���� �����ϱ� ���ø����̼� �����̰� ���µ�
		// Ŭ���̾�Ʈ�� ���ø����̼� �����̰� �� ���� �� ����
		// => ���ø����̼� �Ǳ� ���� UI Update�� ���� �� ���� ���� => UI�� ���� ��ȸ�� ����� �ݿ� �ȵ�
		// ==> �������� �ٲ� ���� ���� �Ѱ���
		AChatPlayerState* ChatPlayerState = SenderPlayerController->GetPlayerState<AChatPlayerState>();
		if (ChatPlayerState)
		{
			ChatPlayerController->Client_UpdateAttemptCountUI(ChatPlayerState->AttemptCount);
		}
	}
}

void AChatGameMode::ParseChat(APlayerController* SenderPlayerController, const FString& Message, const FName& SenderName)
{
	// '/'�� ���� ����
	if (Message[0] == '/')
	{
		AChatGameState* ChatGameState = GetGameState<AChatGameState>();
		if (!ChatGameState || !SenderPlayerController)
		{
			return;
		}

		// ���������� "/start"�� ���� ������ �� �ֵ���
		if (!bIsOnGame && Message == "/start" && GetWorld()->GetFirstPlayerController() == SenderPlayerController)
		{
			StartGame();
			return;
		}

		// ������ ���۵��� �ʾ����� �Է� ����
		if (!bIsOnGame)
		{
			UE_LOG(LogTemp, Warning, TEXT("Game didn't start"));
			return;
		}

		// ���� �ڽ��� ������ Ȯ���ϰ� �ƴϸ� ����	
		if (ChatGameState->PlayerArray[ChatGameState->CurrentTurnPlayerIndex] != SenderPlayerController->GetPlayerState<AChatPlayerState>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Not your Turn"));
			return;
		}

		// �÷��̾ �Է��� ���� broadcast
		BroadcastChatToClients(Message, SenderName);

		// ���� ��
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

		// �õ� Ƚ�� ����
		DecreaseAttemptCount(SenderPlayerController);

		// �� ��� broadcast
		BroadcastChatToClients(Msg, "");

		// �� ����
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

	// PlayerArray�� AGameStateBase�� �⺻������ ����ִ� �迭,
	// ���� ���ӿ� ���� ���� ��� �÷��̾���� APlayerState���� ��Ƴ��� �迭
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
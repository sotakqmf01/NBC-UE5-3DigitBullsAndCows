#include "ChatGameState.h"
#include "ChatPlayerController.h"
#include "ChatPlayerState.h"
#include "ChatGameMode.h"
#include "Net/UnrealNetwork.h"

AChatGameState::AChatGameState() :
	Answer(""),
	Bulls(0),
	Cows(0),
	Out(false),
	CurrentTurnPlayerIndex(-1),
	TimeLimit(30.0f)
{
	// 기본
	//bReplicates = true;
}

void AChatGameState::GameStateClear()
{
	Answer = "";
	Bulls = 0;
	Cows = 0;
	CurrentTurnPlayerIndex = -1;
}

void AChatGameState::DecideFirstTurn()
{
	CurrentTurnPlayerIndex = FMath::RandRange(0, PlayerArray.Num() - 1);

	UpdateTurnPlayerUI();
}

void AChatGameState::StartTurn()
{
	GetWorldTimerManager().SetTimer(TurnTimerHandle, this, &AChatGameState::TimeOutPassTurn, TimeLimit, false);

	GetWorldTimerManager().SetTimer(UIUpdateTimerHandle, this, &AChatGameState::UpdateTimerUI, 0.1f, true);
}

void AChatGameState::TimeOutPassTurn()
{
	UE_LOG(LogTemp, Warning, TEXT("Time Out"));

	APlayerState* PlayerState = PlayerArray[CurrentTurnPlayerIndex];
	if (!PlayerState)
	{
		return;
	}

	AChatPlayerState* CurrentPlayerState = Cast<AChatPlayerState>(PlayerState);
	if (!CurrentPlayerState)
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(CurrentPlayerState->GetOwner());
	if (!PlayerController)
	{
		return;
	}
	
	// 제한 시간 다되면 기회 소모
	AChatPlayerController* ChatPlayerController = Cast<AChatPlayerController>(PlayerController);
	if (ChatPlayerController)
	{
		AChatGameMode* ChatGameMode = GetWorld()->GetAuthGameMode<AChatGameMode>();
		if (ChatGameMode)
		{
			UE_LOG(LogTemp, Warning, TEXT("Decrease AttemptCount In GameState"));
			ChatGameMode->DecreaseAttemptCount(ChatPlayerController);

			ChatGameMode->BroadcastChatToClients("Time Out!!", CurrentPlayerState->PlayerName);
		
			if (ChatGameMode->IsGameOver(""))
			{
				ChatGameMode->EndGame();
			}
			else
			{
				PassTurn();
			}
		}
	}
}

void AChatGameState::PassTurn()
{
	GetWorldTimerManager().ClearTimer(TurnTimerHandle);

	CurrentTurnPlayerIndex = (CurrentTurnPlayerIndex + 1) % PlayerArray.Num();

	UpdateTurnPlayerUI();

	StartTurn();
}

void AChatGameState::EndTurn()
{
	GetWorldTimerManager().ClearTimer(TurnTimerHandle);
	GetWorldTimerManager().ClearTimer(UIUpdateTimerHandle);

	GameStateClear();

	UpdateTurnPlayerUI();
}

void AChatGameState::UpdateTurnPlayerUI()
{
	// 인덱스 유효 범위 학인
	if (CurrentTurnPlayerIndex >= PlayerArray.Num() || CurrentTurnPlayerIndex < -1)
	{
		return;
	}

	FName CurrentTurnPlayerName;

	// 인덱스 값에 따라 이름 설정
	if (CurrentTurnPlayerIndex == -1)
	{
		CurrentTurnPlayerName = "";
	}
	else
	{
		APlayerState* PlayerState = PlayerArray[CurrentTurnPlayerIndex];
		if (PlayerState)
		{
			AChatPlayerState* CurrentPlayerState = Cast<AChatPlayerState>(PlayerState);
			if (CurrentPlayerState)
			{
				CurrentTurnPlayerName = CurrentPlayerState->PlayerName;
			}
		}
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AChatPlayerController* ChatPlayerController = Cast<AChatPlayerController>(It->Get());
		if (ChatPlayerController)
		{
			ChatPlayerController->Client_UpdateTurnPlayerUI(CurrentTurnPlayerName);
		}
	}
}

void AChatGameState::UpdateTimerUI()
{
	float RemainingTime = GetWorldTimerManager().GetTimerRemaining(TurnTimerHandle);
	if (RemainingTime <= 0.0f)
	{
		RemainingTime = 0.0f;
	}

	Multicast_UpdateTimerUI(RemainingTime);
}

void AChatGameState::Multicast_UpdateTimerUI_Implementation(float RemainingTime)
{
	AChatPlayerController* ChatPlayerController = Cast<AChatPlayerController>(GetWorld()->GetFirstPlayerController());
	if (ChatPlayerController)
	{
		ChatPlayerController->UpdateTimerUI(RemainingTime);
	}
}

void AChatGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AChatGameState, CurrentTurnPlayerIndex);
}

// 제한 시간 UI 갱신할 때 모든 클라이언트에 대해 ClientRPC 호출하는 방법
//void AChatGameState::UpdateTimerUI()
//{
//	float RemainingTime = GetWorldTimerManager().GetTimerRemaining(TurnTimerHandle);
//	if (RemainingTime <= 0.0f)
//	{
//		RemainingTime = 0.0f;
//	}
//
//	// 모든 플레이어 컨트롤러에 대해 Multicast 호출
//	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
//	{
//		AChatPlayerController* ChatPlayerController = Cast<AChatPlayerController>(It->Get());
//		if (ChatPlayerController)
//		{
//			ChatPlayerController->Client_UpdateTimerUI(RemainingTime);
//		}
//	}
//}
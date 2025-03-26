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
	// �⺻
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
	
	// ���� �ð� �ٵǸ� ��ȸ �Ҹ�
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
	// �ε��� ��ȿ ���� ����
	if (CurrentTurnPlayerIndex >= PlayerArray.Num() || CurrentTurnPlayerIndex < -1)
	{
		return;
	}

	FName CurrentTurnPlayerName;

	// �ε��� ���� ���� �̸� ����
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

// ���� �ð� UI ������ �� ��� Ŭ���̾�Ʈ�� ���� ClientRPC ȣ���ϴ� ���
//void AChatGameState::UpdateTimerUI()
//{
//	float RemainingTime = GetWorldTimerManager().GetTimerRemaining(TurnTimerHandle);
//	if (RemainingTime <= 0.0f)
//	{
//		RemainingTime = 0.0f;
//	}
//
//	// ��� �÷��̾� ��Ʈ�ѷ��� ���� Multicast ȣ��
//	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
//	{
//		AChatPlayerController* ChatPlayerController = Cast<AChatPlayerController>(It->Get());
//		if (ChatPlayerController)
//		{
//			ChatPlayerController->Client_UpdateTimerUI(RemainingTime);
//		}
//	}
//}
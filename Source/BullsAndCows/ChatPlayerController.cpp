#include "ChatPlayerController.h"
#include "ChatGameMode.h"
#include "ChatPlayerState.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Kismet/GameplayStatics.h"

AChatPlayerController::AChatPlayerController() :
	ChatUIClass(nullptr),
	ChatUIInstance(nullptr),
	ChatInputBox(nullptr),
	ChatLog(nullptr)
{
}

void AChatPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetupHUD();
	PlayerLogin();
}

void AChatPlayerController::SetupHUD()
{
	ENetMode NetMode = GEngine->GetNetMode(GetWorld());
	UE_LOG(LogTemp, Warning, TEXT("NetMode: %d"), NetMode);

	if (NetMode == NM_ListenServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current is Listen Server"));
	}

	// 서버에는 UI가 없음 => 서버에서 UI 추가하면 에러 발생 => 서버에서의 SetupHUD는 무시
	if (!IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("Skip Server"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("SetupHUD Client"));

	if (ChatUIInstance)
	{
		ChatUIInstance->RemoveFromParent();
		ChatUIInstance = nullptr;
	}

	if (ChatUIClass)
	{
		ChatUIInstance = CreateWidget<UUserWidget>(this, ChatUIClass);
		if (ChatUIInstance)
		{
			ChatUIInstance->AddToViewport();
			ChatInputBox = Cast<UEditableTextBox>(ChatUIInstance->GetWidgetFromName("ChatInputBox"));
			ChatLog = Cast<UScrollBox>(ChatUIInstance->GetWidgetFromName("ChatLog"));

			// 서버는 리플리케이션을 할 필요가 없으니, OnRep_PlayerState() 호출 안됨
			// => 직접 입력창 UI 업데이트
			AChatPlayerState* ChatPlayerState = GetPlayerState<AChatPlayerState>();
			if (ChatPlayerState)
			{
				if (HasAuthority())
				{
					ChatInputBox->SetText(FText::FromName(ChatPlayerState->PlayerName));
				}
			}
		}
	}

	bShowMouseCursor = true;
	SetInputMode(FInputModeUIOnly());
}

void AChatPlayerController::PlayerLogin()
{
	if (HasAuthority())
	{
		PlayerName = "Host";
	}
	else
	{
		PlayerName = "Guest";
	}
}

// 클라이언트 입력창 UI 업데이트
void AChatPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	UE_LOG(LogTemp, Warning, TEXT("PlayerState Replicated"));

	AChatPlayerState* ChatPlayerState = GetPlayerState<AChatPlayerState>();
	if (ChatPlayerState)
	{
		ChatInputBox->SetText(FText::FromName(ChatPlayerState->PlayerName));
	}

	if (!ChatUIInstance)
	{
		SetupHUD();
	}
}

void AChatPlayerController::SendChat(const FString& Message)
{
	if (!Message.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Send Message To Server"));
		
		AChatPlayerState* ChatPlayerState = GetPlayerState<AChatPlayerState>();
		if (ChatPlayerState)
		{
			Server_SendChatToServer(Message, ChatPlayerState->PlayerName);
		}

		// 채팅 보내고 나면 입력창 비우기
		if (ChatInputBox)
		{
			UE_LOG(LogTemp, Warning, TEXT("ChatInputBox OK"));
			ChatInputBox->SetText(FText::FromString(TEXT("")));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ChatInputBox NO"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO Message"));
	}
}

void AChatPlayerController::UpdateTimerUI(float RemainingTime)
{
	if (!ChatUIInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("TimerUI : No ChatUIInstance"));
		return;
	}

	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Server : GameState's Multicast_UpdatedUI called! RemainingTime : %.1f"), RemainingTime);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Client : GameState's Multicast_UpdatedUI called! RemainingTime : %.1f"), RemainingTime);
	}

	UTextBlock* TimeLimitText = Cast<UTextBlock>(ChatUIInstance->GetWidgetFromName("TimeLimitText"));
	if (TimeLimitText)
	{
		TimeLimitText->SetText(FText::FromString(FString::Printf(TEXT("Time Limit : %d"), FMath::CeilToInt(RemainingTime))));
	}
}

// https://dev.epicgames.com/documentation/en-us/unreal-engine/remote-procedure-calls-in-unreal-engine
bool AChatPlayerController::Server_SendChatToServer_Validate(const FString& Message, const FName& SenderName)
{
	return true;
}

void AChatPlayerController::Server_SendChatToServer_Implementation(const FString& Message, const FName& SenderName)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerController To Server"));
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(GetWorld());
	if (GameModeBase)
	{
		AChatGameMode* ChatGameMode = Cast<AChatGameMode>(GameModeBase);
		if (ChatGameMode)
		{
			ChatGameMode->ReceiveChatFromClient(this, Message, SenderName);
		}
	}
}

void AChatPlayerController::Client_ReceiveChatFromServer_Implementation(const FString& Message, const FName& SenderName)
{
	UE_LOG(LogTemp, Warning, TEXT("Client Receive Chat From Server"));

	if (ChatUIInstance)
	{
		if (ChatLog)
		{
			UTextBlock* NewChatEntry = ChatUIInstance->WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
			if (NewChatEntry)
			{
				if (SenderName != "")
				{
					NewChatEntry->SetText(FText::FromString(SenderName.ToString() + " : " + Message));
				}
				else
				{
					NewChatEntry->SetText(FText::FromString("[" + Message + "]"));
				}

				// 채팅을 채팅 창에 추가, 밑에 추가됨(일반적인 채팅창 생각하면 됨)
				ChatLog->AddChild(NewChatEntry);
				// 기본적으로 스크롤이 위에 가 있기 때문에 밑에 있도록
				ChatLog->ScrollToEnd();
			}
		}
	}
}

void AChatPlayerController::Client_UpdateAttemptCountUI_Implementation(int32 AttemptCount)
{
	if (!ChatUIInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("No ChatUIInstance"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Client_UpdateUI called! Count: %d"), AttemptCount);

	UTextBlock* ChanceText = Cast<UTextBlock>(ChatUIInstance->GetWidgetFromName("ChanceText"));
	if (ChanceText)
	{
		ChanceText->SetText(FText::FromString(FString::Printf(TEXT("chance : %d"), AttemptCount)));
	}
}

void AChatPlayerController::Client_UpdateTurnPlayerUI_Implementation(const FName& CurrentTurnPlayerName)
{
	if (!ChatUIInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("No ChatUIInstance"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Client_UpdateUI called! current Turn : %s"), *CurrentTurnPlayerName.ToString());

	UTextBlock* TurnPlayerText = Cast<UTextBlock>(ChatUIInstance->GetWidgetFromName("TurnPlayerText"));
	if (TurnPlayerText)
	{
		TurnPlayerText->SetText(FText::FromString(FString::Printf(TEXT("Turn : %s"), *CurrentTurnPlayerName.ToString())));
	}
}
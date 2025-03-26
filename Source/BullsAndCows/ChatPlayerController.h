#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChatPlayerController.generated.h"

class UEditableTextBox;
class UScrollBox;

UCLASS()
class BULLSANDCOWS_API AChatPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AChatPlayerController();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendChatToServer(const FString& Message, const FName& SenderName);	// PlayerController에서 GameMode(서버)로
	UFUNCTION(Client, Reliable)
	void Client_ReceiveChatFromServer(const FString& Message, const FName& SenderName);	// GameMode(서버)에서 PlayerController로
	UFUNCTION(Client, Reliable)
	void Client_UpdateAttemptCountUI(int32 AttemptCount);	// 시도 횟수 바뀔 때 마다 UI 업데이트(개인)
	UFUNCTION(Client, Reliable)
	void Client_UpdateTurnPlayerUI(const FName& CurrentTurnPlayerName);	// 현재 턴인 플레이어 보여주는 UI 업데이트(전부)

	UFUNCTION(BlueprintCallable)
	void SendChat(const FString& Message);	// UI에 입력한 메시지를 PlayerController로

	void UpdateTimerUI(float RemainingTime);	// 제한 시간을 보여주는 UI 업데이트(전부)

protected:
	virtual void BeginPlay() override;

	// PlayerState가 네트워크를 통해 클라이언트로 복제(replication)되었을 때 자동으로 호출되는 함수
	virtual void OnRep_PlayerState() override;

private:
	void SetupHUD();	// UI
	void PlayerLogin();	// 로그인
	
protected:
	// UI 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<UUserWidget> ChatUIClass;
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "HUD")
	UUserWidget* ChatUIInstance;
	UPROPERTY()
	UEditableTextBox* ChatInputBox;
	UPROPERTY()
	UScrollBox* ChatLog;

private:	
	FName PlayerName;
};

//UFUNCTION(Client, Reliable)
//void Client_UpdateTimerUI(float RemainingTime);	// 제한 시간을 보여주는 UI 업데이트(전부)
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
	void Server_SendChatToServer(const FString& Message, const FName& SenderName);	// PlayerController���� GameMode(����)��
	UFUNCTION(Client, Reliable)
	void Client_ReceiveChatFromServer(const FString& Message, const FName& SenderName);	// GameMode(����)���� PlayerController��
	UFUNCTION(Client, Reliable)
	void Client_UpdateAttemptCountUI(int32 AttemptCount);	// �õ� Ƚ�� �ٲ� �� ���� UI ������Ʈ(����)
	UFUNCTION(Client, Reliable)
	void Client_UpdateTurnPlayerUI(const FName& CurrentTurnPlayerName);	// ���� ���� �÷��̾� �����ִ� UI ������Ʈ(����)

	UFUNCTION(BlueprintCallable)
	void SendChat(const FString& Message);	// UI�� �Է��� �޽����� PlayerController��

	void UpdateTimerUI(float RemainingTime);	// ���� �ð��� �����ִ� UI ������Ʈ(����)

protected:
	virtual void BeginPlay() override;

	// PlayerState�� ��Ʈ��ũ�� ���� Ŭ���̾�Ʈ�� ����(replication)�Ǿ��� �� �ڵ����� ȣ��Ǵ� �Լ�
	virtual void OnRep_PlayerState() override;

private:
	void SetupHUD();	// UI
	void PlayerLogin();	// �α���
	
protected:
	// UI ����
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
//void Client_UpdateTimerUI(float RemainingTime);	// ���� �ð��� �����ִ� UI ������Ʈ(����)
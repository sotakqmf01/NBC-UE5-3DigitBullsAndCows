#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ChatGameMode.generated.h"

UCLASS()
class BULLSANDCOWS_API AChatGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AChatGameMode();

	virtual void BeginPlay() override;
	// Ŭ���̾�Ʈ�� ������ �α����ϰ� ���� ȣ��� => ���� �÷��̾� �� �ø��� ����
	virtual void PostLogin(APlayerController* NewPlayer) override;

	void StartGame();
	void InitPlayerAttemptCount();
	void EndGame();
	void ReceiveChatFromClient(APlayerController* SenderPlayerController, const FString& Message, const FName& SenderName);
	void DecreaseAttemptCount(APlayerController* SenderPlayerController);
	void UpdateAttemptCountUI(APlayerController* SenderPlayerController);
	void ParseChat(APlayerController* SenderPlayerController, const FString& Message, const FName& SenderName);
	bool IsGameOver(const FName& SenderName);
	bool IsDraw();
	void BroadcastChatToClients(const FString& Message, const FName& SenderName);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameMode")
	int32 PlayerAttemptCount;

private:
	int32 ConnectedPlayerCount;
	bool bIsOnGame;
};

// PlayerController�� ������ ��, PlayerState�� ����������
// �Ǽ��� Ư�� ��Ȳ�� PlayerState�� ���� �� ����
// �� ���� ����� Ÿ�̸��� ��������Ʈ �Լ�
//void InitPlayerStateAfterDelay(APlayerController* NewPlayer);
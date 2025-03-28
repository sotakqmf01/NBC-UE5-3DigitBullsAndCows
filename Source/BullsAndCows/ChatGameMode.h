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
	// 클라이언트가 서버에 로그인하고 나면 호출됨 => 현재 플레이어 수 늘리기 좋음
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
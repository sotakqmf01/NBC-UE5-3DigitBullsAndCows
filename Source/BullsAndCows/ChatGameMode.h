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

// PlayerController가 생성될 때, PlayerState도 생성되지만
// 실수나 특정 상황에 PlayerState가 없을 수 있음
// 그 때를 대비한 타이머의 델리게이트 함수
//void InitPlayerStateAfterDelay(APlayerController* NewPlayer);
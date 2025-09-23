
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ChatGameState.generated.h"

UCLASS()
class BULLSANDCOWS_API AChatGameState : public AGameState
{
	GENERATED_BODY()

public:
	AChatGameState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void GameStateClear();
	void DecideFirstTurn();
	void StartTurn();
	void TimeOutPassTurn();
	void PassTurn();
	void EndTurn();

	// 피드백
	// - StartTurn 함수에서 타이머 정리하는 로직이 보이질 않는데 추가만 해주시면 좋을 것 같습니다!
	// ▶ 매번 /start로 시작할 때 타이머 정리 한번 해주는게 좋아보이긴 함
	void ClearTurnTimers();

	void UpdateTurnPlayerUI();	// ClientRPC로 Turn UI 업데이트
	void UpdateTimerUI();	// MulticastRPC로 Timer UI 업데이트

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateTimerUI(float RemainingTime);	// MulticastRPC로 Timer UI 업데이트

public:
	// 플레이어들이 answer랑 bulls, cows를 알면 안될 것 같음 => 리플리케이션 X
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BullsAndCows")
	FString Answer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BullsAndCows")
	int32 Bulls;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BullsAndCows")
	int32 Cows;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BullsAndCows")
	bool Out;

	// 턴
	UPROPERTY(Replicated)
	int32 CurrentTurnPlayerIndex;

	float TimeLimit;

private:
	FTimerHandle TurnTimerHandle;
	FTimerHandle UIUpdateTimerHandle;
};

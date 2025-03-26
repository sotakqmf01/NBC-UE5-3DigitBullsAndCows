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

	void UpdateTurnPlayerUI();	// ClientRPC�� Turn UI ������Ʈ
	void UpdateTimerUI();	// MulticastRPC�� Timer UI ������Ʈ

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateTimerUI(float RemainingTime);	// MulticastRPC�� Timer UI ������Ʈ

public:
	// �÷��̾���� answer�� bulls, cows�� �˸� �ȵ� �� ���� => ���ø����̼� X
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BullsAndCows")
	FString Answer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BullsAndCows")
	int32 Bulls;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BullsAndCows")
	int32 Cows;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BullsAndCows")
	bool Out;

	// ��
	UPROPERTY(Replicated)
	int32 CurrentTurnPlayerIndex;

	float TimeLimit;

private:
	FTimerHandle TurnTimerHandle;
	FTimerHandle UIUpdateTimerHandle;
};

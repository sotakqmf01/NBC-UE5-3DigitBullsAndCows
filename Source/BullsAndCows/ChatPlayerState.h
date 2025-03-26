#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ChatPlayerState.generated.h"

UCLASS()
class BULLSANDCOWS_API AChatPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AChatPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void DecreaseAttemptCount();
	void SetAttemptCount(int32 NewAttemptCount);

public:
	UPROPERTY(Replicated)
	FName PlayerName;
	UPROPERTY(Replicated)
	int32 AttemptCount;
};

#include "ChatPlayerState.h"
#include "Net/UnrealNetwork.h"

AChatPlayerState::AChatPlayerState() :
	PlayerName(""),
	AttemptCount(0)
{
	// 기본
	//bReplicates = true;
}

void AChatPlayerState::DecreaseAttemptCount()
{
	AttemptCount = FMath::Max(AttemptCount - 1, 0);
}

void AChatPlayerState::SetAttemptCount(int32 NewAttemptCount)
{
	AttemptCount = NewAttemptCount;
}

void AChatPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// AChatPlayerState 클래스의 PlayerName과 AttemptCount를 리플리케이션 할 수 있도록
	DOREPLIFETIME(AChatPlayerState, PlayerName);
	DOREPLIFETIME(AChatPlayerState, AttemptCount);
}

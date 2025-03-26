#include "ChatPlayerState.h"
#include "Net/UnrealNetwork.h"

AChatPlayerState::AChatPlayerState() :
	PlayerName(""),
	AttemptCount(0)
{
	// �⺻
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
	
	// AChatPlayerState Ŭ������ PlayerName�� AttemptCount�� ���ø����̼� �� �� �ֵ���
	DOREPLIFETIME(AChatPlayerState, PlayerName);
	DOREPLIFETIME(AChatPlayerState, AttemptCount);
}

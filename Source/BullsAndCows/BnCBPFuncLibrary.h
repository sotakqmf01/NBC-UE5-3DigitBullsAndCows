#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BnCBPFuncLibrary.generated.h"

UCLASS()
class BULLSANDCOWS_API UBnCBPFuncLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// ������ ���� ����(���ڿ��� ��ȯ)
	UFUNCTION(BlueprintCallable, Category = "BullsAndCows")
	static FString GenerateRandomNumber();
	// Ŭ���̾�Ʈ ������ ���� ��
	UFUNCTION(BlueprintCallable, Category = "BullsAndCows")
	static void EvaluateBullsAndCows(const FString& guess, const FString& answer, UPARAM(ref)int32& OutBulls, UPARAM(ref)int32& OutCows, UPARAM(ref)bool& OutOUT);
	// Ŭ���̾�Ʈ ���� ���ڿ��� ��ȿ�� �Է����� Ȯ��
	static bool IsValidInput(const FString& guess, const FString& answer);
};

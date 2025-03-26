#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BnCBPFuncLibrary.generated.h"

UCLASS()
class BULLSANDCOWS_API UBnCBPFuncLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// 랜덤한 숫자 생성(문자열로 반환)
	UFUNCTION(BlueprintCallable, Category = "BullsAndCows")
	static FString GenerateRandomNumber();
	// 클라이언트 예측과 정답 비교
	UFUNCTION(BlueprintCallable, Category = "BullsAndCows")
	static void EvaluateBullsAndCows(const FString& guess, const FString& answer, UPARAM(ref)int32& OutBulls, UPARAM(ref)int32& OutCows, UPARAM(ref)bool& OutOUT);
	// 클라이언트 예측 문자열이 유효한 입력인지 확인
	static bool IsValidInput(const FString& guess, const FString& answer);
};

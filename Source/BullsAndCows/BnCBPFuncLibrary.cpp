#include "BnCBPFuncLibrary.h"
#include "Misc/DefaultValueHelper.h"

FString UBnCBPFuncLibrary::GenerateRandomNumber()
{
	int32 num = 0;
	TArray<int32> arr = { 1,2,3,4,5,6,7,8,9 };

	for (int32 i = 0; i < 3; i++)
	{
		int32 idx = FMath::RandRange(0, arr.Num()-1);
		num = num * 10 + arr[idx];

		arr.RemoveAt(idx);
	}

	return FString::FromInt(num);
}

void UBnCBPFuncLibrary::EvaluateBullsAndCows(const FString& guess, const FString& answer, UPARAM(ref)int32& OutBulls, UPARAM(ref)int32& OutCows, UPARAM(ref)bool& OutOUT)
{
	OutBulls = 0;
	OutCows = 0;
	OutOUT = false;

	// ���� ������ ���� ������, '/'�� ���� ���ؾ���
	if (!IsValidInput(guess, answer))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Wrong Input")));
		OutOUT = true;
		return;
	}

	for (int32 i = 0; i < answer.Len(); i++)
	{
		for (int32 j = 0; j < answer.Len(); j++)
		{
			if (guess[i + 1] == answer[j])
			{
				if (i == j) 
				{
					OutBulls++;
				}
				else
				{
					OutCows++;
				}
			}
		}
	}
}

bool UBnCBPFuncLibrary::IsValidInput(const FString& guess, const FString& answer)
{
	// ���� ���� '/' ����
	// Mid(n) : n��° �ε������� ������ �ڸ� Substring
	FString GuessNum = guess.Mid(1);
	int32 ParsedNum;

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Guess Num = %s"), *GuessNum));

	// ���� ���̺��� ��� false
	if (GuessNum.Len() != answer.Len())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("The length of the input number and the correct number are not the same")));
		return false;
	}

	// ���ڷ� ��ȯ ���ϸ� false
	if (!FDefaultValueHelper::ParseInt(GuessNum, ParsedNum))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Can't Convert to Int")));
		return false;
	}

	// �ߺ� ������ false
	TSet<TCHAR> GuessNumDigits;
	for (TCHAR Char : GuessNum)
	{
		if (GuessNumDigits.Contains(Char))
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Duplicated Input")));
			return false;
		}

		GuessNumDigits.Add(Char);
	}

	return true;
}

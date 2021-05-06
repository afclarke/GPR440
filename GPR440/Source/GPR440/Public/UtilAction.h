// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UtilPerception.h"
#include "UtilAction.generated.h"

class AUtilAgent;

UENUM(BlueprintType)
enum class EUtilActionType : uint8
{
	INVALID = 0,
	HIDE,
	EXPLORE,
	FLOCK,
	EXTEND,
	EAT,
	ATTACK,
	IDLE,
	DEFEND
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActDelegate, float, DeltaTime);

/**
 *
 */
UCLASS(Abstract, Blueprintable)
class GPR440_API UUtilAction : public UObject
{
	GENERATED_BODY()
		
public:
	void Init();
	void Cleanup();
	
	// normal, multiplicative evaluation of considerations
	Utility Evaluate(AUtilAgent* pAgent, AUtilGameMode* pGameMode);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EUtilActionType mActionType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UUtilPerception>> mConsiderations;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UUtilPerception>> mProbabilities;

	UPROPERTY(BlueprintAssignable)
	FActDelegate mAct;
	
	// ideally, for efficiency & data-driven, this would be an array of struct objects,
	// but USTRUCT polymorphism isn't possible and all Util should be BP-able
	UPROPERTY(BlueprintReadOnly)
	TArray<UUtilPerception*> mConsiderationObjs;
	UPROPERTY(BlueprintReadOnly)
	TArray<UUtilPerception*> mProbabilityObjs;

	UPROPERTY(BlueprintReadOnly)
	float mUtilityCache;
	UPROPERTY(BlueprintReadOnly)
	bool mChosenCache;
	
};

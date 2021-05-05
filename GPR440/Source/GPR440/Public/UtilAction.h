// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UtilConsideration.h"
#include "UtilAction.generated.h"

class AUtilAgent;

UENUM(BlueprintType)
enum class EUtilActionType : uint8
{
	INVALID = 0,
	HIDE,
	EXPLORE,
	FLOCK,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActDelegate);

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
	TArray<TSubclassOf<UUtilConsideration>> mConsiderations;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UUtilConsideration>> mProbabilities;

	UPROPERTY(BlueprintAssignable)
	FActDelegate mAct;
	
	// ideally, for efficiency & data-driven, this would be an array of struct objects,
	// but USTRUCT polymorphism isn't possible and all Util should be BP-able
	UPROPERTY(BlueprintReadOnly)
	TArray<UUtilConsideration*> mConsiderationObjs;
	UPROPERTY(BlueprintReadOnly)
	TArray<UUtilConsideration*> mProbabilityObjs;

	UPROPERTY(BlueprintReadOnly)
	float mUtilityCache;
	UPROPERTY(BlueprintReadOnly)
	bool mChosenCache;
	
};

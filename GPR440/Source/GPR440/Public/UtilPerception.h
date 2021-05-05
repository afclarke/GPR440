// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UtilPerception.generated.h"

#define Utility float
class AUtilAgent;
class AUtilGameMode;

/**
 *
 */
UCLASS(Abstract, Blueprintable)
class GPR440_API UUtilPerception : public UObject
{
	GENERATED_BODY()

public:
	Utility consider(AUtilAgent* pAgent, AUtilGameMode* pGameMode);

protected:
	// perceive normalized knowledge
	UFUNCTION(BlueprintImplementableEvent)
	float perceive(AUtilAgent* pAgent, AUtilGameMode* pGameMode) const;
	//PURE_VIRTUAL(UUtilConsideration::consider, return -1;)

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* mCurve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor mCurveColor = FLinearColor::White;
	
	UPROPERTY(BlueprintReadOnly)
	float mUtilityCache;
	UPROPERTY(BlueprintReadOnly)
	float mNormalizedInputCache;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UtilConsideration.generated.h"

#define Utility float
class AUtilAgent;

/**
 *
 */
UCLASS(Abstract, Blueprintable)
class GPR440_API UUtilConsideration : public UObject
{
	GENERATED_BODY()

public:
	Utility consider(AUtilAgent* pAgent) const;

protected:
	// perceive normalized knowledge
	UFUNCTION(BlueprintImplementableEvent)
	float perceive(AUtilAgent* pAgent) const;
	//PURE_VIRTUAL(UUtilConsideration::consider, return -1;)

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCurveFloat* mCurve;

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilAction.h"

PRAGMA_DISABLE_OPTIMIZATION
void UUtilAction::Init()
{
	mConsiderationObjs.Empty();
	for (TSubclassOf<UUtilConsideration> considerationClass : mConsiderations)
	{
		UUtilConsideration* newConsideration = NewObject<UUtilConsideration>(
			this, considerationClass);
		mConsiderationObjs.Add(newConsideration);
	}
	mProbabilityObjs.Empty();
	for (TSubclassOf<UUtilConsideration> considerationClass : mProbabilities)
	{
		UUtilConsideration* newProbability = NewObject<UUtilConsideration>(
			this, considerationClass);
		mConsiderationObjs.Add(newProbability);
	}
}

void UUtilAction::Cleanup()
{
	mConsiderationObjs.Empty();
	mProbabilityObjs.Empty();
}

Utility UUtilAction::Evaluate(AUtilAgent* pAgent, AUtilGameMode* pGameMode)
{
	// average considerations
	Utility utility = 0.f;
	for (UUtilConsideration* consideration : mConsiderationObjs)
	{
		utility += consideration->consider(pAgent, pGameMode);
	}
	int32 numConsiderations = mConsiderationObjs.Num();
	if(numConsiderations > 0)
	{
		utility /= numConsiderations;
	}
	// multiply by probabilities
	for (UUtilConsideration* probability : mProbabilityObjs)
	{
		utility *= probability->consider(pAgent, pGameMode);
	}
	// cache and return
	mUtilityCache = utility;
	return utility;
}
PRAGMA_ENABLE_OPTIMIZATION

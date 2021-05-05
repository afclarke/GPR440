// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilAction.h"

PRAGMA_DISABLE_OPTIMIZATION
void UUtilAction::Init()
{
	mConsiderationObjs.Empty();
	for (TSubclassOf<UUtilPerception> perceptionClass : mConsiderations)
	{
		UUtilPerception* newConsideration = NewObject<UUtilPerception>(
			this, perceptionClass);
		mConsiderationObjs.Add(newConsideration);
	}
	mProbabilityObjs.Empty();
	for (TSubclassOf<UUtilPerception> perceptionClass : mProbabilities)
	{
		UUtilPerception* newProbability = NewObject<UUtilPerception>(
			this, perceptionClass);
		mProbabilityObjs.Add(newProbability);
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
	for (UUtilPerception* consideration : mConsiderationObjs)
	{
		utility += consideration->consider(pAgent, pGameMode);
	}
	int32 numConsiderations = mConsiderationObjs.Num();
	if(numConsiderations > 0)
	{
		utility /= numConsiderations;
	}
	// multiply by probabilities
	for (UUtilPerception* probability : mProbabilityObjs)
	{
		utility *= probability->consider(pAgent, pGameMode);
	}
	// cache and return
	mUtilityCache = utility;
	return utility;
}
PRAGMA_ENABLE_OPTIMIZATION

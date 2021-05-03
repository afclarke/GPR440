// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilAction.h"

//UUtilAction* UUtilAction::Construct(EUtilActionType actionType, TArray<UUtilConsideration*> considerations)
//{
//	mActionType = actionType;
//	mConsiderations = considerations;
//	return this;
//}
PRAGMA_DISABLE_OPTIMIZATION
void UUtilAction::Init()
{
	mConsiderationObjs.Empty();
	mConsiderationObjs.SetNum(0);
	for (TSubclassOf<UUtilConsideration> considerationClass : mConsiderations)
	{
		UUtilConsideration* newConsideration = NewObject<UUtilConsideration>(this, considerationClass);
		//UUtilConsideration* newConsideration = NewObject<UUtilConsideration>(
		//	this, considerationClass->StaticClass());
		mConsiderationObjs.Add(newConsideration);
	}
}

void UUtilAction::Cleanup()
{
	mConsiderationObjs.Empty();
}

Utility UUtilAction::evaluate(AUtilAgent* pAgent, AUtilGameMode* pGameMode)
{
	Utility utility = 1.f;
	for (int i = 0; i < mConsiderations.Num(); i++)
	{
		utility *= mConsiderationObjs[i]->consider(pAgent, pGameMode);
	}
	mUtilityCache = utility;
	return utility;
}
PRAGMA_ENABLE_OPTIMIZATION

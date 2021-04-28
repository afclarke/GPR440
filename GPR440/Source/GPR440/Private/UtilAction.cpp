// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilAction.h"

//UUtilAction* UUtilAction::Construct(EUtilActionType actionType, TArray<UUtilConsideration*> considerations)
//{
//	mActionType = actionType;
//	mConsiderations = considerations;
//	return this;
//}
void UUtilAction::Init()
{
	for (TSubclassOf<UUtilConsideration> considerationClass : mConsiderations)
	{
		UUtilConsideration* newConsideration = Cast<UUtilConsideration>(
			considerationClass->ClassDefaultObject);
		//UUtilConsideration* newConsideration = NewObject<UUtilConsideration>(
		//	this, considerationClass->StaticClass());
		mConsiderationObjs.Add(newConsideration);
	}
}

Utility UUtilAction::evaluate(AUtilAgent* pAgent)
{
	Utility utility = 1.f;
	for (int i = 0; i < mConsiderations.Num(); i++)
	{
		utility *= mConsiderationObjs[i]->consider(pAgent);
	}
	return utility;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilDeciderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UtilGameMode.h"

PRAGMA_DISABLE_OPTIMIZATION
UUtilDeciderComponent::UUtilDeciderComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

//void UUtilDeciderComponent::Construct(EUtilDecisionMethod decisionMethod, TArray<UUtilAction*> actions)
//{
//	mDecisionMethod = decisionMethod;
//	mActions = actions;
//}

EUtilActionType UUtilDeciderComponent::decide(AUtilAgent* pAgent)
{
	AUtilGameMode* pGameMode = Cast<AUtilGameMode>(
		UGameplayStatics::GetGameMode(GetWorld()));
	
	EUtilActionType decidedAction = EUtilActionType::INVALID;
	switch (mDecisionMethod)
	{
	case EUtilDecisionMethod::GREATEST:
	{
		Utility greatestUtil = -1;
		for (UUtilAction* action : mActionObjs)
		{
			Utility actionUtil = action->evaluate(pAgent, pGameMode);
			if(actionUtil >= greatestUtil)
			{
				greatestUtil = actionUtil;
				decidedAction = action->mActionType;
			}
		}
		break;
	}
	case EUtilDecisionMethod::WEIGHTED_RANDOM:
	{
		break;
	}
	}
	return decidedAction;
}


void UUtilDeciderComponent::BeginPlay()
{
	Super::BeginPlay();

	mActionObjs.Empty();
	mActionObjs.SetNum(0);
	for (TSubclassOf<UUtilAction> actionClass : mActions)
	{
		
		UUtilAction* newAction = NewObject<UUtilAction>(this, actionClass);
		//UUtilAction* newAction = NewObject<UUtilAction>(
		//	this, actionClass->StaticClass());
		newAction->Init();
		mActionObjs.Add(newAction);
	}
}

void UUtilDeciderComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (UUtilAction* actionObj : mActionObjs)
	{
		actionObj->Cleanup();
	}
	mActionObjs.Empty();
}


void UUtilDeciderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
PRAGMA_ENABLE_OPTIMIZATION

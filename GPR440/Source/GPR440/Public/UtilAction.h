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

/**
 *
 */
UCLASS(Abstract, Blueprintable)
class GPR440_API UUtilAction : public UObject
{
	GENERATED_BODY()
		
public:
	//UUtilAction(TArray<UUtilConsideration> considerations, void(AAgent::* act)())
	//	:mConsiderations(considerations), mAct(act)
	//{
	//}

	//UFUNCTION(BlueprintCallable)
	//UUtilAction* Construct(EUtilActionType actionType, TArray<UUtilConsideration*> considerations);

	void Init();
	void Cleanup();
	
	// normal, multiplicative evaluation of considerations
	Utility evaluate(AUtilAgent* pAgent, AUtilGameMode* pGameMode);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EUtilActionType mActionType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UUtilConsideration>> mConsiderations;

	// ideally, we'd have void function pointers for the action,
	// instead of an enum which puts the onus of execution on the agent
	//void(AAgent::* mAct)();
	//FActDelegate mAct;
	
	// ideally, for efficiency, this would be an array of struct objects,
	// but USTRUCT polymorphism isn't possible and all Util should be BP-able
	UPROPERTY(BlueprintReadOnly)
	TArray<UUtilConsideration*> mConsiderationObjs;

	UPROPERTY(BlueprintReadOnly)
	float mUtilityCache;
	UPROPERTY(BlueprintReadOnly)
	bool mChosenCache;
};

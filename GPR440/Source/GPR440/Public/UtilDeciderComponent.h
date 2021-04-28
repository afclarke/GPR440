// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UtilAction.h"
#include "UtilDeciderComponent.generated.h"

class AUtilAgent;

UENUM(BlueprintType)
enum class EUtilDecisionMethod : uint8
{
	GREATEST,
	WEIGHTED_RANDOM
};

UCLASS()
class GPR440_API UUtilDeciderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UUtilDeciderComponent();

	//UFUNCTION(BlueprintCallable)
	//void Construct(EUtilDecisionMethod decisionMethod, TArray<UUtilAction*> actions);
	
	EUtilActionType decide(AUtilAgent* pAgent);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUtilDecisionMethod mDecisionMethod;
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UUtilAction>> mActions;

private:
	UPROPERTY()
	TArray<UUtilAction*> mActionObjs;
};

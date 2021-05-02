// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UtilDeciderComponent.h"
#include "UtilAgent.generated.h"

UCLASS(Abstract)
class GPR440_API AUtilAgent : public APawn
{
	GENERATED_BODY()

public:
	AUtilAgent();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float mMaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float mMaxHunger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mHunger;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UUtilDeciderComponent* mpUtilDeciderComponent;

	UFUNCTION(BlueprintImplementableEvent)
	bool Act(EUtilActionType actionType);
	//PURE_VIRTUAL(AUtilAgent::Act)

};

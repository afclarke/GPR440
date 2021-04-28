// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UtilDeciderComponent.h"
#include "UtilAgent.generated.h"

UCLASS(Abstract)
class GPR440_API AUtilAgent : public ACharacter
{
	GENERATED_BODY()

public:
	AUtilAgent();

	UFUNCTION(BlueprintCallable)
	float GetHealth() { return mHealth; }
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float mMaxHealth;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UUtilDeciderComponent* mpUtilDeciderComponent;

	UFUNCTION(BlueprintImplementableEvent)
	bool Act(EUtilActionType actionType);
	//PURE_VIRTUAL(AUtilAgent::Act)

	UPROPERTY(BlueprintReadWrite)
	float mHealth;

};

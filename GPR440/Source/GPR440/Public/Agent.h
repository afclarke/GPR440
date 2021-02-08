// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Agent.generated.h"

UCLASS()
class GPR440_API AAgent : public ACharacter
{
	GENERATED_BODY()

public:
	AAgent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	void Wander();

public:
	void Mutate();
	int32 GetFitness() { return mGoalCount - mCollisionCount; }

protected:
	// Default Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DebugLineThickness = 15.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float WanderMovementRadius = 1000.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float WanderArriveRadius = 75.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MoveInputLerpScalar = 2.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MutationRate = 0.05f;

	// Instance Properties
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	float ForwardLineTraceLength = 500.0f;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	float WhiskerLineTraceLength = 200.0f;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	float WhiskerAngle = 40.0f;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	float ForwardAvoidInputScalar = 20.0f;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	float WhiskerAvoidInputScalar = 50.0f;

	UFUNCTION()
	void OnCollision(AActor* overlappedActor, AActor* otherActor);
	UFUNCTION(BlueprintImplementableEvent)
	void OnCollisionEvent(int32 collisionCount);
	
private:
	UCharacterMovementComponent* mpCharacterMovementComponent;
	FVector mWanderTarget;
	FVector mWanderInput;

	FVector mTarInput;
	FVector mCurInput;

	int32 mCollisionCount;
	int32 mGoalCount;
};

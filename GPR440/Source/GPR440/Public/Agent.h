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
	// Sets default values for this pawn's properties
	AAgent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void Wander();

protected:
	// Default Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DebugLineThickness = 15.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float WanderMovementRadius = 1000.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float WanderArriveRadius = 20.0f;

	// Instance Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ForwardLineTraceLength = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WhiskerLineTraceLength = 200.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WhiskerAngle = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MoveInputLerpScalar = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ForwardAvoidInputScalar = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WhiskerAvoidInputScalar = 20.0f;

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
};

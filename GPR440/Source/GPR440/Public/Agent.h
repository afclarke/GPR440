// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InfluenceMap.h"
#include "Agent.generated.h"

class AFlock;
class AFlowField;

//DECLARE_DELEGATE(FActDelegate)

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
	//FORCEINLINE int32 GetFitness() const { return mGoalCount - mCollisionCount; }
	FORCEINLINE int32 GetFitness() const { return mCollisionCount; }
	FORCEINLINE void SetFlock(AFlock* pFlock) { mpFlock = pFlock; }
	
	UFUNCTION(BlueprintCallable)
	void SetDrawDebug(bool enabled);

	void WriteToSahredInfluenceMaps();
	void ReadInfluenceMaps();

protected:
	UFUNCTION()
	void OnCollision(AActor* overlappedActor, AActor* otherActor);
	UFUNCTION(BlueprintImplementableEvent)
	void OnCollisionEvent(int32 collisionCount);
	UFUNCTION(BlueprintImplementableEvent)
	void SetCollisionCountTextEnabled(bool enabled);

private:
	FVector CalcFlockInput();
	FVector BoidSeparation();
	FVector BoidAlignment();
	FVector BoidCohesion();

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float BoidSeparationWeight = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float BoidAlignmentWeight = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float BoidCohesionWeight = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float BoidSeparationRadius = 100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float BoidAlignmentRadius = 100.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float BoidCohesionRadius = 100.0f;

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

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	int32 ProximityRadius;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	AInfluenceMap* ProximityInfluenceMap;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	AInfluenceMap* EnemyProximityInfluenceMap;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	int32 ThreatRadius;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	AInfluenceMap* ThreatInfluenceMap;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	AInfluenceMap* EnemyThreatInfluenceMap;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	bool Chaser;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	int32 InterestRadius;

private:
	UCharacterMovementComponent* mpCharacterMovementComponent;
	FVector mWanderTarget;
	FVector mWanderInput;

	FVector mTarInput;
	FVector mCurInput;

	int32 mCollisionCount;
	int32 mGoalCount;

	AFlock* mpFlock;
	AFlowField* mpFlowField;

	class AInfluenceMapManager* mpInfluenceMapManager;
	int32 mProximityStampIndex;
	int32 mThreatStampIndex;
	UPROPERTY(VisibleAnywhere)
	AInfluenceMap* mpWorkingMap;

	FVector mMoveVector;
	
	bool mDrawDebug = true;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuadTree.h"
#include "Flock.generated.h"

class AAgent;

UCLASS()
class GPR440_API AFlock : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlock();

protected:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	void Tick(float DeltaTime) override;

	TArray<AActor*> GetNeighborhood(AActor* pActor, float radius) const;

	UFUNCTION(BlueprintCallable)
	void SetDrawDebug(bool enabled);
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAgent> BoidClassBP;
	UPROPERTY(EditAnywhere)
	float BoidSpawnRadius = 1000.0f;
	UPROPERTY(EditAnywhere)
	int32 FlockSize = 20;
	UPROPERTY(EditAnywhere)
	FBox2D QuadTreeRect;

private:
	TArray<AAgent*> mBoids;
	QuadTree* mpBoidsQuadTree;

	bool mDrawDebug = true;
};

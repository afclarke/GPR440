// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Flock.generated.h"

class AAgent;

UCLASS()
class GPR440_API AFlock : public AActor
{
	GENERATED_BODY()
	
public:	
	AFlock();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	TArray<AAgent*> GetNeighborhood(FVector center, float radius) const;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAgent> BoidClassBP;
	UPROPERTY(EditDefaultsOnly)
	float BoidSpawnRadius = 1000.0f;
	UPROPERTY(EditDefaultsOnly)
	int32 FlockSize = 20;

private:
	UPROPERTY()
	TArray<AAgent*> mBoids;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Agent.h"
#include "ObstacleAvoidanceGenetics.generated.h"

/**
 * 
 */
UCLASS()
class GPR440_API AObstacleAvoidanceGenetics : public AActor
{
	GENERATED_BODY()

public:
	AObstacleAvoidanceGenetics();

protected:
	void BeginPlay() override;

public:
	void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AAgent> AgentClassBP;
	UPROPERTY(EditAnywhere)
	int32 GenerationSize = 20;
	UPROPERTY(EditAnywhere)
	float AgentSpawnRadius = 1000.0f;
	UPROPERTY(EditAnywhere)
	int32 GenerationTickCount = 10000;
	UPROPERTY(EditAnywhere)
	int32 NumGenerations = 20;
	
private:
	UPROPERTY()
	TArray<AAgent*> mpAgents;

	int32 mTickCount;
	int32 mGenerationCount;

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "ObstacleAvoidanceGenetics.h"

AObstacleAvoidanceGenetics::AObstacleAvoidanceGenetics()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AObstacleAvoidanceGenetics::BeginPlay()
{
	Super::BeginPlay();

	UWorld* pWorld = GetWorld();
	for(int i = 0; i < GenerationSize; i++)
	{
		FVector spawnLoc = FMath::VRand() * AgentSpawnRadius;
		spawnLoc.Z = 230;
		mpAgents.Add(pWorld->SpawnActor<AAgent>(AgentClassBP.Get(), spawnLoc, FRotator::ZeroRotator));
	}
	mTickCount = 0;
	mGenerationCount = 0;
}

void AObstacleAvoidanceGenetics::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(mTickCount++ >= GenerationTickCount)
	{
		mTickCount = 0;
		mGenerationCount++;

		if(mGenerationCount >= NumGenerations)
		{
			
		}
		else
		{
			for (AAgent* agent : mpAgents)
			{
				agent->Destroy();
			}
		}
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Flock.h"
#include "Agent.h"

AFlock::AFlock()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AFlock::BeginPlay()
{
	Super::BeginPlay();

	// spawn boids in random locations
	UWorld* pWorld = GetWorld();
	for(int32 i = 0; i < FlockSize; i++)
	{
		FVector spawnLoc = FMath::VRand() * BoidSpawnRadius;
		spawnLoc.Z = 230;
		AAgent* pNewBoid = pWorld->SpawnActor<AAgent>(BoidClassBP.Get(), spawnLoc,
			FRotator::ZeroRotator);
		pNewBoid->SetFlock(this);
		mBoids.Add(pNewBoid);
	}
}

void AFlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TArray<AAgent*> AFlock::GetNeighborhood(FVector center, float radius) const
{
	float sqrRadius = radius * radius;
	
	TArray<AAgent*> neighborhood;
	for (AAgent* pBoid : mBoids)
	{
		if((pBoid->GetActorLocation() - center).SizeSquared() <= sqrRadius)
		{
			neighborhood.Add(pBoid);
		}
	}

	return neighborhood;
}


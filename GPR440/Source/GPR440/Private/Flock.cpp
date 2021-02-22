// Fill out your copyright notice in the Description page of Project Settings.


#include "Flock.h"
#include "Agent.h"

PRAGMA_DISABLE_OPTIMIZATION
AFlock::AFlock()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFlock::BeginPlay()
{
	Super::BeginPlay();

	mpBoidsQuadTree = new QuadTree(0, QuadTreeRect);

	// spawn boids in random locations
	UWorld* pWorld = GetWorld();
	for (int32 i = 0; i < FlockSize; i++)
	{
		FVector spawnLoc = FMath::VRand() * BoidSpawnRadius;
		spawnLoc.Z = 230;
		AAgent* pNewBoid = pWorld->SpawnActor<AAgent>(BoidClassBP.Get(), spawnLoc,
			FRotator::ZeroRotator);
		pNewBoid->SetFlock(this);
		mBoids.Add(pNewBoid);
		mpBoidsQuadTree->Insert(pNewBoid);
	}
}

void AFlock::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	delete mpBoidsQuadTree;
	mpBoidsQuadTree = nullptr;
}

void AFlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(mUseQuadTree)
	{
		// re-build quadtree
		mpBoidsQuadTree->Clear();
		for (AAgent* boid : mBoids)
		{
			mpBoidsQuadTree->Insert(boid);
		}
		// draw quadtree debug
		if (mDrawDebug)
		{
			mpBoidsQuadTree->Draw(GetWorld());
		}
	}
}

TArray<AActor*> AFlock::GetNeighborhood(AActor* pActor, float radius) const
{
	const float sqrRadius = radius * radius;

	if(mUseQuadTree)
	{
		return mpBoidsQuadTree->QuerySqrRadius(pActor, sqrRadius);
	}

	TArray<AActor*> neighborhood;
	for (AActor* pBoid : mBoids)
	{
		if (pBoid == pActor) continue;
		if ((pBoid->GetActorLocation() - pActor->GetActorLocation()).SizeSquared() <= sqrRadius)
		{
			neighborhood.Add(pBoid);
		}
	}
	return neighborhood;
}

void AFlock::SetDrawDebug(bool enabled)
{
	mDrawDebug = enabled;
	for (AAgent* pBoid : mBoids)
	{
		pBoid->SetDrawDebug(enabled);
	}
}

PRAGMA_ENABLE_OPTIMIZATION

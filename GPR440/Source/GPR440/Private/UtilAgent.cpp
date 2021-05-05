// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilAgent.h"
#include "UtilGameMode.h"
#include "Kismet/GameplayStatics.h"

PRAGMA_DISABLE_OPTIMIZATION
AUtilAgent::AUtilAgent()
{
	PrimaryActorTick.bCanEverTick = true;

	mpRootMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	SetRootComponent(mpRootMesh);
}

FVector2D AUtilAgent::Flock(float separationRadiusSqr, float alignmentRadiusSqr, float cohesionRadiusSqr,
	float separationWeight, float alignmentWeight, float cohesionWeight, FName withTag)
{
	AUtilGameMode* pGameMode = Cast<AUtilGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	FVector2D loc = FVector2D(GetActorLocation());

	// separation
	TArray<AActor*> separationNeighborhood = pGameMode->mpQuadTree->QuerySqrRadius(
		this, separationRadiusSqr, withTag);
	FVector2D separation = FVector2D::ZeroVector;
	if (separationNeighborhood.Num() > 0)
	{
		for (AActor* pBoid : separationNeighborhood)
		{
			//if (pBoid == this) continue;
			separation += loc - FVector2D(pBoid->GetActorLocation());
		}
		separation /= separationNeighborhood.Num();
		separation.Normalize();
	}
	
	// alignment
	TArray<AActor*> alignmentNeighborhood = pGameMode->mpQuadTree->QuerySqrRadius(
		this, alignmentRadiusSqr, withTag);
	FVector2D alignment = FVector2D::ZeroVector;
	if (alignmentNeighborhood.Num() > 0)
	{
		for (AActor* pBoid : alignmentNeighborhood)
		{
			//if (pBoid == this) continue;
			alignment += FVector2D(pBoid->GetVelocity()).GetSafeNormal();
		}
		alignment /= alignmentNeighborhood.Num();
		alignment.Normalize();
	}

	// cohesion
	TArray<AActor*> cohesionNeighborhood = pGameMode->mpQuadTree->QuerySqrRadius(
		this, cohesionRadiusSqr, withTag);
	FVector2D cohesion = FVector2D::ZeroVector;
	if (cohesionNeighborhood.Num() > 0)
	{
		FVector2D avgNeighborhoodLoc = FVector2D::ZeroVector;
		for (AActor* pBoid : cohesionNeighborhood)
		{
			//if (pBoid == this) continue;
			avgNeighborhoodLoc += FVector2D(pBoid->GetActorLocation());
		}
		avgNeighborhoodLoc /= cohesionNeighborhood.Num();

		cohesion = avgNeighborhoodLoc - loc;
		cohesion.Normalize();
	}

	// normalize weights
	float totalWeight = separationWeight + alignmentWeight + cohesionWeight;
	separationWeight /= totalWeight;
	alignmentWeight /= totalWeight;
	cohesionWeight /= totalWeight;
	// final flock vector
	FVector2D flock = separation * separationWeight + alignment * alignmentWeight
		+ cohesion * cohesionWeight;
	return flock;
}

void AUtilAgent::BeginPlay()
{
	Super::BeginPlay();
	
	Init();
}

void AUtilAgent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	Cleanup();
}

void AUtilAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UUtilAction* chosenAction = Decide();
	if(chosenAction)
	{
		chosenAction->mAct.Broadcast();
	}
}

void AUtilAgent::Init()
{
	mActionObjs.Empty();
	mActionObjs.SetNum(0);
	for (TSubclassOf<UUtilAction> actionClass : mActions)
	{
		UUtilAction* newAction = NewObject<UUtilAction>(this, actionClass);
		//UUtilAction* newAction = NewObject<UUtilAction>(
		//	this, actionClass->StaticClass());
		newAction->Init();
		mActionObjs.Add(newAction);
	}
	BindActs(mActionObjs);
	InitUtilWidget(mActionObjs);
}

void AUtilAgent::Cleanup()
{
	AActor* pOwner = GetOwner();
	for (UUtilAction* actionObj : mActionObjs)
	{
		// remove owner's bound actions
		actionObj->mAct.RemoveAll(pOwner);
		actionObj->Cleanup();
	}
	mActionObjs.Empty();
}

UUtilAction* AUtilAgent::Decide()
{
	AUtilGameMode* pGameMode = Cast<AUtilGameMode>(
		UGameplayStatics::GetGameMode(GetWorld()));

	UUtilAction* decidedAction = nullptr;
	switch (mDecisionMethod)
	{
	case EUtilDecisionMethod::GREATEST:
	{
		Utility greatestUtil = -1;
		for (UUtilAction* action : mActionObjs)
		{
			action->mChosenCache = false;
			Utility actionUtil = action->evaluate(this, pGameMode);
			if (actionUtil >= greatestUtil)
			{
				greatestUtil = actionUtil;
				decidedAction = action;
			}
		}
		break;
	}
	case EUtilDecisionMethod::WEIGHTED_RANDOM:
	{
		break;
	}
	}

	if (decidedAction)
	{
		decidedAction->mChosenCache = true;
	}
	return decidedAction;
}

PRAGMA_ENABLE_OPTIMIZATION

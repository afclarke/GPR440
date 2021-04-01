// Fill out your copyright notice in the Description page of Project Settings.


#include "Agent.h"
#include "DrawDebugHelpers.h"
#include "AIC_Agent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Flock.h"
#include "FlowField.h"
#include "InfluenceMapManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAgent::AAgent()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIC_Agent::StaticClass();
	ACharacter::bUseControllerRotationYaw = true;

	PrimaryActorTick.bCanEverTick = true;

	mpCharacterMovementComponent = GetCharacterMovement();
	mpCharacterMovementComponent->bOrientRotationToMovement = true;
}

PRAGMA_DISABLE_OPTIMIZATION
// Called when the game starts or when spawned
void AAgent::BeginPlay()
{
	Super::BeginPlay();

	mpInfluenceMapManager = Cast<AInfluenceMapManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AInfluenceMapManager::StaticClass()));
	mWanderTarget = GetActorLocation();

	mCollisionCount = 0;
	mGoalCount = -1;
	OnActorBeginOverlap.AddDynamic(this, &AAgent::OnCollision);
	mTarInput = FVector::ZeroVector;
	//mCurInput = FMath::VRand();
	mCurInput = FVector::ZeroVector;

	
	mpInfluenceMapManager->RegisterAgent(this);
	
	mProximityStampIndex = AInfluenceMap::GenerateStamp(EStampFunc::LINEAR, uint32(ProximityRadius));
	mThreatStampIndex = AInfluenceMap::GenerateStamp(EStampFunc::LINEAR, uint32(ThreatRadius));

	
	mpWorkingMap = GetWorld()->SpawnActor<AInfluenceMap>();
	mpWorkingMap->Construct(ProximityInfluenceMap, FVector2D(0,0),
		ProximityRadius * 2, ProximityRadius * 2);
	mpInfluenceMapManager->RegisterMap(mpWorkingMap);
}
PRAGMA_ENABLE_OPTIMIZATION

// Called every frame
void AAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAgent::Wander()
{
	FVector curLocation = GetActorLocation();
	FVector wanderDir = mWanderTarget - curLocation;
	float wanderDist = wanderDir.Size();

	// arrived at wander target, get new random target
	if (wanderDist <= WanderArriveRadius)
	{
		mGoalCount++;
		OnCollisionEvent(GetFitness());

		mWanderTarget = FMath::VRand() * WanderMovementRadius;
		mWanderTarget.Z = curLocation.Z;
		wanderDir = mWanderTarget - curLocation;
	}

	// move to wander target
	mWanderInput = wanderDir.GetSafeNormal();
	mTarInput += mWanderInput;

	DrawDebugSphere(GetWorld(), mWanderTarget, WanderArriveRadius, 100, FColor::Magenta);
}

void AAgent::Mutate()
{

}

void AAgent::SetDrawDebug(bool enabled)
{
	mDrawDebug = enabled;
	SetCollisionCountTextEnabled(enabled);
}

void AAgent::WriteToSahredInfluenceMaps()
{
	FVector2D locationCoords = ProximityInfluenceMap->GetGridCoordsFromWorldLoc(GetActorLocation());
	ProximityInfluenceMap->ApplyStamp(mProximityStampIndex, locationCoords);
	ThreatInfluenceMap->ApplyStamp(mThreatStampIndex, locationCoords);
}

void AAgent::ReadInfluenceMaps()
{
	FVector2D locationCoords = ProximityInfluenceMap->GetGridCoordsFromWorldLoc(GetActorLocation());
	mpWorkingMap->SetCenterCoords(locationCoords);
	FVector moveVector;
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + moveVector, FColor::Green, false, -1, 0, 15);
	AddMovementInput(moveVector.GetSafeNormal());
}

void AAgent::OnCollision(AActor* overlappedActor, AActor* otherActor)
{
	mCollisionCount++;
	OnCollisionEvent(GetFitness());
}

PRAGMA_DISABLE_OPTIMIZATION
FVector AAgent::CalcFlockInput()
{
	if (!mpFlock)
	{
		return FVector::ZeroVector;
	}

	FVector flockInput = BoidSeparation() * BoidSeparationWeight
		+ BoidAlignment() * BoidAlignmentWeight
		+ BoidCohesion() * BoidCohesionWeight;
	flockInput.Normalize();
	return flockInput;
}

FVector AAgent::BoidSeparation()
{
	FVector separation = FVector::ZeroVector;
	FVector loc = GetActorLocation();

	TArray<AActor*> neighborhood = mpFlock->GetNeighborhood(this, BoidSeparationRadius);
	//if (neighborhood.Num() <= 1) return FVector::ZeroVector;
	if (neighborhood.Num() <= 0) return FVector::ZeroVector;

	for (AActor* pBoid : neighborhood)
	{
		//if (pBoid == this) continue;

		separation += loc - pBoid->GetActorLocation();
	}
	separation /= neighborhood.Num();
	separation.Normalize();

	return separation;
}

FVector AAgent::BoidAlignment()
{
	FVector alignment = FVector::ZeroVector;

	TArray<AActor*> neighborhood = mpFlock->GetNeighborhood(this, BoidAlignmentRadius);
	//if (neighborhood.Num() <= 1) return FVector::ZeroVector;
	if (neighborhood.Num() <= 0) return FVector::ZeroVector;

	for (AActor* pBoid : neighborhood)
	{
		//if (pBoid == this) continue;

		alignment += pBoid->GetVelocity().GetSafeNormal();
	}
	alignment /= neighborhood.Num();
	alignment.Normalize();

	return alignment;
}

FVector AAgent::BoidCohesion()
{
	FVector avgNeighborhoodLoc = FVector::ZeroVector;

	TArray<AActor*> neighborhood = mpFlock->GetNeighborhood(this, BoidCohesionRadius);
	//if (neighborhood.Num() <= 1) return FVector::ZeroVector;
	if (neighborhood.Num() <= 0) return FVector::ZeroVector;

	for (AActor* pBoid : neighborhood)
	{
		//if (pBoid == this) continue;

		avgNeighborhoodLoc += pBoid->GetActorLocation();
	}
	avgNeighborhoodLoc /= neighborhood.Num();

	FVector cohesion = avgNeighborhoodLoc - GetActorLocation();
	cohesion.Normalize();

	return cohesion;
}
PRAGMA_ENABLE_OPTIMIZATION

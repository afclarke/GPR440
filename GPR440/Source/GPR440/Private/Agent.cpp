// Fill out your copyright notice in the Description page of Project Settings.


#include "Agent.h"
#include "DrawDebugHelpers.h"
#include "AIC_Agent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Flock.h"
#include "FlowField.h"

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

// Called when the game starts or when spawned
void AAgent::BeginPlay()
{
	Super::BeginPlay();

	mWanderTarget = GetActorLocation();

	mCollisionCount = 0;
	mGoalCount = -1;
	OnActorBeginOverlap.AddDynamic(this, &AAgent::OnCollision);
	mTarInput = FVector::ZeroVector;
	mCurInput = FMath::VRand();

	mpFlowField = Cast<AFlowField>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AFlowField::StaticClass()));
}

// Called every frame
void AAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	uint32 gridIndex = mpFlowField->GetGridIndex(GetActorLocation());
	UE_LOG(LogTemp, Warning, TEXT("INDEX: %u"), gridIndex);
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

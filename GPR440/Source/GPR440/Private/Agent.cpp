// Fill out your copyright notice in the Description page of Project Settings.


#include "Agent.h"
#include "DrawDebugHelpers.h"
#include "AIC_Agent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Flock.h"

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
}

// Called every frame
void AAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	mTarInput = FVector::ZeroVector;

	mTarInput = CalcFlockInput();

	//Wander();

	const UWorld* pWorld = GetWorld();

	// Obstacle Avoidance
	const FVector forwardVector = GetActorForwardVector();
	const FVector leftVector = forwardVector.RotateAngleAxis(-90, FVector::UpVector);
	const FVector rightVector = forwardVector.RotateAngleAxis(90, FVector::UpVector);
	const FVector lineTraceStart = GetActorLocation() + forwardVector * (GetCapsuleComponent()->GetScaledCapsuleRadius() + 0.1f);

	// Forward line trace
	const FVector forwardLineTraceEnd = lineTraceStart + forwardVector * ForwardLineTraceLength;
	FHitResult outForwardLineTraceHit;
	pWorld->LineTraceSingleByChannel(outForwardLineTraceHit, lineTraceStart, forwardLineTraceEnd,
		ECollisionChannel::ECC_Visibility);
	FColor forwardLineTraceDebugColor = outForwardLineTraceHit.bBlockingHit ? FColor::Red : FColor::Blue;
	DrawDebugLine(pWorld, lineTraceStart, forwardLineTraceEnd, forwardLineTraceDebugColor, false, -1, 0, DebugLineThickness);

	// left whisker line trace
	FVector leftWiskerDir = forwardVector.RotateAngleAxis(-WhiskerAngle, FVector::UpVector);
	const FVector leftLineTraceEnd = lineTraceStart + leftWiskerDir * WhiskerLineTraceLength;
	FHitResult outLeftLineTraceHit;
	pWorld->LineTraceSingleByChannel(outLeftLineTraceHit, lineTraceStart, leftLineTraceEnd,
		ECollisionChannel::ECC_Visibility);
	FColor leftLineTraceDebugColor = outLeftLineTraceHit.bBlockingHit ? FColor::Red : FColor::Blue;
	DrawDebugLine(pWorld, lineTraceStart, leftLineTraceEnd, leftLineTraceDebugColor, false, -1, 0, DebugLineThickness);

	// right whisker line trace
	FVector rightWiskerDir = forwardVector.RotateAngleAxis(WhiskerAngle, FVector::UpVector);
	const FVector rightLineTraceEnd = lineTraceStart + rightWiskerDir * WhiskerLineTraceLength;
	FHitResult outRightLineTraceHit;
	pWorld->LineTraceSingleByChannel(outRightLineTraceHit, lineTraceStart, rightLineTraceEnd,
		ECollisionChannel::ECC_Visibility);
	FColor rightLineTraceDebugColor = outRightLineTraceHit.bBlockingHit ? FColor::Red : FColor::Blue;
	DrawDebugLine(pWorld, lineTraceStart, rightLineTraceEnd, rightLineTraceDebugColor, false, -1, 0, DebugLineThickness);

	if (outForwardLineTraceHit.bBlockingHit)
	{
		float forwardAvoidScalar = 1.0f - outForwardLineTraceHit.Distance / ForwardLineTraceLength;
		forwardAvoidScalar *= forwardAvoidScalar;
		mTarInput += outForwardLineTraceHit.Normal * ForwardAvoidInputScalar * forwardAvoidScalar;
	}
	if (outLeftLineTraceHit.bBlockingHit)
	{
		float leftWhiskerAvoidScalar = 1.0f - outLeftLineTraceHit.Distance / WhiskerLineTraceLength;
		leftWhiskerAvoidScalar *= leftWhiskerAvoidScalar;
		mTarInput += rightVector * WhiskerAvoidInputScalar * leftWhiskerAvoidScalar;
	}
	else if (outRightLineTraceHit.bBlockingHit)
	{
		float rightWhiskerAvoidScalar = 1.0f - outRightLineTraceHit.Distance / WhiskerLineTraceLength;
		rightWhiskerAvoidScalar *= rightWhiskerAvoidScalar;
		mTarInput += leftVector * WhiskerAvoidInputScalar * rightWhiskerAvoidScalar;
	}

	mTarInput.Normalize();
	mCurInput = FMath::Lerp(mCurInput, mTarInput, DeltaTime * MoveInputLerpScalar);
	mpCharacterMovementComponent->AddInputVector(mCurInput);
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

	TArray<AAgent*> neighborhood = mpFlock->GetNeighborhood(loc, BoidSeparationRadius);
	for (AAgent* pBoid : neighborhood)
	{
		if (pBoid == this) continue;

		separation += loc - pBoid->GetActorLocation();
	}
	separation /= neighborhood.Num();
	separation.Normalize();

	return separation;
}

FVector AAgent::BoidAlignment()
{
	FVector alignment = FVector::ZeroVector;

	TArray<AAgent*> neighborhood = mpFlock->GetNeighborhood(GetActorLocation(), BoidAlignmentRadius);
	for (AAgent* pBoid : neighborhood)
	{
		if (pBoid == this) continue;

		alignment += pBoid->GetVelocity().GetSafeNormal();
	}
	alignment /= neighborhood.Num();
	alignment.Normalize();

	return alignment;
}

FVector AAgent::BoidCohesion()
{
	FVector avgNeighborhoodLoc = FVector::ZeroVector;
	FVector loc = GetActorLocation();

	TArray<AAgent*> neighborhood = mpFlock->GetNeighborhood(loc, BoidCohesionRadius);
	for (AAgent* pBoid : neighborhood)
	{
		if (pBoid == this) continue;

		avgNeighborhoodLoc += pBoid->GetActorLocation();
	}
	avgNeighborhoodLoc /= neighborhood.Num();

	FVector cohesion = avgNeighborhoodLoc - loc;
	cohesion.Normalize();

	return cohesion;
}
PRAGMA_ENABLE_OPTIMIZATION

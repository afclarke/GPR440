// Fill out your copyright notice in the Description page of Project Settings.


#include "Agent.h"
#include "DrawDebugHelpers.h"
#include "AIC_Agent.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}

PRAGMA_DISABLE_OPTIMIZATION
// Called every frame
void AAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Wander();
	
	const UWorld* pWorld = GetWorld();

	// Obstacle Avoidance
	const FVector lineTraceStart = GetActorLocation();
	const FVector forwardVector = GetActorForwardVector();
	
	// Forward line trace
	const FVector forwardLineTraceEnd = lineTraceStart + forwardVector * ForwardLineTraceLength;
	FHitResult outForwardLineTraceHit;
	pWorld->LineTraceSingleByChannel(outForwardLineTraceHit, lineTraceStart, forwardLineTraceEnd,
		ECollisionChannel::ECC_Visibility);
	FColor forwardLineTraceDebugColor = outForwardLineTraceHit.bBlockingHit ? FColor::Red : FColor::Blue;
	DrawDebugLine(pWorld, lineTraceStart, forwardLineTraceEnd, forwardLineTraceDebugColor, false, -1, 0, DebugLineThickness);

	// left whisker line trace
	FVector leftWiskerDir = forwardVector.RotateAngleAxis(-45.0f, FVector::UpVector);
	const FVector leftLineTraceEnd = lineTraceStart + leftWiskerDir * ForwardLineTraceLength;
	FHitResult outLeftLineTraceHit;
	pWorld->LineTraceSingleByChannel(outLeftLineTraceHit, lineTraceStart, leftLineTraceEnd,
		ECollisionChannel::ECC_Visibility);
	FColor leftLineTraceDebugColor = outLeftLineTraceHit.bBlockingHit ? FColor::Red : FColor::Blue;
	DrawDebugLine(pWorld, lineTraceStart, leftLineTraceEnd, leftLineTraceDebugColor, false, -1, 0, DebugLineThickness);

	// right whisker line trace
	FVector rightWiskerDir = forwardVector.RotateAngleAxis(45.0f, FVector::UpVector);
	const FVector rightLineTraceEnd = lineTraceStart + rightWiskerDir * ForwardLineTraceLength;
	FHitResult outRightLineTraceHit;
	pWorld->LineTraceSingleByChannel(outRightLineTraceHit, lineTraceStart, rightLineTraceEnd,
		ECollisionChannel::ECC_Visibility);
	FColor rightLineTraceDebugColor = outRightLineTraceHit.bBlockingHit ? FColor::Red : FColor::Blue;
	DrawDebugLine(pWorld, lineTraceStart, rightLineTraceEnd, rightLineTraceDebugColor, false, -1, 0, DebugLineThickness);

	// Left whisker line trace
	// Right whisker line trace
}
PRAGMA_ENABLE_OPTIMIZATION

void AAgent::Wander()
{
	FVector curLocation = GetActorLocation();
	FVector wanderDir = mWanderTarget - curLocation;
	float wanderDist = wanderDir.Size();

	// arrived at wander target, get new random target
	if(wanderDist <= WanderArriveRadius)
	{
		mWanderTarget = FMath::VRand() * WanderMovementRadius;
		mWanderTarget.Z = curLocation.Z;
		wanderDir = mWanderTarget - curLocation;
	}

	UE_LOG(LogTemp, Warning, TEXT("WANDER DIR (%f, %f, %f)"), wanderDir.X, wanderDir.Y, wanderDir.Z);
	// move to wander target
	mpCharacterMovementComponent->AddInputVector(wanderDir.GetSafeNormal());
}

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
	
	// Forward line trace
	FHitResult outForwardLineTraceHit;
	const FVector forwardLineTraceEnd = lineTraceStart + GetActorForwardVector() * ForwardLineTraceLength;
	pWorld->LineTraceSingleByChannel(outForwardLineTraceHit, lineTraceStart, forwardLineTraceEnd,
		ECollisionChannel::ECC_Visibility);
	FColor forwardLineTraceDebugColor;
	forwardLineTraceDebugColor = outForwardLineTraceHit.bBlockingHit ? FColor::Red : FColor::Blue;
	
	DrawDebugLine(pWorld, lineTraceStart, forwardLineTraceEnd, forwardLineTraceDebugColor, false, -1, 0, 10);

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

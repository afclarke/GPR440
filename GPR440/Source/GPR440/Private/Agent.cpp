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
	/*const FVector forwardLineTraceEnd = lineTraceStart + forwardVector * ForwardLineTraceLength;
	FHitResult outForwardLineTraceHit;
	pWorld->LineTraceSingleByChannel(outForwardLineTraceHit, lineTraceStart, forwardLineTraceEnd,
		ECollisionChannel::ECC_Visibility);
	FColor forwardLineTraceDebugColor = outForwardLineTraceHit.bBlockingHit ? FColor::Red : FColor::Blue;
	DrawDebugLine(pWorld, lineTraceStart, forwardLineTraceEnd, forwardLineTraceDebugColor, false, -1, 0, DebugLineThickness);*/

	// left whisker line trace
	FVector leftWiskerDir = forwardVector.RotateAngleAxis(-WhiskerAngle, FVector::UpVector);
	const FVector leftLineTraceEnd = lineTraceStart + leftWiskerDir * ForwardLineTraceLength;
	FHitResult outLeftLineTraceHit;
	pWorld->LineTraceSingleByChannel(outLeftLineTraceHit, lineTraceStart, leftLineTraceEnd,
		ECollisionChannel::ECC_Visibility);
	FColor leftLineTraceDebugColor = outLeftLineTraceHit.bBlockingHit ? FColor::Red : FColor::Blue;
	DrawDebugLine(pWorld, lineTraceStart, leftLineTraceEnd, leftLineTraceDebugColor, false, -1, 0, DebugLineThickness);
	
	// right whisker line trace
	FVector rightWiskerDir = forwardVector.RotateAngleAxis(WhiskerAngle, FVector::UpVector);
	const FVector rightLineTraceEnd = lineTraceStart + rightWiskerDir * ForwardLineTraceLength;
	FHitResult outRightLineTraceHit;
	pWorld->LineTraceSingleByChannel(outRightLineTraceHit, lineTraceStart, rightLineTraceEnd,
		ECollisionChannel::ECC_Visibility);
	FColor rightLineTraceDebugColor = outRightLineTraceHit.bBlockingHit ? FColor::Red : FColor::Blue;
	DrawDebugLine(pWorld, lineTraceStart, rightLineTraceEnd, rightLineTraceDebugColor, false, -1, 0, DebugLineThickness);

	if (outLeftLineTraceHit.bBlockingHit)
	{
		mpCharacterMovementComponent->AddInputVector(rightWiskerDir);
		UE_LOG(LogTemp, Warning, TEXT("LEFT BLOCK"));
	}
	else if (outRightLineTraceHit.bBlockingHit)
	{
		mpCharacterMovementComponent->AddInputVector(leftWiskerDir);
		UE_LOG(LogTemp, Warning, TEXT("RIGHT BLOCK"));
	}
	
	FVector mInputVector = mpCharacterMovementComponent->GetPendingInputVector();
	//UE_LOG(LogTemp, Warning, TEXT("INPUT (%f, %f, %f)"), mInputVector.X, mInputVector.Y, mInputVector.Z);
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

	// move to wander target
	mWanderInput = wanderDir.GetSafeNormal();
	mpCharacterMovementComponent->AddInputVector(mWanderInput);

	DrawDebugSphere(GetWorld(), mWanderTarget, WanderArriveRadius, 100, FColor::Magenta);
}

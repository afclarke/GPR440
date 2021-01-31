// Fill out your copyright notice in the Description page of Project Settings.


#include "Agent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AAgent::AAgent()
{
	PrimaryActorTick.bCanEverTick = true;

	// Component setup
	USceneComponent* pRootComponent = GetRootComponent();
	mpCapsuleComponent = CreateDefaultSubobject <UCapsuleComponent>("Capsule");
	mpCapsuleComponent->SetupAttachment(pRootComponent);
	mpSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("SkeletalMesh");
	mpSkeletalMeshComponent->SetupAttachment(mpCapsuleComponent);
}

// Called when the game starts or when spawned
void AAgent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const UWorld* pWorld = GetWorld();

	// Obstacle Avoidance
	const FVector lineTraceStart = GetActorLocation();
	
	// Forward line trace
	FHitResult outForwardLineTraceHit;
	const FVector forwardLineTraceEnd = lineTraceStart + GetActorForwardVector() * mForwardLineTraceLength;
	pWorld->LineTraceSingleByChannel(outForwardLineTraceHit, lineTraceStart, forwardLineTraceEnd,
		ECollisionChannel::ECC_Visibility);
	DrawDebugLine(pWorld, lineTraceStart, forwardLineTraceEnd, FColor::Green);

	// Left whisker line trace
	// Right whisker line trace
}

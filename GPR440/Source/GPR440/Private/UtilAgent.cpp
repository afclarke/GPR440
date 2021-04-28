// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilAgent.h"

PRAGMA_DISABLE_OPTIMIZATION
AUtilAgent::AUtilAgent()
{
	PrimaryActorTick.bCanEverTick = true;

	mpUtilDeciderComponent = CreateDefaultSubobject<UUtilDeciderComponent>("Decider");
}

void AUtilAgent::BeginPlay()
{
	Super::BeginPlay();

	mHealth = mMaxHealth;
}

void AUtilAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Act(mpUtilDeciderComponent->decide(this));
}

void AUtilAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
PRAGMA_ENABLE_OPTIMIZATION

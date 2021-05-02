// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilAgent.h"

PRAGMA_DISABLE_OPTIMIZATION
AUtilAgent::AUtilAgent()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AUtilAgent::BeginPlay()
{
	Super::BeginPlay();
}

void AUtilAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Act(mpUtilDeciderComponent->decide(this));
}
PRAGMA_ENABLE_OPTIMIZATION

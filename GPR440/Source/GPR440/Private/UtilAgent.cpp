// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilAgent.h"

AUtilAgent::AUtilAgent()
{
	PrimaryActorTick.bCanEverTick = true;

	mpUtilDeciderComponent = CreateDefaultSubobject<UUtilDeciderComponent>("Decider");
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UtilAgent.h"

AUtilGameMode::AUtilGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	// TODO: This is super ghetto and food should have a C++ class, but get it by name for now
	static ConstructorHelpers::FClassFinder<AActor> FoodPelletClass(TEXT("/Game/Blueprints/BP_FoodPellet"));
	mFoodPelletClass = FoodPelletClass.Class;
}

void AUtilGameMode::BeginPlay()
{
	mpQuadTree = NewObject<UQuadTree>(this, UQuadTree::StaticClass());
	mpQuadTree->Init(0, FBox2D());

	pTankFlowField = Cast<ATankFlowField>(UGameplayStatics::GetActorOfClass(
		GetWorld(), ATankFlowField::StaticClass()));
}

void AUtilGameMode::Tick(float DeltaSeconds)
{
	// rebuild QuadTree
	mpQuadTree->Clear();
	TArray<AActor*> agents;
	// TODO: Maintain a collection of organized actors
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), agents);
	// TODO: cont. of ghetto food pellet class
	TArray<AActor*> foodPellets;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), mFoodPelletClass, foodPellets);
	agents.Append(foodPellets);
	for (AActor* agent : agents)
	{
		mpQuadTree->Insert(agent);
	}

	// draw debug
	if(mDrawDebug)
	{
		mpQuadTree->Draw(GetWorld());
		pTankFlowField->DebugDraw();
	}
}

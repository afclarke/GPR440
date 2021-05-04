// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilGameMode.h"
#include "Kismet/GameplayStatics.h"

AUtilGameMode::AUtilGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
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
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUtilAgent::StaticClass(), agents);
	for (AActor* agent : agents)
	{
		mpQuadTree->Insert(agent);
	}
	mpQuadTree->Draw(GetWorld());
}

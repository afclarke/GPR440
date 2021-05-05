// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UtilAgent.h"

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
	// TODO: this should only insert specific classes like agents,
	// but for demo convenience, all actors (food) are inserted
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), agents);
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

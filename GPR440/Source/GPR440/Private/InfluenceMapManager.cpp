// Fill out your copyright notice in the Description page of Project Settings.


#include "InfluenceMapManager.h"

AInfluenceMapManager::AInfluenceMapManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AInfluenceMapManager::RegisterMap(AInfluenceMap* map)
{
	mInfluenceMaps.Add(map);
}

void AInfluenceMapManager::RegisterAgent(AAgent* agent)
{
	mAgents.Add(agent);
}

void AInfluenceMapManager::BeginPlay()
{
	Super::BeginPlay();
}

void AInfluenceMapManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (AInfluenceMap* map : mInfluenceMaps)
	{
		map->ClearMap();
	}
	for (AAgent* agent : mAgents)
	{
		agent->WriteToInfluenceMaps();
	}
	for (AAgent* agent : mAgents)
	{
		agent->ReadInfluenceMaps();
	}
}


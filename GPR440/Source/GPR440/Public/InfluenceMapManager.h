// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InfluenceMap.h"
#include "Agent.h"
#include "InfluenceMapManager.generated.h"

UCLASS()
class GPR440_API AInfluenceMapManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AInfluenceMapManager();

	void RegisterMap(AInfluenceMap* map);
	void RegisterAgent(AAgent* agent);
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	TArray<AInfluenceMap*> mInfluenceMaps;
	TArray<AAgent*> mAgents;

};

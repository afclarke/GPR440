// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "QuadTree.h"
#include "UtilAgent.h"
#include "UtilGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GPR440_API AUtilGameMode : public AGameModeBase
{
	GENERATED_BODY()

	AUtilGameMode();
	
protected:
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

public:
	//UPROPERTY(BlueprintReadOnly)
	//TArray<AUtilAgent*> mpUtilAgents;
	UPROPERTY(BlueprintReadOnly)
	UQuadTree* mpQuadTree;
};

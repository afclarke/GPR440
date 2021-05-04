// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UtilDeciderComponent.h"
#include "UtilAgent.generated.h"

UCLASS(Abstract)
class GPR440_API AUtilAgent : public APawn
{
	GENERATED_BODY()

public:
	AUtilAgent();

	UFUNCTION(BlueprintCallable)
	FVector2D Flock(float separationRadiusSqr, float alignmentRadiusSqr, float cohesionRadiusSqr,
		float separationWeight, float alignmentWeight, float cohesionWeight, FName withTag);
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* mpRootMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UUtilDeciderComponent* mpUtilDeciderComponent;

	UFUNCTION(BlueprintImplementableEvent)
	bool Act(EUtilActionType actionType);
	//PURE_VIRTUAL(AUtilAgent::Act)

};

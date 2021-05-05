// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UtilAction.h"
#include "UtilAgent.generated.h"

UENUM(BlueprintType)
enum class EUtilDecisionMethod : uint8
{
	GREATEST,
	WEIGHTED_RANDOM
};

UCLASS(Abstract)
class GPR440_API AUtilAgent : public APawn
{
	GENERATED_BODY()

public:
	AUtilAgent();

	UFUNCTION(BlueprintCallable)
	FVector2D Flock(float separationRadiusSqr, float alignmentRadiusSqr, float cohesionRadiusSqr,
		float separationWeight, float alignmentWeight, float cohesionWeight, FName withTag);

	UFUNCTION(BlueprintImplementableEvent)
	void BindActs(const TArray<UUtilAction*>& pActions);
	UFUNCTION(BlueprintImplementableEvent)
	void InitUtilWidget(const TArray<UUtilAction*>& pActions);
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

private:
	void Init();
	void Cleanup();
	EUtilActionType Decide();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EUtilDecisionMethod mDecisionMethod;
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UUtilAction>> mActions;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* mpRootMesh;

private:
	UPROPERTY()
	TArray<UUtilAction*> mActionObjs;

};

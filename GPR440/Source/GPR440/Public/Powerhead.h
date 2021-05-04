// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Powerhead.generated.h"

class ATankFlowField;

UCLASS()
class GPR440_API APowerhead : public AActor
{
	GENERATED_BODY()
	
public:	
	APowerhead();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(BlueprintReadOnly)
	FVector2D mFlow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D mFlowA;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D mFlowB;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float mCyclePeriod;
	
private:
	ATankFlowField* pFlowField;

	float mCurCycleTime;
	bool mCycleUp;
};

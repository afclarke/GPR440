// Fill out your copyright notice in the Description page of Project Settings.


#include "Powerhead.h"
#include "TankFlowField.h"
#include "Kismet/GameplayStatics.h"

PRAGMA_DISABLE_OPTIMIZATION
APowerhead::APowerhead()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APowerhead::BeginPlay()
{
	Super::BeginPlay();

	pFlowField = Cast<ATankFlowField>(UGameplayStatics::GetActorOfClass(
		GetWorld(), ATankFlowField::StaticClass()));
	pFlowField->AddPowerhead(this);

	mCurCycleTime = FMath::RandRange(0.f, mCyclePeriod);
	mCycleUp = FMath::RandBool();
}

void APowerhead::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(mCycleUp)
	{
		mCurCycleTime += DeltaTime;
		if(mCurCycleTime >= mCyclePeriod)
		{
			mCycleUp = false;
		}
	}
	else
	{
		mCurCycleTime -= DeltaTime;
		if(mCurCycleTime <= 0)
		{
			mCycleUp = true;
		}
	}
	mCurCycleTime = FMath::Clamp(mCurCycleTime, 0.f, mCyclePeriod);
	mFlow = FMath::Lerp(mFlowA, mFlowB, mCurCycleTime / mCyclePeriod);
}
PRAGMA_ENABLE_OPTIMIZATION

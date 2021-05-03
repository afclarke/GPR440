// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilConsideration.h"

PRAGMA_DISABLE_OPTIMIZATION
float UUtilConsideration::consider(AUtilAgent* pAgent, AUtilGameMode* pGameMode)
{
	float normalizedPerception = perceive(pAgent, pGameMode);
	mUtilityCache = mCurve->GetFloatValue(normalizedPerception);
	return mUtilityCache;
}
PRAGMA_ENABLE_OPTIMIZATION

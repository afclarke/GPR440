// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilPerception.h"

PRAGMA_DISABLE_OPTIMIZATION
float UUtilPerception::consider(AUtilAgent* pAgent, AUtilGameMode* pGameMode)
{
	mNormalizedInputCache = perceive(pAgent, pGameMode);
	mUtilityCache = mCurve->GetFloatValue(mNormalizedInputCache);
	return mUtilityCache;
}
PRAGMA_ENABLE_OPTIMIZATION

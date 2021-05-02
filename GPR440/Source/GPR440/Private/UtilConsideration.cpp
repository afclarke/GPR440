// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilConsideration.h"

PRAGMA_DISABLE_OPTIMIZATION
float UUtilConsideration::consider(AUtilAgent* pAgent) const
{
	return mCurve->GetFloatValue(perceive(pAgent));
}
PRAGMA_ENABLE_OPTIMIZATION

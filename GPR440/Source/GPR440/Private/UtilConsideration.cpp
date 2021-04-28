// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilConsideration.h"


float UUtilConsideration::consider(AUtilAgent* pAgent) const
{
	return mCurve->GetFloatValue(perceive(pAgent));
}

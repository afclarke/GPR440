// Fill out your copyright notice in the Description page of Project Settings.

#include "QuadTree.h"
#include "DrawDebugHelpers.h"

PRAGMA_DISABLE_OPTIMIZATION
bool UQuadTree::Insert(AActor* pActor)
{
	const FVector2D actorLoc = FVector2D(pActor->GetActorLocation());

	// actor not inside this quad
	if (!PointIsInsideOrOn(actorLoc))
	{
		return false;
	}

	// has capacity and not yet subdivided
	if (mIsLeaf || mpActors.Num() < mCapacity && !mSubdivided)
	{
		mpActors.Add(pActor);
		return true;
	}

	// at capacity and not subdivided
	if (!mSubdivided)
	{
		Subdivide();
	}

	// try insertion on each sub quad
	for (int32 i = 0; i < 4; i++)
	{
		if (mpNodes[i]->Insert(pActor))
		{
			return true;
		}
	}

	return false;
}

TArray<AActor*> UQuadTree::QuerySqrRadius(AActor* pActor, float sqrRadius)
{
	TArray<AActor*> actorsInRadius;

	const FVector2D actorLoc = FVector2D(pActor->GetActorLocation());

	// not in range, return empty
	if (mRect.ComputeSquaredDistanceToPoint(actorLoc) > sqrRadius)
	{
		return actorsInRadius;
	}

	// query actors
	if (!mSubdivided)
	{
		for (AActor* actor : mpActors)
		{
			// do not including self
			if (actor == pActor)
			{
				continue;
			}
			// add in radius
			if ((FVector2D(actor->GetActorLocation()) - actorLoc).SizeSquared() <= sqrRadius)
			{
				actorsInRadius.Add(actor);
			}
		}
		return actorsInRadius;
	}

	// query sub quadrants
	for (int32 i = 0; i < 4; i++)
	{
		actorsInRadius.Insert(mpNodes[i]->QuerySqrRadius(pActor, sqrRadius), actorsInRadius.Num());
	}
	return actorsInRadius;
}

void UQuadTree::Draw(UWorld* pWorld) const
{
	DrawDebugBox(pWorld, FVector(mRect.GetCenter(), 200), FVector(mRect.GetExtent(), 1), FColor::Magenta, false, -1, 0, 15);
	if (mSubdivided)
	{
		for (int32 i = 0; i < 4; i++)
		{
			mpNodes[i]->Draw(pWorld);
		}
	}
}

void UQuadTree::Init(int32 depth, FBox2D rect)
{
	mDepth = depth;
	mRect = rect;
	mpActors.Reserve(mCapacity);
	mIsLeaf = depth == mMaxDepth;
}

void UQuadTree::Subdivide()
{
	FVector2D halfDims = mRect.GetSize() / 2.0f;
	FVector2D center = mRect.GetCenter();

	// create sub quads
	const int32 nextLevel = mDepth + 1;
	mpNodes[0] = NewObject<UQuadTree>(this, UQuadTree::StaticClass());
	mpNodes[0]->Init(nextLevel, FBox2D(
		FVector2D(center.X - halfDims.X, center.Y), FVector2D(center.X, center.Y + halfDims.Y)));
	mpNodes[1] = NewObject<UQuadTree>(this, UQuadTree::StaticClass());
	mpNodes[1]->Init(nextLevel, FBox2D(
		FVector2D(center.X, center.Y), FVector2D(center.X + halfDims.X, center.Y + halfDims.Y)));
	mpNodes[2] = NewObject<UQuadTree>(this, UQuadTree::StaticClass());
	mpNodes[2]->Init(nextLevel, FBox2D(
		FVector2D(center.X - halfDims.X, center.Y - halfDims.Y), FVector2D(center.X, center.Y)));
	mpNodes[3] = NewObject<UQuadTree>(this, UQuadTree::StaticClass());
	mpNodes[3]->Init(nextLevel, FBox2D(
		FVector2D(center.X, center.Y - halfDims.Y), FVector2D(center.X + halfDims.X, center.Y)));

	// distribute actors to sub quads
	for (int32 i = 0; i < mCapacity; i++)
	{
		int32 j = 0;
		while (!mpNodes[j]->Insert(mpActors[i]))
		{
			j++;
		}
	}
	mpActors.Empty();

	mSubdivided = true;
}

bool UQuadTree::PointIsInsideOrOn(const FVector2D& point) const
{
	// FBox2D does not have "IsInside*OrOn*" function
	return point.X >= mRect.Min.X && point.X <= mRect.Max.X && point.Y >= mRect.Min.Y && point.Y <= mRect.Max.Y;
}

void UQuadTree::Clear()
{
	mpActors.Empty();

	if (mSubdivided)
	{
		for (int32 i = 0; i < 4; i++)
		{
			mpNodes[i]->Clear();
			//delete mpNodes[i];
			// garbage collected
			mpNodes[i] = nullptr;
		}
		mSubdivided = false;
	}

}
PRAGMA_ENABLE_OPTIMIZATION

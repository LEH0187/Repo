// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class TEST2_API FFrustumCulling
{
public:
	FFrustumCulling();
    void UpdateFrustum(const FMatrix& ProjectionMatrix, const FMatrix& ViewMatrix);

    bool IsPointInsideFrustum(const FVector& Point) const;
    bool IsSphereInsideFrustum(const FVector& Center, float Radius) const;

private:
    TArray<FPlane> FrustumPlanes;  // 6개의 평면을 저장할 배열
};

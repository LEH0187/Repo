// Fill out your copyright notice in the Description page of Project Settings.


#include "FrustumCulling.h"

// 생성자에서 프러스텀 평면을 초기화
FFrustumCulling::FFrustumCulling()
{
    FrustumPlanes.SetNum(6);  // 프러스텀의 6개의 평면
}

// 프러스텀 평면 업데이트 함수
void FFrustumCulling::UpdateFrustum(const FMatrix& ProjectionMatrix, const FMatrix& ViewMatrix)
{
    FMatrix FrustumMatrix = ViewMatrix * ProjectionMatrix;

    // Left Plane
    FrustumPlanes[0] = FPlane(
        FrustumMatrix.M[3][0] + FrustumMatrix.M[0][0],
        FrustumMatrix.M[3][1] + FrustumMatrix.M[0][1],
        FrustumMatrix.M[3][2] + FrustumMatrix.M[0][2],
        FrustumMatrix.M[3][3] + FrustumMatrix.M[0][3]
    );

    // Right Plane
    FrustumPlanes[1] = FPlane(
        FrustumMatrix.M[3][0] - FrustumMatrix.M[0][0],
        FrustumMatrix.M[3][1] - FrustumMatrix.M[0][1],
        FrustumMatrix.M[3][2] - FrustumMatrix.M[0][2],
        FrustumMatrix.M[3][3] - FrustumMatrix.M[0][3]
    );

    // Bottom Plane
    FrustumPlanes[2] = FPlane(
        FrustumMatrix.M[3][0] + FrustumMatrix.M[1][0],
        FrustumMatrix.M[3][1] + FrustumMatrix.M[1][1],
        FrustumMatrix.M[3][2] + FrustumMatrix.M[1][2],
        FrustumMatrix.M[3][3] + FrustumMatrix.M[1][3]
    );

    // Top Plane
    FrustumPlanes[3] = FPlane(
        FrustumMatrix.M[3][0] - FrustumMatrix.M[1][0],
        FrustumMatrix.M[3][1] - FrustumMatrix.M[1][1],
        FrustumMatrix.M[3][2] - FrustumMatrix.M[1][2],
        FrustumMatrix.M[3][3] - FrustumMatrix.M[1][3]
    );

    // Near Plane
    FrustumPlanes[4] = FPlane(
        FrustumMatrix.M[3][0] + FrustumMatrix.M[2][0],
        FrustumMatrix.M[3][1] + FrustumMatrix.M[2][1],
        FrustumMatrix.M[3][2] + FrustumMatrix.M[2][2],
        FrustumMatrix.M[3][3] + FrustumMatrix.M[2][3]
    );

    // Far Plane
    FrustumPlanes[5] = FPlane(
        FrustumMatrix.M[3][0] - FrustumMatrix.M[2][0],
        FrustumMatrix.M[3][1] - FrustumMatrix.M[2][1],
        FrustumMatrix.M[3][2] - FrustumMatrix.M[2][2],
        FrustumMatrix.M[3][3] - FrustumMatrix.M[2][3]
    );

    // 평면의 법선을 정규화하여 정확한 프러스텀을 얻음
    for (FPlane& Plane : FrustumPlanes)
    {
        FVector Normal = FVector(Plane.X, Plane.Y, Plane.Z).GetSafeNormal();
        Plane = FPlane(Normal, Plane.W);  // W 값을 유지하면서 새 평면을 생성하여 할당
    }
}

// 프러스텀 내에 점이 있는지 확인
bool FFrustumCulling::IsPointInsideFrustum(const FVector& Point) const
{
    for (const FPlane& Plane : FrustumPlanes)
    {
        if (Plane.PlaneDot(Point) < 0)
        {
            return false; // 점이 평면의 뒤에 있으므로 프러스텀 외부에 있음
        }
    }
    return true; // 모든 평면의 내부에 점이 있음
}

// 프러스텀 내에 구가 있는지 확인
bool FFrustumCulling::IsSphereInsideFrustum(const FVector& Center, float Radius) const
{
    for (const FPlane& Plane : FrustumPlanes)
    {
        if (Plane.PlaneDot(Center) < -Radius)
        {
            return false; // 구가 평면의 뒤에 있으므로 프러스텀 외부에 있음
        }
    }
    return true; // 모든 평면의 내부에 구가 있음
}

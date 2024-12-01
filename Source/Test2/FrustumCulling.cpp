// Fill out your copyright notice in the Description page of Project Settings.


#include "FrustumCulling.h"


FFrustumCulling::FFrustumCulling()
{
    
}

FFrustumCulling::~FFrustumCulling()
{
    delete ViewFamily;
}

void FFrustumCulling::InitializeInfo(UWorld* _World)
{
    PlayerController = _World->GetFirstPlayerController();
    LocalPlayer = PlayerController->GetLocalPlayer();
    
    ViewFamily = new FSceneViewFamilyContext(
        FSceneViewFamily::ConstructionValues(
            LocalPlayer->ViewportClient->Viewport,
            _World->Scene,
            LocalPlayer->ViewportClient->EngineShowFlags
        ).SetRealtimeUpdate(true)
    );

    bCompleteInitializeInfo = true;
}

FConvexVolume FFrustumCulling::GetCameraFrustum()
{
    
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("not exsite PC"));
        return FConvexVolume();
    }

    if (LocalPlayer && LocalPlayer->ViewportClient)
    {
        FVector ViewLocation;
        FRotator ViewRotation;
        PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

        FSceneView* SceneView;
        FGraphEventRef GetViewEvent = FFunctionGraphTask::CreateAndDispatchWhenReady([this, &SceneView, &ViewLocation, &ViewRotation]{
           SceneView = LocalPlayer->CalcSceneView(ViewFamily, ViewLocation, ViewRotation, LocalPlayer->ViewportClient->Viewport);
        }, TStatId(), nullptr, ENamedThreads::GameThread);
        GetViewEvent->Wait();
        if (SceneView)
        {
            return SceneView->CullingFrustum;
        }
        UE_LOG(LogTemp, Warning, TEXT("Empty FConvexVolume"));
    }
    return FConvexVolume(); // 기본 빈 프러스텀 반환
}

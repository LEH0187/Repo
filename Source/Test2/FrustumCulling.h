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
	~FFrustumCulling();

public:
    void InitializeInfo(UWorld* _World);
    FConvexVolume GetCameraFrustum();   

public:
    bool bCompleteInitializeInfo = false;

private:
    FSceneViewFamilyContext* ViewFamily;
    APlayerController* PlayerController;
    ULocalPlayer* LocalPlayer;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseGenerator.h"


UNoiseGenerator::UNoiseGenerator()
{
    GroundBaseWrapper = CreateDefaultSubobject<UFastNoiseWrapper>(TEXT("GroundBase"));
    GroundBaseWrapper->SetupFastNoise(EFastNoise_NoiseType::PerlinFractal, 1337, 0.00001f); //...

    
    GroundBaseCurve = ConstructorHelpers::FObjectFinder<UCurveFloat>(TEXT("/Game/World/TerrainCurves/GroundBase")).Object;
}

float UNoiseGenerator::GetGroundBaseNoise3D(float X, float Y, float Z, float Amplitude)
{
    if(GroundBaseCurve)
    {
        return GroundBaseCurve->GetFloatValue(GroundBaseWrapper->GetNoise3D(X, Y, Z)) * Amplitude;
    }
    return 0.f;
}

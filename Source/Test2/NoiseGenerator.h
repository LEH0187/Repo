// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FastNoise.h"
#include "FastNoiseGenerator/Public/FastNoiseWrapper.h"
#include "Curve/CurveUtil.h"
#include "NoiseGenerator.generated.h"

/**
 * 
 */
UCLASS()
class TEST2_API UNoiseGenerator : public UObject
{
	GENERATED_BODY()
	
public:
	UNoiseGenerator();
	float GetGroundBaseNoise3D(float X, float Y, float Z, float Amplitude = 1);

public:
	TObjectPtr<UCurveFloat> GroundBaseCurve;
private:
	UFastNoiseWrapper* GroundBaseWrapper;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Test2/Procedural/ProceduralPlanet.h"
#include "WorldGameState.generated.h"

/**
 * 
 */
UCLASS()
class TEST2_API AWorldGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AWorldGameState();

	UFUNCTION(BlueprintCallable, category="GameState")
	AProceduralPlanet* GetBasicWorld() const { return this->BasicWorld; }
	UFUNCTION(BlueprintCallable, category="GameState")
	void SetBasicWorld(AProceduralPlanet* P) { this->BasicWorld = P; }
public:
	AProceduralPlanet* BasicWorld;
};

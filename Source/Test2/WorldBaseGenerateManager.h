// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldGameState.h"
#include "WorldBaseGenerateManager.generated.h" 

UCLASS()
class TEST2_API AWorldBaseGenerateManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldBaseGenerateManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	AProceduralPlanet* GeneratePlanet_Implementation();
};

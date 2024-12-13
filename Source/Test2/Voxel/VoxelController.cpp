// Fill out your copyright notice in the Description page of Project Settings.


#include "Voxel/VoxelController.h"

// Sets default values
AVoxelController::AVoxelController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVoxelController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVoxelController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


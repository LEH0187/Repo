// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Voxel.generated.h"

// Cube Vertcies Index
//						 Pos_Z  Neg_X	
//    4------7				| /
//   /|     /|				|/
//  5------6 | 	  Neg_Y ㅡㅡㅡㅡㅡ Pos_Y
//  | 0----| 3			   /|
//  |/     |/			  / |
//  1------2		 Pos_X  Neg_Z
//  CW Draw


enum class EVoxelDirection { Forward, Right, Back, Left, Up, Down };

enum class EVoxelBlockType { Null, Air, Stone, Dirt, Grass };

struct FCubeFaces
{
	TArray<FVector> CubeFace_PosX;
	TArray<FVector> CubeFace_NegX;

	TArray<FVector> CubeFace_PosY;
	TArray<FVector> CubeFace_NegY;

	TArray<FVector> CubeFace_PosZ;
	TArray<FVector> CubeFace_NegZ;
};

UCLASS()
class TEST2_API AVoxel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoxel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

private:
	void InitializeVoxel1X1();

public:
	EVoxelBlockType BlockType;
	bool bSolid;
	FVector Position;

	FCubeFaces Faces;
	TArray<FVector> Cube1X1;
	TArray<int32> Voxel1X1Triangles;
};

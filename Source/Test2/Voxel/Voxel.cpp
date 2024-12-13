// Fill out your copyright notice in the Description page of Project Settings.


#include "Voxel/Voxel.h"

// Sets default values
AVoxel::AVoxel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitializeVoxel1X1();
	BlockType = EVoxelBlockType::Null;
}

// Called when the game starts or when spawnedSource/Test2/Voxel/Voxel.cpp
void AVoxel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVoxel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVoxel::InitializeVoxel1X1()
{
	
	Cube1X1.Add(FVector( -1.f,  -1.f,  -1.f));
	Cube1X1.Add(FVector(  1.f,  -1.f,  -1.f));
	Cube1X1.Add(FVector(  1.f,   1.f,  -1.f));
	Cube1X1.Add(FVector( -1.f,   1.f,  -1.f));
	Cube1X1.Add(FVector( -1.f,  -1.f,   1.f));
	Cube1X1.Add(FVector(  1.f,  -1.f,   1.f));
	Cube1X1.Add(FVector(  1.f,   1.f,   1.f));
	Cube1X1.Add(FVector( -1.f,   1.f,   1.f));

	Faces.CubeFace_PosX = { Cube1X1[5], Cube1X1[1], Cube1X1[2], Cube1X1[6]};
	Faces.CubeFace_NegX = { Cube1X1[7], Cube1X1[3], Cube1X1[0], Cube1X1[4]};
	Faces.CubeFace_PosY = { Cube1X1[6], Cube1X1[2], Cube1X1[3], Cube1X1[7]};
	Faces.CubeFace_NegY = { Cube1X1[4], Cube1X1[0], Cube1X1[1], Cube1X1[5]};
	Faces.CubeFace_PosZ = { Cube1X1[1], Cube1X1[0], Cube1X1[3], Cube1X1[2]};
	Faces.CubeFace_NegZ = { Cube1X1[5], Cube1X1[4], Cube1X1[7], Cube1X1[6]};
	
	Voxel1X1Triangles.Add(5);Voxel1X1Triangles.Add(6);Voxel1X1Triangles.Add(1);
	Voxel1X1Triangles.Add(1);Voxel1X1Triangles.Add(6);Voxel1X1Triangles.Add(2);
	
	Voxel1X1Triangles.Add(7);Voxel1X1Triangles.Add(4);Voxel1X1Triangles.Add(3);
	Voxel1X1Triangles.Add(3);Voxel1X1Triangles.Add(4);Voxel1X1Triangles.Add(0);
	
	Voxel1X1Triangles.Add(6);Voxel1X1Triangles.Add(7);Voxel1X1Triangles.Add(2);
	Voxel1X1Triangles.Add(2);Voxel1X1Triangles.Add(7);Voxel1X1Triangles.Add(3);
	
	Voxel1X1Triangles.Add(4);Voxel1X1Triangles.Add(5);Voxel1X1Triangles.Add(0);
	Voxel1X1Triangles.Add(0);Voxel1X1Triangles.Add(5);Voxel1X1Triangles.Add(1);

	Voxel1X1Triangles.Add(4);Voxel1X1Triangles.Add(7);Voxel1X1Triangles.Add(5);
	Voxel1X1Triangles.Add(5);Voxel1X1Triangles.Add(7);Voxel1X1Triangles.Add(6);
	
	Voxel1X1Triangles.Add(1);Voxel1X1Triangles.Add(2);Voxel1X1Triangles.Add(0);
	Voxel1X1Triangles.Add(0);Voxel1X1Triangles.Add(2);Voxel1X1Triangles.Add(3);
}


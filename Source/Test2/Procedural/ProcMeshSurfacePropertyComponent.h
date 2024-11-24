// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProceduralMeshComponent.h"
#include "ProcMeshSurfacePropertyComponent.generated.h"


class UProceduralMeshComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST2_API UProcMeshSurfacePropertyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProcMeshSurfacePropertyComponent();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	bool InitializeSurfacePropertis(TArray<FVector> _Vertices, FJsonSerializableArrayInt);

	TArray<FVector> GetNormals() 			{ return Normals; }
	TArray<FVector2D> GetUVs() 				{ return UVs; }
	TArray<FProcMeshTangent> GetTangents() 	{ return Tangents; }


private:
    bool CalculateNormals(TArray<FVector> _Vertices, FJsonSerializableArrayInt _Triangles);
    bool CalculateUVs(TArray<FVector> _Vertices);
    bool CalculateTangents(TArray<FVector> _Vertices);

private:
    TArray<FVector>                     Normals;
    TArray<FVector2D>                   UVs;
    TArray<FProcMeshTangent>            Tangents;
};

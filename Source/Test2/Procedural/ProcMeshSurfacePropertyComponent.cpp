// Fill out your copyright notice in the Description page of Project Settings.


#include "Procedural/ProcMeshSurfacePropertyComponent.h"

// Sets default values for this component's properties
UProcMeshSurfacePropertyComponent::UProcMeshSurfacePropertyComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UProcMeshSurfacePropertyComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	Normals.Empty();
    Tangents.Empty();
}

bool UProcMeshSurfacePropertyComponent::InitializeSurfacePropertis(TArray<FVector> _Vertices, FJsonSerializableArrayInt _Triangles)
{
	if(_Vertices.Num() == 0 || _Triangles.Num() == 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Vertices or Triangles is Empty"));
		return false;
	}

	bool bSuccessUVs = CalculateUVs(_Vertices);
	bool bSuccessNormals = CalculateNormals(_Vertices, _Triangles);
	bool bSuccessTangents = CalculateTangents(_Vertices);

	if(bSuccessUVs && bSuccessNormals && bSuccessTangents)
	{
		return true;
	}
	return false;
}

bool UProcMeshSurfacePropertyComponent::CalculateNormals(TArray<FVector> _Vertices, TArray<int32> _Triangles)
{
	return true;
}

bool UProcMeshSurfacePropertyComponent::CalculateUVs(TArray<FVector> _Vertices)
{
	return true;
}

bool UProcMeshSurfacePropertyComponent::CalculateTangents(TArray<FVector> _Vertices)
{
	return true;
}

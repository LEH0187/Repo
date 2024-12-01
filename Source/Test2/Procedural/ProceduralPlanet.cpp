#include "ProceduralPlanet.h"
#include "DrawDebugHelpers.h"

// Sets default values
AProceduralPlanet::AProceduralPlanet()
{
    PrimaryActorTick.bCanEverTick = true;

    ProcComp = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMeshComp"));
    GeometryControl = CreateDefaultSubobject<UGeometryControlComponent>(TEXT("GeometryControl"));
}

// Called when the game starts or when spawned
void AProceduralPlanet::BeginPlay()
{
    Super::BeginPlay(); 
    GeometryControl->Initialize(6371000.f, RunTimeMaxSubdivsionLevel, PreComputedSubdivisionLevel);
}   

void AProceduralPlanet::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void AProceduralPlanet::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    if(GeometryControl->IsReadyInitialMesh())
    {        
        FGraphEventRef LODEvent = FFunctionGraphTask::CreateAndDispatchWhenReady([this](){
            bool& ref = GeometryControl->UpdateLOD();
            if(ref)
            {
                AsyncTask(ENamedThreads::GameThread, [this]{
                    DrawMesh();
                });
                ref = false;
            }
        },TStatId(), nullptr, ENamedThreads::AnyNormalThreadNormalTask);       
    }
}



void AProceduralPlanet::DrawMesh()
{
    FScopeLock Lock(&Mutex);
    ProcComp->ClearAllMeshSections();
    ProcComp->CreateMeshSection_LinearColor(
            0,
            GeometryControl->GetVertices(),
            GeometryControl->GetTriangles(),
            /*SurfacePropertyComp->GetNormals()*/TArray<FVector>(),
            /*SurfacePropertyComp->GetUVs()*/TArray<FVector2D>(),
            TArray<FLinearColor>(),
            /*SurfacePropertyComp->GetTangents()*/TArray<FProcMeshTangent>(),
            true
        );
    GeometryControl->InitializeGeometryData();
}

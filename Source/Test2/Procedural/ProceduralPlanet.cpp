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
    GeometryControl->Initialize(6371000.f, 14, 10);
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
            FGraphEventRef& ref = GeometryControl->UpdateLOD();
            if(ref.IsValid() && ref->IsComplete())
            {
                AsyncTask(ENamedThreads::GameThread, [this]{
                    FMeshDrawProperties MDP;
                    MDP.Verties     = GeometryControl->GetVertices();
                    MDP.Triangles   = GeometryControl->GetTriangles();
                    DrawMesh(MDP);
                });
                ref = nullptr;
            }
        },TStatId(), nullptr, ENamedThreads::AnyNormalThreadNormalTask);       
    }
}



void AProceduralPlanet::DrawMesh(FMeshDrawProperties MeshProperties)
{
    FScopeLock Lock(&Mutex);
    ProcComp->ClearMeshSection(0);
    ProcComp->CreateMeshSection_LinearColor(0, MeshProperties.Verties, MeshProperties.Triangles, MeshProperties.Normals, MeshProperties.UVs, TArray<FLinearColor>(),
                                            MeshProperties.Tangents, true);
    GeometryControl->InitializeGeometryData();
}

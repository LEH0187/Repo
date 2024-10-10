#pragma once

#include "CoreMinimal.h"

struct FOctreeNode
{
    FVector Center;                // 노드의 중심
    FVector Extent;                // 노드의 범위
    TArray<FVector> Vertices;      // 이 노드에 속한 정점들

    FOctreeNode* Children[8];      // 8개의 자식 노드

    FOctreeNode(const FVector& InCenter, const FVector& InExtent)
        : Center(InCenter), Extent(InExtent)
    {
        for (int32 i = 0; i < 8; ++i)
        {
            Children[i] = nullptr;
        }
    }

    FVector GetCenter() { return Center; }
};

class FOctree
{
private:
    FOctreeNode* RootNode;
    int32 MaxDepth;
    int32 MaxVertices = 200;
public:
    // Octree 생성자
    FOctree(const FVector& Origin, const FVector& WorldExtent, int32 InMaxDepth)
    {
        RootNode = new FOctreeNode(Origin, WorldExtent);
        MaxDepth = InMaxDepth;
    }

    // 소멸자 - 메모리 관리
    ~FOctree()
    {
        DeleteNode(RootNode);
    }

    FOctreeNode* GetRootNode() { return RootNode; }

    // Octree에 정점을 삽입하는 함수
    bool InsertVertex(FOctreeNode* Node, const FVector& Vertex, int32 CurrentDepth = 0)
    {
        // 현재 노드의 경계 내에 있는지 확인
        if (!IsInsideNode(Node, Vertex))
        {
            return false;
        }

        // 노드가 분할되지 않았고 최대 정점 수를 초과하지 않으면 정점 추가
        if (CurrentDepth == MaxDepth || Node->Vertices.Num() < MaxVertices)
        {
            AddUniqueVertex(Node, Vertex); // 중복되지 않으면 정점 추가
            return true;
        }

        // 분할되지 않았다면 자식 노드를 생성
        if (!Node->Children[0])
        {
            CreateChildren(Node);
        }

        // 적절한 자식 노드로 재귀적으로 삽입
        int32 ChildIndex = GetChildIndex(Node, Vertex);
        return InsertVertex(Node->Children[ChildIndex], Vertex, CurrentDepth + 1);
    }

    TArray<FVector> GetAllVerticesInNode(FOctreeNode* Node)
    {
        TArray<FVector> CollectedVertices;

        if (!Node) return CollectedVertices; // 노드가 유효한지 확인

        // 현재 노드의 모든 정점을 추가
        CollectedVertices.Append(Node->Vertices);

        // 자식 노드가 있다면 재귀적으로 정점들을 수집
        for (int32 i = 0; i < 8; ++i)
        {
            if (Node->Children[i])
            {
                CollectedVertices.Append(GetAllVerticesInNode(Node->Children[i]));
            }
        }

        return CollectedVertices;
    }

    // 노드가 정점을 포함하는지 확인하는 함수
    bool IsInsideNode(FOctreeNode* Node, const FVector& Vertex)
    {
        return (Vertex.X >= Node->Center.X - Node->Extent.X && Vertex.X <= Node->Center.X + Node->Extent.X &&
                Vertex.Y >= Node->Center.Y - Node->Extent.Y && Vertex.Y <= Node->Center.Y + Node->Extent.Y &&
                Vertex.Z >= Node->Center.Z - Node->Extent.Z && Vertex.Z <= Node->Center.Z + Node->Extent.Z);
    }


    // 노드 삭제 함수
    void DeleteNode(FOctreeNode* Node)
    {
        if (Node)
        {
            for (int i = 0; i < 8; ++i)
            {
                DeleteNode(Node->Children[i]);
            }
            delete Node;
        }
    }


private:
    // 자식 노드 생성
    void CreateChildren(FOctreeNode* Node)
    {
        FVector ChildExtent = Node->Extent * 0.5f;
        for (int i = 0; i < 8; ++i)
        {
            FVector ChildCenter = Node->Center;
            ChildCenter.X += (i & 1) ? ChildExtent.X : -ChildExtent.X;
            ChildCenter.Y += (i & 2) ? ChildExtent.Y : -ChildExtent.Y;
            ChildCenter.Z += (i & 4) ? ChildExtent.Z : -ChildExtent.Z;
            Node->Children[i] = new FOctreeNode(ChildCenter, ChildExtent);
        }
    }

    // 정점이 어느 자식 노드에 들어가는지 계산
    int32 GetChildIndex(const FOctreeNode* Node, const FVector& Vertex)
    {
        int32 ChildIndex = 0;
        if (Vertex.X >= Node->Center.X) ChildIndex |= 1;
        if (Vertex.Y >= Node->Center.Y) ChildIndex |= 2;
        if (Vertex.Z >= Node->Center.Z) ChildIndex |= 4;
        return ChildIndex;
    }

    void AddUniqueVertex(FOctreeNode* Node, const FVector& Vertex)
    {
        // 정점이 이미 있는지 확인
        for (const FVector& ExistingVertex : Node->Vertices)
        {
            if (ExistingVertex.Equals(Vertex, KINDA_SMALL_NUMBER)) // 유사한 정점은 추가하지 않음
            {
                return;
            }
        }
        Node->Vertices.Add(Vertex); // 중복이 없을 때만 추가
    }

};

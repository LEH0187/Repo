struct FKDTreeNode
{
    FVector Vertex;
    FKDTreeNode* Left;
    FKDTreeNode* Right;

    FKDTreeNode(FVector InVertex) : Vertex(InVertex), Left(nullptr), Right(nullptr) {}
};

class TEST2_API FKDTree
{
public:
    FKDTreeNode* Root;

    FKDTree() : Root(nullptr) {}

    void BuildKDTree(TArray<FVector>& Vertex)
    {
        for(FVector& V : Vertex)
        {
            Insert(V);
        }
    }

    FKDTreeNode* AddNode(FKDTreeNode* Node, const FVector& Vertex, int Depth)
    {
        if (Node == nullptr)
        {
            return new FKDTreeNode(Vertex);
        }

        int Axis = Depth % 3;
        if (Vertex[Axis] < Node->Vertex[Axis])
        {
            Node->Left = AddNode(Node->Left, Vertex, Depth + 1);
        }
        else
        {
            Node->Right = AddNode(Node->Right, Vertex, Depth + 1);
        }

        return Node;
    }

    void Insert(const FVector& Vertex)
    {
        Root = AddNode(Root, Vertex, 0);
    }

    void FindNearest(FKDTreeNode* Node, const FVector& Target, int Depth, FVector& Best, float& BestDistSquared)
    {
        if (Node == nullptr)
        {
            return;
        }

        float DistSquared = FVector::DistSquared(Node->Vertex, Target);
        if (DistSquared < BestDistSquared)
        {
            Best = Node->Vertex;
            BestDistSquared = DistSquared;
        }

        int Axis = Depth % 3;
        if (Target[Axis] < Node->Vertex[Axis])
        {
            FindNearest(Node->Left, Target, Depth + 1, Best, BestDistSquared);
            if (FMath::Square(Node->Vertex[Axis] - Target[Axis]) < BestDistSquared)
            {
                FindNearest(Node->Right, Target, Depth + 1, Best, BestDistSquared);
            }
        }
        else
        {
            FindNearest(Node->Right, Target, Depth + 1, Best, BestDistSquared);
            if (FMath::Square(Node->Vertex[Axis] - Target[Axis]) < BestDistSquared)
            {
                FindNearest(Node->Left, Target, Depth + 1, Best, BestDistSquared);
            }
        }
    }

    FVector FindClosestVertex(const FVector& Target)
    {
        FVector Best;
        float BestDistSquared = MAX_FLT;
        FindNearest(Root, Target, 0, Best, BestDistSquared);
        return Best;
    }
    void FindPointsWithinBounds(FKDTreeNode* Node, const FVector& Min, const FVector& Max, int Depth, TArray<FVector>& FoundPoints)
    {
        if (Node == nullptr)
        {
            return;
        }

        // 현재 정점이 범위 내에 있는지 확인
        if (Node->Vertex.X >= Min.X && Node->Vertex.X <= Max.X &&
            Node->Vertex.Y >= Min.Y && Node->Vertex.Y <= Max.Y &&
            Node->Vertex.Z >= Min.Z && Node->Vertex.Z <= Max.Z)
        {
            FoundPoints.Add(Node->Vertex);
        }

        // 현재 축 기준으로 자식 노드를 탐색
        int Axis = Depth % 3;
        if (Min[Axis] < Node->Vertex[Axis])
        {
            FindPointsWithinBounds(Node->Left, Min, Max, Depth + 1, FoundPoints);
        }
        if (Max[Axis] >= Node->Vertex[Axis])
        {
            FindPointsWithinBounds(Node->Right, Min, Max, Depth + 1, FoundPoints);
        }
    }

    TArray<FVector> FindVerticesWithinBounds(const FVector& Min, const FVector& Max)
    {
        TArray<FVector> FoundPoints;
        FindPointsWithinBounds(Root, Min, Max, 0, FoundPoints);
        return FoundPoints;
    }

};

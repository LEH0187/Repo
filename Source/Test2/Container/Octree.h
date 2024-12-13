// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

template<typename T>
struct Node
{
	Node(T* _Parent, T _Data, FVector _Center, FVector _Extent)
		 : Parent(_Parent), Data(_Data), Center(_Center), Extent(_Extent) {}

	T Data;
	FVector Center;
	FVector Extent;

	T* Children[8];
	T* Parent;
};

template<typename T>
class TEST2_API Octree
{
public:
	Octree(int8 _MaxDepth, int32 _Extent, T _Data);
	~Octree();

	bool AddData(T _data);

private:
	Node<T>* Head;
	Node<T>* LastNodeParentNode;
private:
	int8 MaxDepth;
	int8 CurrentDepth;
	int32 Extent;
};

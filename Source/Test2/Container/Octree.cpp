// Fill out your copyright notice in the Description page of Project Settings.


#include "Container/Octree.h"

template<typename T>
Octree<T>::Octree(int8 _MaxDepth, int32 _Extent, T _Data) 
    : MaxDepth(_MaxDepth), Extent(_Extent)
{
    Head = new Node(nullptr, _Data, _Extent/2, _Extent);
}

template<typename T>
Octree<T>::~Octree()
{
    if(Head)
    {
        delete Head;
    }
}


template<typename T>
bool Octree<T>::AddData(T _data)
{
    
    return true;
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Chunk/Chunk.h"

template<typename T>
Chunk<T>::Chunk(int8 _ComponentsScale, int32 _ComponentsNum)
   : ComponentsScale(_ComponentsScale), ComponentsNum(_ComponentsNum)
{
}

template<typename T>
Chunk<T>::~Chunk()
{
    
}

template<typename T>
bool Chunk<T>::InsertItemAllChunck(T _Item)
{
    for(int8 i = 0; i < ComponentsNum; ++i)
    {
        for(int8 j = 0; j < ComponentsNum; ++j)
        {
            for(int8 k = 0; k < ComponentsNum; ++k)
            {
                Chunck[i][j][k].Add(_Item);
            }
        }
    }
}

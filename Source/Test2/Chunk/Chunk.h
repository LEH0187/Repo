// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

template<typename T>
class TEST2_API Chunk
{
public:
	/// @brief 	(_Scale x _Scale x _Scale)로 청크의 크기를 설정.
	/// @brief (_ComponentsNum x _ComponentsNum x _ComponentsNum)로 청크 내 컴포는트 수 정의.
	/// @param _ComponentsScale 	청크의 한 변의 크기 (n * n * n)
	/// @param _ComponentsNum 		청크 내부의 한 변의 컴포넌트 수 (n * n * n)
	Chunk(int8 _ComponentsScale, int32 _ComponentsNum);
	~Chunk();

	bool InsertItemAllChunck(T _Item);

private:
	int8 ComponentsScale;
	int32 ComponentsNum;

	TArray<TArray<TArray<T>>> Chunck;
};

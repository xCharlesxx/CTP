#pragma once
#include "CoreMinimal.h"

//inline void Copy(FVector* dest, const FVector* a) {
//	dest[0] = a->X;
//	dest[1] = a->Y;
//	dest[2] = a->Z;
//};
inline float Dist(const FVector* v1, const FVector* v2) 
{
	const float dx = v2->X - v1->X;
	const float dy = v2->Y - v1->Y;
	const float dz = v2->Z - v1->Z;
	return sqrtf(dx*dx + dy * dy + dz * dz);
};
inline unsigned int NextPow2(unsigned int v)
{
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
};
inline void Add(FVector* dest, const FVector* v1, const FVector* v2)
{
	dest->X = v1->X + v2->X;
	dest->Y = v1->Y + v2->Y;
	dest->Z = v1->Z + v2->Z;
};
inline bool Contains(AActor* find, TArray<AActor*> within)
{
	if (find == nullptr) { return false; }

	for (int i = 0; i < within.Num(); i++)
	{
		if (within[i] == find)
			return true; 
	}
	return false; 
};
inline bool Contains(FString* find, TArray<FString*> within)
{
	if (find == nullptr) { return false; }

	for (int i = 0; i < within.Num(); i++)
	{
		if (within[i] == find)
			return true;
	}
	return false;
};
//Obsolete - use FindComponentByClass<ClassName>()
//inline bool Contains(AAIController* find, AActor* within)
//{
//	if (find == nullptr || within == nullptr) { return false; }
//
//	TArray<AAIController*> controllerComponents; 
//	within->GetComponents(controllerComponents); 
//
//	if (controllerComponents.Num() > 0)
//	for (AAIController* aic : controllerComponents)
//	{
//		if (aic == find)
//			return true;
//	}
//	return false;
//}
inline FVector CalculateVectorBetweenPoints2D(const float speed, const FVector start, const FVector end)
{
	float dirX = end.X - start.X; 
	float dirY = end.Y - start.Y; 
	float distance = sqrt(pow(dirX, 2.0) + pow(dirY, 2.0)); 
	float time = speed / distance; 
	float velX = dirX * time; 
	float velY = dirX * time; 
	return FVector(velX,velY,0); 
};

/// Performs a vector subtraction. (@p v1 - @p v2)
///  @param[out]	dest	The result vector. [(x, y, z)]
///  @param[in]		v1		The base vector. [(x, y, z)]
///  @param[in]		v2		The vector to subtract from @p v1. [(x, y, z)]
inline void Sub(FVector* dest, const FVector* v1, const FVector* v2)
{
	dest->X = v1->X - v2->X;
	dest->Y = v1->Y - v2->Y;
	dest->Z = v1->Z - v2->Z;
};
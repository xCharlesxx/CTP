// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Navigation/PathFollowingComponent.h"
#include "Runtime/Engine/Classes/AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/AI/Navigation/RecastNavMesh.h"
#include "CPathFollowComponent.generated.h"

class UNavMovementComponent; 
class UCharacterMovementComponent; 

UCLASS()
class CTP_API UCPathFollowComponent : public UPathFollowingComponent
{
	GENERATED_BODY()
	UCPathFollowComponent(const FObjectInitializer& PCIP);
protected:
	UPROPERTY(transient)
	class UCharacterMovementComponent * CharacterMoveComp;
private: 
	 
public:	
	
	/** sets variables related to current move segment */
	virtual void SetMoveSegment(int32 SegmentStartIndex) override;
	// used to cache UCharacterMovementComponent we're using in SetMoveSegment implementation
	virtual void SetMovementComponent(UNavMovementComponent* MoveComp) override;
	/** follow current path segment */
	virtual void FollowPathSegment(float DeltaTime) override; 
	/** check state of path following, update move segment if needed */
	virtual void UpdatePathSegment() override; 
};


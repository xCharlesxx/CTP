// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviourTree.h"
#include "CMath.h"
#include "Perception/AIPerceptionComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

//UCLASS()
class CDecisionClass
{
	//GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	CDecisionClass(UAIPerceptionComponent* perceptionComponent);
	UAIPerceptionComponent* perceptionComponent;// = GetAIPerceptionComponent();
	AActor* UpdateVision(const TArray<AActor*>& Actors, const AActor* self);
	int Update(const int status);
	//bool aniAttack = false;
	//bool aniDie = false;
	//bool aniWounded = false;
	//bool aniFoundFriend = false;
	//bool aniFoundFoe = false;
protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;
	BehaviourTree behaviourTree; 

private: 
	TArray<AActor*> perceivedActors;
	TArray<AActor*> friends;
	TArray<AActor*> foes;
	bool hostileVisible = false; 
	FVector lastKnownVector; 
	FVector searchVector; 
	//EStatusEnum* status; 
	int alyState = 0; 
private:
	//UAIBlueprintHelperLibrary::GetBlackboard(0);
	// Called every frame
	
};

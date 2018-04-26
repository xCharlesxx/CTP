// Fill out your copyright notice in the Description page of Project Settings.

#include "CDecisionClass.h"
#include "CustomController.h"
#include "Engine/World.h"

CDecisionClass::CDecisionClass(UAIPerceptionComponent* PC, const AActor* self)
{
	perceptionComponent = PC;
	perceptionComponent->GetCurrentlyPerceivedActors(0, perceivedActors);
	mySelf = (ACustomController*)self;

	for (int i = 0; i < perceivedActors.Num(); i++)
	if (!perceivedActors[i]->ActorHasTag("CAI"))
	{
		foes.Add(perceivedActors[i]); 
		break;
	}
	BehaviourTree::Selector* selector1 = new BehaviourTree::Selector;
	BehaviourTree::Selector* selector2 = new BehaviourTree::Selector;
	BehaviourTree::AllyState* allyState = new BehaviourTree::AllyState (&alyState, &myState);

	behaviourTree.setRootChild(&selector1[0]); 
	//Has Ally
	selector1[0].AddChild({&selector2[0]});
	//Has check ally state
	selector2[0].AddChild({allyState});
}

AActor* CDecisionClass::UpdateVision(const TArray<AActor*>& Actors)
{
	UE_LOG(LogTemp, Warning, TEXT("///////////////////"));
	UE_LOG(LogTemp, Warning, TEXT("My Name is: %s"), *mySelf->GetName());
	UE_LOG(LogTemp, Warning, TEXT("///////////////////"));

	//For each update in perception
	for (int i = 0; i < Actors.Num(); i++)
	{
		//If element update contains known Actor, remove Actor
		if (Contains(Actors[i], friends))
			friends.Remove(Actors[i]);
		else if (Contains(Actors[i], foes))
		{
			foes.Remove(Actors[i]);
			lastKnownVector = Actors[i]->GetActorLocation();
			UE_LOG(LogTemp, Warning, TEXT("Lost Foe"));
			UE_LOG(LogTemp, Warning, TEXT("Last Known Vector: %s"), *(lastKnownVector.ToString()));
			//DrawDebugSphere(GetWorld(), lastKnownVector, 24, 32, FColor(255, 0, 0));
		}
		//else if actor is friend, add friend && not self
		else if (Actors[i]->ActorHasTag("CAI") && Actors[i] != mySelf)
			friends.Add(Actors[i]);
		//else if actor is foe, add foe
		else if (Actors[i]->ActorHasTag("Player"))
		{
			foes.Add(Actors[i]);
			hostileVisible = true; 
		}
		else 
			UE_LOG(LogTemp, Warning, TEXT("Error, Unknown Actor Percieved (Possibly self)"));
	}
	UE_LOG(LogTemp, Warning, TEXT("Friends Visible: %i"), friends.Num());
	UE_LOG(LogTemp, Warning, TEXT("Foes Visible: %i"), foes.Num());
	UE_LOG(LogTemp, Warning, TEXT("%s"), *BTSuccess);
	if (foes.Num() > 0)
	{
		return foes[0]; 
	}
	if (friends.Num() > 0)
	{
		//aniFoundFoe = true;
		ACustomController* foundCC = (ACustomController*)friends[0];
		UE_LOG(LogTemp, Warning, TEXT("Friend 0 Has Status: %i"), uint8(foundCC->status));
		return friends[0];
	}
	return nullptr; 
}

//Cannot pass enum in any way, casting as int
int CDecisionClass::Update(const int stats)
{
	myState = (int)mySelf->status; 
	//If immobilized return same status 
	if (stats == 2 || stats == 3)
		return stats; 
	//Get Ally Status
	if (friends.Num() > 0)
	{
		ACustomController* foundCC = (ACustomController*)friends[0];
		alyState = (int)foundCC->status;
	}
	if (hostileVisible)
	{
		hostileVisible = false; 
		//Return FoundFoe
		return 7;
	}
	int result = behaviourTree.run();
	if (result == 0)
	{
		FString newBehaviour = "Null: " + FString::FromInt(result);
		if (newBehaviour != BTSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("My Name is %s"), *mySelf->GetName());
			UE_LOG(LogTemp, Warning, TEXT("Behaviour tree returned %s"), *newBehaviour);
			BTSuccess = newBehaviour; 
		}
		//UE_LOG(LogTemp, Warning, TEXT("Behaviour tree returned Null"));
		return stats;
	}
	else
	{
		FString newBehaviour = "Behaviour: " + FString::FromInt(result);
		if (newBehaviour != BTSuccess)
		{
			UE_LOG(LogTemp, Warning, TEXT("My Name is %s"), *mySelf->GetName());
			UE_LOG(LogTemp, Warning, TEXT("Behaviour tree returned %s"), *newBehaviour);
			BTSuccess = newBehaviour;
		}
		
		return result;
	}
}




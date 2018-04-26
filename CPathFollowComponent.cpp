// Fill out your copyright notice in the Description page of Project Settings.


#include "CPathFollowComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "CUNavAreaJump.h"
#include "CMath.h"
#include "GameFramework/CharacterMovementComponent.h"

 UCPathFollowComponent::UCPathFollowComponent(const FObjectInitializer& PCIP) : Super(PCIP)
 {
	
 }

 void UCPathFollowComponent::SetMovementComponent(UNavMovementComponent* MoveComp)
 {
	 UE_LOG(LogTemp, Warning, TEXT("SetMovementComponent"));
	 Super::SetMovementComponent(MoveComp);

	 CharacterMoveComp = Cast<UCharacterMovementComponent>(MovementComp);
 }

 void UCPathFollowComponent::FollowPathSegment(float DeltaTime)
 {
	 ////Pointer Safety Checks
	 //if (MovementComp == NULL || !Path.IsValid())
	 //{
		// return;
	 //}
	 ////~~~~~~~~~~~~~~~~~~~~~~~~~
	 ////~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 ////Use Jump/Fall Pathing?
	 ////~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 //if (Path->IsPartial()) //AI could not reach player, try using jump pathing!
	 //{
		// //I send out instructions to my custom character class here
		//// customController->ReceiveJumpFallPathingRequest();

		// return;
		// //~~~
	 //}
	 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 //Proceed normally (no jump pathing)
	 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	 Super::FollowPathSegment(DeltaTime);
 }

 void UCPathFollowComponent::UpdatePathSegment()
 {
	 Super::UpdatePathSegment();
	 //if (Path->IsPartial()) //AI could not reach player, try using jump pathing!
	 //{
		// //I send out instructions to my custom character class here
		// // customController->ReceiveJumpFallPathingRequest();
		// //Path.
		//// return;
		// //~~~
	 //} 
 }
 
 void UCPathFollowComponent::SetMoveSegment(int32 SegmentStartIndex)
 {
	UE_LOG(LogTemp, Warning, TEXT("SetMoveSegment"));
	Super::SetMoveSegment(SegmentStartIndex);
	
	if (CharacterMoveComp != NULL)
	{
		const FNavPathPoint& SegmentStart = Path->GetPathPoints()[MoveSegmentStartIndex];
		//If node has Jump flag then jump
		if (FNavAreaHelper::HasJumpFlag(SegmentStart))
		{
			UE_LOG(LogTemp, Warning, TEXT("JUMPING"));
			//CharacterMoveComp->SetMovementMode(MOVE_);
			float distanceUp = Path->GetPathPoints()[MoveSegmentStartIndex + 1].Location.Z - SegmentStart.Location.Z;
			float distanceForward = Dist(&Path->GetPathPoints()[MoveSegmentStartIndex + 1].Location, &SegmentStart.Location);
			CharacterMoveComp->JumpZVelocity = distanceUp;//(distanceForward + distanceUp) *1.25;
			FVector location = Path->GetPathPoints()[MoveSegmentStartIndex].Location;
			FVector destination = Path->GetPathPoints()[MoveSegmentStartIndex + 1].Location; 
			FVector direction(destination.X - location.X, destination.Y - location.Y, 0);
			CharacterMoveComp->Velocity = direction; 
			CharacterMoveComp->AirControl = 1; 
			CharacterMoveComp->DoJump(true);
			UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), distanceForward);
			UE_LOG(LogTemp, Warning, TEXT("Height: %f"), distanceUp);
			//CharacterMoveComp->Launch(CharacterMoveComp->GetCurrentAcceleration());
		}
		else
		{
			// regular move
			UE_LOG(LogTemp, Warning, TEXT("Walking"));
			CharacterMoveComp->SetMovementMode(MOVE_Walking);
		}
	}
 }




// Sets default values for this component's properties
//AUCCPathFollowComponent::AUCCPathFollowComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<CCPathFollowComponent>(TEXT("PathFollowComponent")));
//{
//	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
//	// off to improve performance if you don't need them.
//	PrimaryComponentTick.bCanEverTick = true;
//
//	// ...
//}
//
//
//// Called when the game starts
//void AUCCPathFollowComponent::BeginPlay()
//{
//	//Super::BeginPlay();
//
//	// ...
//	
//}
//

// Called every frame
//void UCCPathFollowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	//Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	// ...
//}


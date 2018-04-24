// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Ball.generated.h"

UCLASS()
class CTP_API ABall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABall(const FObjectInitializer& PCIP);
	void Colour(FColor colour); 

protected:
	// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

private: 
	USphereComponent * sphere; 
	// Called every frame
	//virtual void Tick(float DeltaTime) override;
};

//UCLASS()
//class AICTP_API AGreenBall : public AActor
//{
//	GENERATED_BODY()
//
//public:
//	// Sets default values for this actor's properties
//	AGreenBall(const FObjectInitializer& PCIP);
//	void Colour(FColor colour);
//
//protected:
//	// Called when the game starts or when spawned
//	//virtual void BeginPlay() override;
//
//private:
//	USphereComponent * sphere;
//	// Called every frame
//	//virtual void Tick(float DeltaTime) override;
//};

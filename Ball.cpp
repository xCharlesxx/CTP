// Fill out your copyright notice in the Description page of Project Settings.

#include "Ball.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"


// Sets default values
ABall::ABall(const FObjectInitializer& PCIP)
{
	sphere = PCIP.CreateDefaultSubobject<USphereComponent>(this, FName("My sphere"));
	RootComponent = sphere;
	sphere->bHiddenInGame = false;
	//sphere->CreationMethod = EComponentCreationMethod::Native; 
	//sphere->OnComponentCreated(); 
	//sphere->bGenerateOverlapEvents = false; 
	sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	sphere->SetMobility(EComponentMobility::Movable);
	//sphere->RegisterComponent(); 
	//sphere->ShapeColor = FColor::Green;
	//sphere->RegisterComponent(); 
	/*sphere->ShapeColor = FColor::Green;
	sphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere Shell")); 
	sphereMesh->AttachTo(RootComponent); 
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Game/Geometry/Meshes/Shape_Sphere.Shape_Sphere"));
	if (SphereMeshAsset.Succeeded())
	{
		sphereMesh->SetStaticMesh(SphereMeshAsset.Object);
		sphereMesh->SetRelativeLocation(FVector().ZeroVector);
	}
	sphereMesh->SetWorldScale3D(FVector().OneVector/2); 
	sphereMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	sphereMesh->SetMobility(EComponentMobility::Movable);
	material = sphereMesh->GetMaterial(1);
	dynamicMat = UMaterialInstanceDynamic::Create(sphereMesh->GetMaterial(1), NULL);
	dynamicMat->SetVectorParameterValue(FName("Colour"), FLinearColor::Black);
	sphereMesh->SetMaterial(0, dynamicMat); */
}

void ABall::Colour(FColor colour)
{
	//dynamicMat->SetVectorParameterValue(FName(TEXT("Colour")), colour); 
	//USphereComponent * sphere2 = NewObject<USphereComponent>(this); 
	//sphere2->bHiddenInGame = false; 
	//sphere2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//sphere2->SetMobility(EComponentMobility::Movable);
	sphere->ShapeColor = colour;
}

// Called when the game starts or when spawned
//void ABall::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}
//
//// Called every frame
//void ABall::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}



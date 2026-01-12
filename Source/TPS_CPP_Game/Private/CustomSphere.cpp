// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomSphere.h"

// Sets default values
ACustomSphere::ACustomSphere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(Root);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ACustomSphere::BeginPlay()
{
	Super::BeginPlay();

	health = 100;
}

// Called every frame
void ACustomSphere::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACustomSphere::ApplyDamage_Implementation(const int dmg)
{
	health -= dmg;

	UE_LOG(LogTemp, Warning, TEXT("Health: %d"), health);

	if (health <= 0) {
		Destroy();
	}
}


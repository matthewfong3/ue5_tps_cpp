// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageInterface.h"
#include "CustomCube.generated.h"

UCLASS()
class TPS_CPP_GAME_API ACustomCube : public AActor, public IDamageInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	UPROPERTY(EditAnywhere, Category = "Health")
	int health;
	
public:	
	// Sets default values for this actor's properties
	ACustomCube();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ApplyDamage_Implementation(const int dmg);
};

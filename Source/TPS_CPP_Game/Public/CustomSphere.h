// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageInterface.h"
#include "CustomSphere.generated.h"

UCLASS()
class TPS_CPP_GAME_API ACustomSphere : public AActor, public IDamageInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMesh;
	
public:	
	// Sets default values for this actor's properties
	ACustomSphere();

	UPROPERTY(EditAnywhere, Category = "Health")
	int health;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ApplyDamage_Implementation(const int dmg) override;
};

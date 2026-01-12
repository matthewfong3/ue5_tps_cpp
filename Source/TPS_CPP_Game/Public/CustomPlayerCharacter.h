// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "Animation/AnimInstance.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "CustomPlayerInputBindings.h"
#include "CustomPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UAnimMontage;
class TimerManager;

UCLASS()
class TPS_CPP_GAME_API ACustomPlayerCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	ACustomPlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Getters & Setters
	UFUNCTION(BlueprintCallable)
	bool GetIsSprinting();
	UFUNCTION(BlueprintCallable)
	void SetIsSprinting(bool InIsSprinting);

	UFUNCTION(BlueprintCallable)
	bool GetIsCrouching();
	UFUNCTION(BlueprintCallable)
	void SetIsCrouching(bool InIsCrouching);

	UFUNCTION(BlueprintCallable)
	bool GetIsShooting();
	UFUNCTION(BlueprintCallable)
	void SetIsShooting(bool InIsShooting);

	UFUNCTION(BlueprintCallable)
	bool GetIsADS();
	UFUNCTION(BlueprintCallable)
	void SetIsADS(bool InIsADS);

	UFUNCTION(BlueprintCallable)
	bool GetIsReloading();
	UFUNCTION(BlueprintCallable)
	void SetIsReloading(bool InIsReloading);
protected:
	// Crouching Fields
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Crouch")
	FVector CrouchEyeOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Crouch")
	float CrouchSpeed;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool BCanSprint();
	bool BCanCrouch();
	bool BCanShoot();
	bool BCanADS();
	bool BCanReload();

private:
	// CONSTANTS
	const float RUNNING_SPEED = 600.f;
	const float WALKING_SPEED = 300.f;
	const float CROUCH_SPEED = 150.f;
	const float BULLET_DISTANCE = 10000.0f;

	//UCustomPlayerInputBindings* PIB;
	// Input Mapping Contexts
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UInputMappingContext> DefaultInputMappingContext;

	// Enhanced Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MouseLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ShootAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ADSAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ReloadAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CrouchAction;

	// HUD
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> UserWidgetClass;

	// Camera Fields
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	// Weapon Mesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	// Shooting Animation Montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> ShootAnimMontage;

	// Reload Animation Montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> ReloadAnimMontage;

	// Gunshot muzzle effect
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Particle System", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UParticleSystem> GunShotMuzzleEffect;

	// Player State Fields
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsMoving;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsSprinting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsShooting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsADS;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsCrouching;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsReloading;
	
	// Input Mapping Context
	//void AddMappingContext();
	void BindEnhancedInput(UInputComponent* PlayerInputComponent);

	// Enhanced Input Action Functions
	void Move(const FInputActionValue& Value);
	void StopMove();
	void MouseLook(const FInputActionValue& Value);

	void StartJump();
	void StopJump();

	void Sprint();
	void StopSprinting();

	void ADS();
	void StopADS();

	void Reload();

	// Fire Weapon Fields
	bool OpenFireGate = true;
	FTimerHandle FireTimerHandle;

	// Delegate for Firing Timer Event
	UFUNCTION()
	void FireWeapon();
	void Fire();
	void FireLineTrace();
	void StopFiring();

	// Helper Methods for Firing Weapon
	void PlayAnimationMontage(UAnimMontage* AnimMontage);
	void SpawnGunShotMuzzleEffect();

	void StartCrouch();
	void StopCrouch();

	// Crouching Override Methods
	void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

	void OrientRotationToMovement(bool Orient);
};

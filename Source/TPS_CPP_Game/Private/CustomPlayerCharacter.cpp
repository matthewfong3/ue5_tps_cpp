// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "DamageInterface.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PrintStrings.h"

// Sets default values
ACustomPlayerCharacter::ACustomPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//PIB = CreateDefaultSubobject<UCustomPlayerInputBindings>(TEXT("Custom Player Input Bindings"));

	// Set up Camera components
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	CameraBoom->SetupAttachment(RootComponent);

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Set up weapon mesh component
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	WeaponMesh->SetupAttachment(GetMesh());

	// Initialize character/camera rotation settings
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Initialize Player States Fields
	bIsMoving = false;
	bIsSprinting = false;
	bIsShooting = false;
	bIsADS = false;
	bIsCrouching = false;

	// Initialize Crouching Fields
	CrouchEyeOffset = FVector(0.f);
	CrouchSpeed = 12.f;
}

// Called when the game starts or when spawned
void ACustomPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Initialize Skeletal Mesh location and rotation
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));

	// Initialize Camera Boom length and offset postion
	CameraBoom->TargetArmLength = 200.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 50.0f, 10.0f);

	// Attach Weapon Mesh to Skeletal Mesh's WeaponSocket
	WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket_R"));

	// Bind the Fire Delegate to the name of FireWeapon() from "this" object
	FTimerDelegate FireDelegate;
	FireDelegate.BindUFunction(this, FName("FireWeapon"));

	// Create Widget HUD
	if (UserWidgetClass) 
	{
		if (UUserWidget* playerHUD = CreateWidget<UUserWidget>(Cast<APlayerController>(GetController()), UserWidgetClass))
		{
			playerHUD->AddToViewport();
		}
	}
}

// Called every frame
void ACustomPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CrouchInterpTime = FMath::Min(1.f, CrouchSpeed * DeltaTime);
	CrouchEyeOffset = (1.f - CrouchInterpTime) * CrouchEyeOffset;
}

bool ACustomPlayerCharacter::GetIsSprinting()
{
	return bIsSprinting;
}

void ACustomPlayerCharacter::SetIsSprinting(bool InIsSprinting)
{
	bIsSprinting = InIsSprinting;
}

bool ACustomPlayerCharacter::GetIsCrouching()
{
	return bIsCrouching;
}

void ACustomPlayerCharacter::SetIsCrouching(bool InIsCrouching)
{
	bIsCrouching = InIsCrouching;
}

bool ACustomPlayerCharacter::GetIsShooting()
{
	return bIsShooting;
}

void ACustomPlayerCharacter::SetIsShooting(bool InIsShooting)
{
	bIsShooting = InIsShooting;
}

bool ACustomPlayerCharacter::GetIsADS() {
	return bIsADS;
}

void ACustomPlayerCharacter::SetIsADS(bool InIsADS)
{
	bIsADS = InIsADS;
}

bool ACustomPlayerCharacter::GetIsReloading() 
{
	return bIsReloading;
}

void ACustomPlayerCharacter::SetIsReloading(bool InIsReloading)
{
	bIsReloading = InIsReloading;
}

void ACustomPlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementInput = Value.Get<FVector2D>();

	if (GetController() != nullptr) {
		bIsMoving = true;
		OrientRotationToMovement(false);

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator RotationYaw(0, Rotation.Yaw, 0);
		const FRotator RotationYawRoll(0, Rotation.Yaw, Rotation.Roll);

		const FVector ControlRotationForward = FRotationMatrix(RotationYaw).GetUnitAxis(EAxis::X);
		const FVector ControlRotationRight = FRotationMatrix(RotationYawRoll).GetUnitAxis(EAxis::Y);

		AddMovementInput(ControlRotationForward, MovementInput.Y);
		AddMovementInput(ControlRotationRight, MovementInput.X);
	}
}

void ACustomPlayerCharacter::StopMove()
{
	bIsMoving = false;
	OrientRotationToMovement(true);
}

void ACustomPlayerCharacter::MouseLook(const FInputActionValue& Value)
{
	const FVector2D MouseInput = Value.Get<FVector2D>();

	if (GetController() != nullptr) {

		AddControllerYawInput(MouseInput.X);
		AddControllerPitchInput(MouseInput.Y);
	}
}

bool ACustomPlayerCharacter::BCanSprint()
{
	return !bIsShooting && !bIsADS && GetCharacterMovement()->IsMovingOnGround() && !bIsReloading;
}

bool ACustomPlayerCharacter::BCanCrouch()
{
	return !GetCharacterMovement()->IsFalling();
}

bool ACustomPlayerCharacter::BCanShoot()
{
	return !bIsSprinting && !bIsReloading;
}

bool ACustomPlayerCharacter::BCanADS()
{
	return !bIsReloading && !bIsSprinting;
}

bool ACustomPlayerCharacter::BCanReload()
{
	return !bIsReloading && !bIsShooting;
}

void ACustomPlayerCharacter::StartJump()
{
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		Jump();
	}
}

void ACustomPlayerCharacter::StopJump()
{
	StopJumping();
}

void ACustomPlayerCharacter::Sprint()
{
	if (!BCanSprint())
	{
		return;
	}

	StopADS();
	OrientRotationToMovement(false);
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = RUNNING_SPEED;
}

void ACustomPlayerCharacter::StopSprinting()
{
	OrientRotationToMovement(true);
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = WALKING_SPEED;
}

void ACustomPlayerCharacter::StartCrouch()
{
	if (!BCanCrouch())
	{
		return;
	}
	Crouch();
	bIsCrouching = true;
	GetCharacterMovement()->MaxWalkSpeed = CROUCH_SPEED;
}

void ACustomPlayerCharacter::StopCrouch()
{
	UnCrouch();
	bIsCrouching = false;
	GetCharacterMovement()->MaxWalkSpeed = WALKING_SPEED;
}

void ACustomPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (HalfHeightAdjust == 0.f)
	{
		return;
	}

	float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	if (GetMesh() && DefaultChar->GetMesh())
	{
		//FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		FVector MeshRelativeLocation = GetMesh()->GetRelativeLocation();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z - HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
		GetMesh()->SetRelativeLocation(MeshRelativeLocation);
	}

	CrouchEyeOffset.Z += StartBaseEyeHeight - BaseEyeHeight + HalfHeightAdjust;
	if (FollowCamera)
	{
		FollowCamera->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight), false);
	}
}

void ACustomPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (HalfHeightAdjust == 0.f)
	{
		return;
	}

	float StartBaseEyeHeight = BaseEyeHeight;
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	if (GetMesh() && DefaultChar->GetMesh())
	{
		//FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		FVector MeshRelativeLocation = GetMesh()->GetRelativeLocation();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z - HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
		GetMesh()->SetRelativeLocation(MeshRelativeLocation);
	}

	CrouchEyeOffset.Z += StartBaseEyeHeight - BaseEyeHeight - HalfHeightAdjust;


}

void ACustomPlayerCharacter::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	if (FollowCamera)
	{
		FollowCamera->GetCameraView(DeltaTime, OutResult);
		OutResult.Location += CrouchEyeOffset;
	}
}

void ACustomPlayerCharacter::OrientRotationToMovement(bool Orient)
{
	bUseControllerRotationYaw = !Orient;
	GetCharacterMovement()->bOrientRotationToMovement = Orient;
}

void ACustomPlayerCharacter::Fire()
{
	if (!BCanShoot()) 
	{
		return;
	}
	
	if (!OpenFireGate)
	{
		return;
	}

	bIsShooting = true;
	OpenFireGate = false;
	OrientRotationToMovement(false);
	const float FireRate = 0.09f;
	GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ACustomPlayerCharacter::FireWeapon, FireRate, true);
}

void ACustomPlayerCharacter::FireWeapon()
{
	FireLineTrace();
	PlayAnimationMontage(ShootAnimMontage);
	SpawnGunShotMuzzleEffect();
}

void ACustomPlayerCharacter::FireLineTrace()
{
	FVector CameraTraceStart = FollowCamera->GetComponentLocation();
	FVector CameraTraceEnd = FollowCamera->GetComponentLocation() + FollowCamera->GetForwardVector() * BULLET_DISTANCE;
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);

	// Step 1: Line trace from camera location to where crosshair is aiming
	bool bCameraHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraTraceStart, CameraTraceEnd, ECollisionChannel::ECC_Camera, TraceParams);

	// Step 2: Use the weapon muzzle socket location as the start location of bullet trace
	FVector BulletTraceStart = WeaponMesh->GetSocketLocation(TEXT("MuzzleSocket"));
	FVector BulletTraceEnd = bCameraHit ? HitResult.ImpactPoint : HitResult.TraceEnd;

	// Step 3: If camera's line trace hit something, draw bullet trace to the impact point
	//		   Else, draw bullet trace to the end of the line trace
	if (bCameraHit) 
	{
		DrawDebugLine(GetWorld(), BulletTraceStart, BulletTraceEnd, FColor::Green, false, 5.0f, 0, 1.0f);

		AActor* HitActor = HitResult.GetActor();

		if (HitActor->GetClass()->ImplementsInterface(UDamageInterface::StaticClass())) 
		{
			int32 BulletDamage = 20;
			IDamageInterface::Execute_ApplyDamage(HitActor, BulletDamage);
		}

		printf_k(-1, "Hit: %s", *HitActor->GetClass()->GetName());
	}
	else 
	{
		DrawDebugLine(GetWorld(), BulletTraceStart, BulletTraceEnd, FColor::Red, false, 5.0f, 0, 1.0f);
	}
}

void ACustomPlayerCharacter::StopFiring()
{
	OrientRotationToMovement(true);
	bIsShooting = false;
	OpenFireGate = true;
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ACustomPlayerCharacter::ADS()
{
	if (!BCanADS())
	{
		return;
	}

	StopSprinting();
	OrientRotationToMovement(false);
	bIsADS = true;
	FRotator TargetRotation = FRotator(GetActorRotation().Pitch, GetBaseAimRotation().Yaw, GetActorRotation().Roll);
	SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, GetWorld()->GetDeltaSeconds(), 20.f));
	FollowCamera->SetFieldOfView(FMath::FInterpTo(FollowCamera->FieldOfView, 60.f, GetWorld()->GetDeltaSeconds(), 120.f));
}

void ACustomPlayerCharacter::StopADS()
{
	OrientRotationToMovement(true);
	bIsADS = false;
	FollowCamera->SetFieldOfView(FMath::FInterpTo(FollowCamera->FieldOfView, 90.f, GetWorld()->GetDeltaSeconds(), 120.f));
}

void ACustomPlayerCharacter::Reload()
{
	if (!BCanReload())
	{
		return;
	}

	bIsReloading = true;
	PlayAnimationMontage(ReloadAnimMontage);
}

void ACustomPlayerCharacter::PlayAnimationMontage(UAnimMontage* AnimMontage)
{
	if (GetMesh()) 
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && AnimMontage)
		{
			AnimInstance->Montage_Play(AnimMontage, 1.0f);
		}
	}
}

void ACustomPlayerCharacter::SpawnGunShotMuzzleEffect()
{
	if (GunShotMuzzleEffect) 
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			GunShotMuzzleEffect,
			WeaponMesh->GetSocketLocation(TEXT("MuzzleSocket")),
			FRotator::ZeroRotator,
			FVector(1.0f),
			true);
	}
}

// Called to bind functionality to input
void ACustomPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	//if (PIB)
	//{
	//	PIB->AddMappingContext();
	//}

	BindEnhancedInput(PlayerInputComponent);
}

/*
void ACustomPlayerCharacter::AddMappingContext()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) 
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) 
		{
			Subsystem->AddMappingContext(DefaultInputMappingContext, 0);
		}
	}
}
*/
void ACustomPlayerCharacter::BindEnhancedInput(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		// Bind Move function to move input action
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACustomPlayerCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ACustomPlayerCharacter::StopMove);

		// Bind Mouse Look function to Mouse Look input action
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ACustomPlayerCharacter::MouseLook);

		// Bind Jump function to jump input action
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACustomPlayerCharacter::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACustomPlayerCharacter::StopJump);

		// Bind Sprint function to sprint input action
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ACustomPlayerCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ACustomPlayerCharacter::StopSprinting);

		// Bind Crouch function to crouch input action
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ACustomPlayerCharacter::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ACustomPlayerCharacter::StopCrouch);

		// Bind Fire function to shoot input action
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ACustomPlayerCharacter::Fire);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Completed, this, &ACustomPlayerCharacter::StopFiring);

		// Bind ADS function to ADS input action
		EnhancedInputComponent->BindAction(ADSAction, ETriggerEvent::Started, this, &ACustomPlayerCharacter::ADS);
		EnhancedInputComponent->BindAction(ADSAction, ETriggerEvent::Completed, this, &ACustomPlayerCharacter::StopADS);

		// Bind Reload function to reload input action
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ACustomPlayerCharacter::Reload);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerInputBindings.h"
#include "GameFramework/Pawn.h"

// Sets default values for this component's properties
UCustomPlayerInputBindings::UCustomPlayerInputBindings()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCustomPlayerInputBindings::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UCustomPlayerInputBindings::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCustomPlayerInputBindings::AddMappingContext()
{
	APawn PlayerPawn = APawn();

	if (APlayerController* PlayerController = Cast<APlayerController>(PlayerPawn.GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultInputMappingContext, 0);
		}
	}
}
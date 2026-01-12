// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomAnimNotify.h"
#include <CustomPlayerCharacter.h>

void UCustomAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner()) {
		if (ACustomPlayerCharacter* player = Cast<ACustomPlayerCharacter>(MeshComp->GetOwner()))
		{
			player->SetIsReloading(false);
			UE_LOG(LogTemp, Warning, TEXT("Finish Reloading"));
		}
	}
}

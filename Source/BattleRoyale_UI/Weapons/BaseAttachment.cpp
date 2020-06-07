// Copyright Juan Marcelo Portillo. All Rights Reserved


#include "BaseAttachment.h"
#include "../BattleRoyaleSettings.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"

// Sets default values
ABaseAttachment::ABaseAttachment()
{
	AttachmentMesh = CreateDefaultSubobject<UStaticMeshComponent>("Attachment Mesh");
	AttachmentMesh->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);

	RootComponent = AttachmentMesh;
	PhysicsComponent = AttachmentMesh;

	AttachmentData = nullptr;

	SetPhysics(true);

}

void ABaseAttachment::UpdateSettings()
{
	if (UBattleRoyaleSettings* Settings = GetMutableDefault<UBattleRoyaleSettings>())
	{
		AttachmentData = Settings->FindAttachment(ItemName);
		if (AttachmentData)
			AttachmentMesh->SetStaticMesh(AttachmentData->AttachmentMesh);
		else
			AttachmentMesh->SetStaticMesh(NULL);
	}
}

void ABaseAttachment::SetWeaponOwner(ABaseWeapon* Weapon)
{
	WeaponOwner = Weapon;
}


bool ABaseAttachment::GetAttachmentData(FWeaponAttachmentData& outAttachmentData) const
{
	if (AttachmentData)
	{
		outAttachmentData = *AttachmentData;
		return true;
	}
	return false;
}


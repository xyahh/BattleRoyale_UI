// Copyright Juan Marcelo Portillo. All Rights Reserved

#include "BaseWeapon.h"
#include "BaseAttachment.h"
#include "../BattleRoyaleSettings.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/CollisionProfile.h"

ABaseWeapon::ABaseWeapon()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon Mesh");
	WeaponMesh->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);

	RootComponent = WeaponMesh;
	PhysicsComponent = WeaponMesh;

	WeaponData = nullptr;

	SetPhysics(true);
}

void ABaseWeapon::UpdateSettings()
{
	if (UBattleRoyaleSettings* Settings = GetMutableDefault<UBattleRoyaleSettings>())
	{
		WeaponData = Settings->FindWeapon(ItemName);
		if (WeaponData)
			WeaponMesh->SetSkeletalMesh(WeaponData->WeaponMesh);
		else
			WeaponMesh->SetSkeletalMesh(NULL);
	}
}

bool ABaseWeapon::GetWeaponData(FWeaponCoreData& outWeaponData) const
{
	if (WeaponData)
	{
		outWeaponData = *WeaponData;
		return true;
	}
	return false;
}

bool ABaseWeapon::InstallAttachment(ABaseAttachment* Attachment, bool bOverrideCurrentAttachment)
{
	FWeaponAttachmentData AttachmentData;
	if (Attachment && Attachment->GetAttachmentData(AttachmentData))
	{
		//Check if there's a Current Attachment installed in our Spot
		if (ABaseAttachment** CurrAtt = CurrentAttachments.Find(AttachmentData.WeaponSlot))
		{
			if (bOverrideCurrentAttachment)
				UninstallAttachment(*CurrAtt);
			else 
				return false; //There's currently a 
		}

		CurrentAttachments.Add(AttachmentData.WeaponSlot, Attachment);
		return true;
	}
	return false;
}

bool ABaseWeapon::UninstallAttachment(ABaseAttachment* Attachment)
{
	FWeaponAttachmentData AttachmentData;
	if (Attachment && Attachment->GetAttachmentData(AttachmentData))
	{
		if (ABaseAttachment** CurrAtt = CurrentAttachments.Find(AttachmentData.WeaponSlot))
		{
			//Check if the Found Attachment at the Slot is the same Attachment, 
			//if so, remove the Slot from the Map and return true
			if (*CurrAtt == Attachment)
			{
				CurrentAttachments.Remove(AttachmentData.WeaponSlot);
				return true;
			}
		}
	}
	return false;
}

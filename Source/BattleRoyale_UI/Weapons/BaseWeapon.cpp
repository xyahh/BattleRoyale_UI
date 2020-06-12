// Copyright Juan Marcelo Portillo. All Rights Reserved

#include "BaseWeapon.h"
#include "BaseAttachment.h"
#include "../BattleRoyaleSettings.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/CollisionProfile.h"

ABaseWeapon::ABaseWeapon()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	WeaponMesh->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	Camera->SetWorldScale3D(FVector(0.1f));
	Camera->SetupAttachment(WeaponMesh);

	RootComponent = WeaponMesh;
	PhysicsComponent = WeaponMesh;
	
	WeaponData = nullptr;

	SetPhysics(true);
}

void ABaseWeapon::UpdateSettings()
{
	if (!WeaponMesh) return;

	if (UBattleRoyaleSettings* Settings = GetMutableDefault<UBattleRoyaleSettings>())
	{
		WeaponData = Settings->FindWeapon(ItemName);
		if (WeaponData)
			WeaponMesh->SetSkeletalMesh(WeaponData->WeaponMesh);
		else
			WeaponMesh->SetSkeletalMesh(NULL);
	}
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	UpdateCamera();
}

bool ABaseWeapon::GetWeaponData(FWeaponCoreData& outWeaponData
	, bool bCombineAttachmentData) const
{
	if (WeaponData)
	{
		outWeaponData = *WeaponData;
		if (bCombineAttachmentData)
		{
			for (auto& Attachment : CurrentAttachments)
			{
				FWeaponAttachmentData AttachmentData;
				if (Attachment.Value->GetAttachmentData(AttachmentData))
					outWeaponData.BaseData += AttachmentData.AttachmentStats;
			}
		}

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
		UpdateCamera();
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
				UpdateCamera();
				return true;
			}
		}
	}
	return false;
}

void ABaseWeapon::UpdateCamera()
{
	FWeaponCoreData CameraData;
	if (Camera && WeaponMesh && GetWeaponData(CameraData, true))
	{
		FVector SocketLocation = WeaponMesh->GetSocketTransform(CameraData.AimSocket
			, ERelativeTransformSpace::RTS_Component).GetLocation();

		UE_LOG(LogTemp, Warning, TEXT("Aim Socket Location : %s"), *SocketLocation.ToString());

		Camera->FieldOfView = 90.f / GET_MULT_NON_ZERO(CameraData.BaseData.ScopeZoom);
		Camera->SetRelativeLocation(SocketLocation);
	}
}

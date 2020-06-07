// Copyright Juan Marcelo Portillo. All Rights Reserved

#include "BattleRoyaleSettings.h"
#include "Engine/DataTable.h"

UBattleRoyaleSettings::UBattleRoyaleSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer),
	ItemUIClass(AItemUI::StaticClass())
{
}

bool UBattleRoyaleSettings::FindWeapon(const FString& WeaponName, FWeaponCoreData& outWeaponData)
{
	if (FWeaponCoreData* FoundWeapon = FindWeapon(WeaponName))
	{
		outWeaponData = *FoundWeapon;
		return true;
	}
	return false;
}

UClass* UBattleRoyaleSettings::GetItemUIClass()
{
	return ItemUIClass.Get();
}

FWeaponCoreData* UBattleRoyaleSettings::FindWeapon(const FString& WeaponName)
{
	//make it static so as to only find it once (since WeaponDataTablePath is only loaded once)
	static UDataTable* WeaponDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass()
		, NULL, *WeaponDataTablePath));

	if (WeaponDataTable)
		return WeaponDataTable->FindRow<FWeaponCoreData>(*WeaponName
			, FString(TEXT("Exec Find Weapon: ")) + WeaponName);

	return nullptr;
}

bool UBattleRoyaleSettings::FindAttachment(const FString& AttachmentName
	, FWeaponAttachmentData& outAttachmentData)
{
	if (FWeaponAttachmentData* FoundAttachment = FindAttachment(AttachmentName))
	{
		outAttachmentData = *FoundAttachment;
		return true;
	}
	return false;
}

FWeaponAttachmentData* UBattleRoyaleSettings::FindAttachment(const FString& AttachmentName)
{
	//make it static so as to only find it once (since WeaponDataTablePath is only loaded once)
	static UDataTable* AttachmentDataTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass()
		, NULL, *AttachmentDataTablePath));

	if (AttachmentDataTable)
		return AttachmentDataTable->FindRow<FWeaponAttachmentData>(*AttachmentName
			, FString(TEXT("Exec Find Attachment: ")) + AttachmentName);

	return nullptr;
}

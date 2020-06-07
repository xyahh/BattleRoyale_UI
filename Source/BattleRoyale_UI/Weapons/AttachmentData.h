// Copyright Juan Marcelo Portillo. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "WeaponCustomData.h"
#include "Engine/DataTable.h"
#include "AttachmentData.generated.h"

//Data Struct containing Data related to Weapon Attachments
USTRUCT(BlueprintType)
struct FWeaponAttachmentData : public FTableRowBase
{
    GENERATED_BODY()

    /*
    Weapon Compatible Check Order
    1) Check for Compatible Weapon Types
    2) From this Compatible Weapon Types, remove all those that are found "Incompatible" from list
    3) Add the Compatible list of Weapons and make sure there are no duplicates.
    */

    FWeaponAttachmentData()
        : WeaponSlot(EWeaponSlot::WS_None)
        , AttachmentStats(FWeaponCustomizableData::ZeroData)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Types")
    FString AttachmentName;

    //The Slot that this Attachment should go to
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Types")
    EWeaponSlot WeaponSlot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment Properties")
    class UStaticMesh* AttachmentMesh;

    /*
    The properties that make this Attachment affect the Weapon overall Stats.
    Some Properties like "Magazine Count", their values will be added
    Some Properties like Bullet Speed or recoil, their values will be multiplied if NOT zero
    Some Properties like "Scope Zoom" their values will just be set (if not zero)
   */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attachment Properties")
    FWeaponCustomizableData AttachmentStats;

    //Weapon Types that are allowed to see this Attachment (e.g. SRs for Scopes)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Types")
    TSet<EWeaponType> CompatibleWeaponTypes;

    /* Weapons within the WeaponTypes that are not Compatible. 
     * E.g. Most ARs have Grip slot, except AK-47. AK47 would go here.
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Types")
    TSet<FString> IncompatibleWeapons; 

    /* Weapons outside the WeaponTypes that are Compatible.
     * E.g. Guns dont a M16 stock slot, so CompatibleWeaponTypes would be empty
     * But this list will have "M16" and "M4" as possible guns that do equip this slot.
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Types")
    TSet<FString> CompatibleWeapons;


};

// Copyright Juan Marcelo Portillo. All Rights Reserved
#pragma once

#include "CoreMinimal.h"
#include "WeaponCustomData.h"
#include "Styling/SlateBrush.h"
#include "WeaponData.generated.h"

//Data struct containing Data related to the Weapons themselves
USTRUCT(BlueprintType)
struct FWeaponCoreData : public FTableRowBase
{
    GENERATED_BODY()

public:

    FWeaponCoreData()
        : WeaponName("None")
        , WeaponType(EWeaponType::WT_NoWeapon)
        , SlotGroup(EWeaponGroup::WG_None)
    {
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Core Info")
    FString WeaponName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Core Info")
    EWeaponType WeaponType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Core Info")
    EWeaponGroup SlotGroup;

    //Compatible Slots for Part Attachment
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Core Info")
    TMap<EWeaponSlot, FName> AttachmentSlots; 

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Core Info", meta = (DisplayThumbnail = "true", AllowedClasses = "Texture,MaterialInterface,SlateTextureAtlasInterface", DisallowedClasses = "MediaTexture"))
    class UObject* WeaponImage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Core Info")
    class USkeletalMesh* WeaponMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Core Info")
    FWeaponCustomizableData BaseData; //Raw Base Data without Attachments
};

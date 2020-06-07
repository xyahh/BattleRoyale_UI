// Copyright Juan Marcelo Portillo. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Weapons/WeaponData.h"
#include "Weapons/AttachmentData.h"
#include "Weapons/ItemUI.h"
#include "BattleRoyaleSettings.generated.h"

/**
 * 
 */
UCLASS(config = EditorUserSettings, DefaultConfig)
class BATTLEROYALE_UI_API UBattleRoyaleSettings : public UObject
{
    GENERATED_BODY()

public:

    UBattleRoyaleSettings(const FObjectInitializer& ObjectInitializer);

    /*
     * Finds the Weapon Core Data from the WeaponDataTable. 
     * Returns true if found, false if not found or if WeaponDataTable is Nullptr
     * outWeaponData for the found Weapon
    */
    UFUNCTION(BlueprintCallable, Category = "Weapons")
    bool FindWeapon(const FString& WeaponName, FWeaponCoreData& outWeaponData);

    UClass* GetItemUIClass();

    //Handles the actual task of finding the weapon. (the Func above is just a wrapper UFUNCTION)
    FWeaponCoreData* FindWeapon(const FString& WeaponName);


    /*
    * Finds the Attachment Data from the AttachmentDataTable.
    * Returns true if found, false if not found or if WeaponDataTable is Nullptr
    * outWeaponData for the found Weapon
   */
    UFUNCTION(BlueprintCallable, Category = "Attachments")
    bool FindAttachment(const FString& AttachmentName, FWeaponAttachmentData& outAttachmentData);

    FWeaponAttachmentData* FindAttachment(const FString& AttachmentName);

protected:

    UPROPERTY(Config, EditAnywhere, Category = "Items")
    TSubclassOf<AItemUI> ItemUIClass;

    UPROPERTY(Config, EditAnywhere, Category = "Weapons")
    FString WeaponDataTablePath;

    UPROPERTY(Config, EditAnywhere, Category = "Attachments")
    FString AttachmentDataTablePath;

    // Slots where the are going to be Stored in Character 

};

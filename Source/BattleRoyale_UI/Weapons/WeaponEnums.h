// Copyright Juan Marcelo Portillo. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "WeaponEnums.generated.h"

UENUM(BlueprintType)
enum class EWeaponGroup : uint8
{
    WG_None         UMETA(DisplayName = "None"),
    WG_Primary      UMETA(DisplayName = "Primary"),
    WG_Secondary    UMETA(DisplayName = "Secondary"),
};


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    WT_NoWeapon         UMETA(DisplayName = "No Weapon"),
    WT_AssaultRifle     UMETA(DisplayName = "Assault Rifle"),
    WT_SniperRifle      UMETA(DisplayName = "Sniper Rifle"),
    WT_MarksmanRifle    UMETA(DisplayName = "Marksman Rifle"),
    WT_SubmachineGun    UMETA(DisplayName = "Submachine Gun"),
    WT_Launcher         UMETA(DisplayName = "Launcher"),
    WT_Pistol           UMETA(DisplayName = "Pistol"),
};


UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
    WS_None         UMETA(DisplayName = "None"),
    WS_Barrel       UMETA(DisplayName = "Barrel"),
    WS_Grip         UMETA(DisplayName = "Grip"),
    WS_Scope        UMETA(DisplayName = "Scope"),
    WS_Magazine     UMETA(DisplayName = "Magazine"),
    WS_Stock        UMETA(DisplayName = "Stock"),
};


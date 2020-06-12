// Copyright Juan Marcelo Portillo. All Rights Reserved

#pragma once
#include "CoreMinimal.h"
#include "WeaponEnums.h"
#include "Engine/DataTable.h"
#include "WeaponCustomData.generated.h"

#define GET_MULT_NON_ZERO(Var) ((Var != 0.f) ? Var : 1.f)

//Data Struct containing Elements that can Change the Weapon via Attachments or other Sort.
USTRUCT(BlueprintType)
struct FWeaponCustomizableData : public FTableRowBase
{
    GENERATED_BODY()

    FWeaponCustomizableData()
        : MagazineCapacity(30)
        , RateOfFire(10.f)
        , BulletsPerShotCycle(1)
        , BurstRounds(3)
        , BulletSpeed(2500.f)
        , VerticalRecoil(0.f)
        , HorizontalRecoil(0.f)
        , ScopeZoom(1.f)
    {

    }

    FWeaponCustomizableData(int32 MagazineCapacity
        , float RateOfFire, int32 BulletsPerShotCycle
        , int32 BurstRounds, float BulletSpeed
        , float VerticalRecoil, float HorizontalRecoil, float ScopeZoom) 
        : MagazineCapacity(MagazineCapacity)
        , RateOfFire(RateOfFire), BulletsPerShotCycle(BulletsPerShotCycle)
        , BurstRounds(BurstRounds), BulletSpeed(BulletSpeed)
        , VerticalRecoil(VerticalRecoil), HorizontalRecoil(HorizontalRecoil)
        , ScopeZoom(ScopeZoom)
    {

    }

    static FWeaponCustomizableData ZeroData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Firing Settings")
    int32 MagazineCapacity; //Bullet Count

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Firing Settings")
    float RateOfFire; // Shot Cycles per Second.

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Firing Settings")
    int32 BulletsPerShotCycle; //Bullets Fired for every Shot Cycle

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Firing Settings")
    int32 BurstRounds; //How many ShotCycles are used in Burst Mode for every Shot

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Firing Settings")
    float BulletSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Firing Settings")
    float VerticalRecoil;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Firing Settings")
    float HorizontalRecoil;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Firing Settings")
    float ScopeZoom;

public:
    FWeaponCustomizableData& operator+=(const FWeaponCustomizableData& Other)
    {
        MagazineCapacity    += Other.MagazineCapacity;
        RateOfFire          *= GET_MULT_NON_ZERO(Other.RateOfFire);
        BulletsPerShotCycle += Other.BulletsPerShotCycle;
        BurstRounds         += Other.BurstRounds;
        BulletSpeed         *= GET_MULT_NON_ZERO(Other.BulletSpeed);
        VerticalRecoil      *= GET_MULT_NON_ZERO(Other.VerticalRecoil);
        ScopeZoom           *= GET_MULT_NON_ZERO(Other.ScopeZoom);
        return *this;
    }

    FWeaponCustomizableData& operator-=(const FWeaponCustomizableData& Other)
    {
        MagazineCapacity    -= Other.MagazineCapacity;
        RateOfFire          /= GET_MULT_NON_ZERO(Other.RateOfFire);
        BulletsPerShotCycle -= Other.BulletsPerShotCycle;
        BurstRounds         -= Other.BurstRounds;
        BulletSpeed         /= GET_MULT_NON_ZERO(Other.BulletSpeed);
        VerticalRecoil      /= GET_MULT_NON_ZERO(Other.VerticalRecoil);
        ScopeZoom           /= GET_MULT_NON_ZERO(Other.ScopeZoom);
        return *this;
    }

};



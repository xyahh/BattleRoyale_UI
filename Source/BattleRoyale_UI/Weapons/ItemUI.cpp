// Copyright Juan Marcelo Portillo. All Rights Reserved

#include "ItemUI.h"
#include "BaseWeapon.h"
#include "Components/WidgetComponent.h"

// Sets default values
AItemUI::AItemUI()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	ItemWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WeaponWidgetComponent"));
	ItemWidgetComponent->SetupAttachment(SceneComponent);
	ItemWidgetComponent->SetDrawSize(FVector2D(200.f, 100.f));
}

// Called when the game starts or when spawned
#include "Kismet/GameplayStatics.h"
void AItemUI::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	if (IsValid(ReferenceItem))
		ReferenceItem->ItemTraceStateDelegate.AddDynamic(this, &AItemUI::ItemTraced);

	if (IsValid(ItemWidgetComponent))
		ItemWidgetComponent->SetVisibility(false);
}

// Called every frame
#include "Kismet/KismetMathLibrary.h"
void AItemUI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(ReferenceItem) && IsValid(PlayerPawn))
	{
		FVector ItemLocation = ReferenceItem->GetActorLocation();
		SetActorLocation(ItemLocation);
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(ItemLocation
			, PlayerPawn->GetActorLocation()));	
	}
}

void AItemUI::ItemTraced(AActor* InstigatorActor, bool bIsWeaponTraced)
{
	if (IsValid(ItemWidgetComponent))
		ItemWidgetComponent->SetVisibility(bIsWeaponTraced);
}


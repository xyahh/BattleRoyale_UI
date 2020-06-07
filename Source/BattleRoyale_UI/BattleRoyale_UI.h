// Copyright Juan Marcelo Portillo. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyaleSettings.h"

#if WITH_EDITOR
	#include "ISettingsModule.h"
	#include "ISettingsSection.h"
	#include "ISettingsContainer.h"
#endif

class FBattleRoyaleSettingsModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual bool SupportsDynamicReloading() override;

private:

	bool HandleSettingsSaved();
	void RegisterSettings();
	void UnregisterSettings();

};

// Copyright Juan Marcelo Portillo. All Rights Reserved

#include "BattleRoyale_UI.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FBattleRoyaleSettingsModule"

//IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, BattleRoyale_UI, "BattleRoyale_UI" );
IMPLEMENT_PRIMARY_GAME_MODULE(FBattleRoyaleSettingsModule, BattleRoyale_UI, "BattleRoyale_UI");

void FBattleRoyaleSettingsModule::StartupModule()
{
	RegisterSettings();
}

void FBattleRoyaleSettingsModule::ShutdownModule()
{
	UnregisterSettings();
}

bool FBattleRoyaleSettingsModule::SupportsDynamicReloading()
{
	return true;
}

bool FBattleRoyaleSettingsModule::HandleSettingsSaved()
{
	UBattleRoyaleSettings* Settings = GetMutableDefault<UBattleRoyaleSettings>();
	bool ResaveSettings = false;
	if (ResaveSettings && Settings)
		Settings->SaveConfig();
	return true;
}

void FBattleRoyaleSettingsModule::RegisterSettings()
{
#if WITH_EDITOR
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr <ISettingsModule>("Settings"))
	{
		//add a category for the set
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings(
			"Project"
			, "Battle Royale"
				, "General",
			LOCTEXT("RuntimeGeneralSettingsName", "General"),
			LOCTEXT("RuntimeGeneralSettingsDescription", "Battle Royale Config"),
			GetMutableDefault <UBattleRoyaleSettings>());

		if (SettingsSection.IsValid())
		{
			//Register the auto-save processing at the time of update
			SettingsSection->OnModified().BindRaw(this, &FBattleRoyaleSettingsModule::HandleSettingsSaved);
		}
	}
#endif
}

void FBattleRoyaleSettingsModule::UnregisterSettings()
{
#if WITH_EDITOR
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr <ISettingsModule>("Settings"))
	{
			SettingsModule->UnregisterSettings(
				"Project"
				, "Battle Royale"
					, "General");
	}
#endif
}


#undef LOCTEXT_NAMESPACE
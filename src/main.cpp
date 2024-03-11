#include <Geode/Geode.hpp>
#include "Hooks.h"

using namespace geode::prelude;


$on_mod(Loaded) {
#ifdef GEODE_IS_WINDOWS
	//matdash::create_console();
#endif
	BetterPauseManager::sharedState()->loadState();
	Mod::get()->addCustomSetting<SettingQuickSettingsValue>("Quick-Settings-Select", "none");
	//std::cout << "0x" << std::hex << offsetof(AttemptAtReversingDialogObject, m_content) << std::endl;
}

$on_mod(DataSaved) {
	BetterPauseManager::sharedState()->saveState();
}




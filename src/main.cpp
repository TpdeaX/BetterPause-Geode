#include <Geode/Geode.hpp>
#include <Geode/loader/SettingV3.hpp>
#include "Hooks.h"
#include "SelectQuickSettings.h"

using namespace geode::prelude;

$on_mod(Loaded) {
	BetterPauseManager::sharedState()->loadState();
}

$execute {
	(void)ButtonSettingPressedEventV3(Mod::get(), "Quick-Settings-Select").listen([](std::string_view) {
		SelectQuickSettings::create(false)->show();
	}).leak();
}

$on_mod(DataSaved) {
	BetterPauseManager::sharedState()->saveState();
}

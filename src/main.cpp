#include <Geode/Geode.hpp>
#include <Geode/loader/SettingV3.hpp>
#include "Hooks.h"
#include "SelectQuickSettings.h"

using namespace geode::prelude;

$on_mod(Loaded) {
	BetterPauseManager::sharedState()->loadState();
}

$execute {
	new EventListener<EventFilter<ButtonSettingPressedEventV3>>(
		+[](ButtonSettingPressedEventV3* event) {
			SelectQuickSettings::create()->show();
			return ListenerResult::Propagate;
		},
		ButtonSettingPressedEventV3(Mod::get(), "Quick-Settings-Select")
	);
}

$on_mod(DataSaved) {
	BetterPauseManager::sharedState()->saveState();
}

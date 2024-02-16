#include "BetterPauseManager.h"

void BetterPauseManager::loadState() {
	if (Mod::get()->getSavedValue<std::string>("quick-Settings-Select-user").c_str() != nullptr) {
		auto vectorSaved = Utils::stringToVector<int>(Mod::get()->getSavedValue<std::string>("quick-Settings-Select-user"));
		if (!vectorSaved.empty()) {
			BetterPauseManager::posQuickA.clear();
			for (int element : vectorSaved) {
				BetterPauseManager::posQuickA.push_back(element);
				std::cout << element << std::endl;
			}
		}
	}
	
}

void BetterPauseManager::saveState() {
	Mod::get()->setSavedValue<std::string>("quick-Settings-Select-user", Utils::vectorToString(BetterPauseManager::posQuickA));
	for (int element : BetterPauseManager::posQuickA) {
		std::cout << element << std::endl;
	}
}
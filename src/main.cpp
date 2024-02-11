#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelTools.hpp>
#include <Geode/modify/MoreOptionsLayer.hpp>
#include <Geode/modify/GameOptionsLayer.hpp>
#include <Geode/modify/GJOptionsLayer.hpp>
#include <Geode/modify/CustomSongWidget.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/modify/GameToolbox.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/modify/DailyLevelPage.hpp>
#include "BetterPauseManager.h"
#include "BetterPause.hpp"

using namespace geode::prelude;

namespace matdash {
	struct Console {
		std::ofstream out, in;
		Console() {
			AllocConsole();
			out = decltype(out)("CONOUT$", std::ios::out);
			in = decltype(in)("CONIN$", std::ios::in);
			std::cout.rdbuf(out.rdbuf());
			std::cin.rdbuf(in.rdbuf());

			FILE* dummy;
			freopen_s(&dummy, "CONOUT$", "w", stdout);
		}
		~Console() {
			out.close();
			in.close();
		}
	};

	inline void create_console() {
		static Console console;
	}
}


class $modify(PauseLayer) {
	static PauseLayer* create(bool isEditor) {

		

		auto ret = PauseLayer::create(isEditor);

		for (size_t i = 1; i < ret->getChildrenCount(); i++)
		{
			auto node = dynamic_cast<cocos2d::CCNode*>(ret->getChildren()->objectAtIndex(i));
			if (node)
			{
				node->setVisible(false);
			}
		}

		auto betterPauseMenu = BetterPause::create(ret);
		betterPauseMenu->setID("better-pause-node");
		ret->addChild(betterPauseMenu, 100);

		

		return ret;
	}

	void onResume(cocos2d::CCObject* sender) {

		auto popuBetterPause = Utils::shareDirectorA()->getRunningScene()->getChildByID("popup-betterpause");

		while (popuBetterPause) {
			popuBetterPause->removeFromParentAndCleanup(true);
			popuBetterPause = Utils::shareDirectorA()->getRunningScene()->getChildByID("popup-betterpause");
		}

		PauseLayer::onResume(sender);
	}
};

std::string decodeBase64Gzip(const std::string& input) {
	return ZipUtils::decompressString(input, false, 0);
}

class $modify(PlayLayer) {
	bool init(GJGameLevel * p0, bool p1, bool p2) {
		BetterPause::m_timeTotalLevelBackup = 0.f;
		
		ProgressPlataformerBetter::m_totalPoints = 0;

		if (!PlayLayer::init(p0, p1, p2)) {
			return false;
		}

		//matdash::create_console();

		return true;
	}

	void addObject(GameObject* p0) {
		PlayLayer::addObject(p0);

		auto effectGameObjectPtr = dynamic_cast<EffectGameObject*>(p0);

		if (effectGameObjectPtr) {
			if (Utils::from<int>(effectGameObjectPtr, 0x31c) == 0x1e) {
				ProgressPlataformerBetter::m_totalPoints += Utils::from<int>(effectGameObjectPtr, 0x5f8);
				//std::cout << maximolkjnbv << std::endl;
			}
		}

	}

	void updateProgressbar() {
		PlayLayer::updateProgressbar();

		//const float MILLISECONDS_TO_SECONDS = 1000.f;
		//unsigned int currentPosition = 0;
		//FMODAudioEngine::sharedEngine()->m_globalChannel->getPosition(&currentPosition, FMOD_TIMEUNIT_MS);
		//float offset = this->m_levelSettings->m_songOffset * MILLISECONDS_TO_SECONDS;


		//FMODAudioEngine::sharedEngine()->getMusicTimeMS(0);
		//std::cout << currentPosition << std::endl;

		//	FMODAudioEngine::sharedEngine()->m_globalChannel->setPosition(static_cast<unsigned int>(10.f), FMOD_TIMEUNIT_MS);

		//std::cout << Utils::getPercentagePerTime() << std::endl;
	}

};

class $modify(GJBaseGameLayer) {
	//double getItemValue(int p1, int p2) {
	//	auto ret = GJBaseGameLayer::getItemValue(p1, p2);

		//std::cout << ret << std::endl;

	//	return ret;
	//}

	void update(float dt) {
		//std::cout << dt << std::endl;
		GJBaseGameLayer::update(dt);
		if (Utils::getplayLayerA()) {
			BetterPause::m_timeTotalLevelBackup += dt;
		}
	}
};

class $modify(MoreOptionsLayer) {
	void addToggle(const char* name, const char* key, const char* info) {
		MoreOptionsLayer::addToggle(name, key, info);

		//std::cout << name << "\t" << key << "\t" << info << std::endl;

		BetterPause::quickSettings_Name.push_back(name);
		BetterPause::quickSettings_Key.push_back(key);
		BetterPause::quickSettings_Desc.push_back(info);
		
	}
};

bool GameOptionsLayer_getSettings = false;

class $modify(GameOptionsLayer) {


	static GameOptionsLayer* create(GJBaseGameLayer * layer) {

		if (Utils::getplayLayerA() == layer) {
			GameOptionsLayer_getSettings = true;
		}

		auto ret = GameOptionsLayer::create(layer);

		GameOptionsLayer_getSettings = false;

		return ret;
	}

};

class $modify(GJOptionsLayer) {

	void addToggle(char const* p1, int p2 , bool p3 , char const* p4) {
		GJOptionsLayer::addToggle(p1, p2, p3, p4);

		BetterPause::quickSettings_NameG.resize(10);
		BetterPause::quickSettings_numberG.resize(10);
		BetterPause::quickSettings_enabledG.resize(10);

		BetterPause::quickSettings_NameG[p2 - 1] = p1;
		BetterPause::quickSettings_numberG[p2 - 1] = p2;
		BetterPause::quickSettings_enabledG[p2 - 1] = p3;

		

	}

};

class $modify(GameToolbox) {

	static gd::string pointsToString(int zzz) {
		auto ret = GameToolbox::pointsToString(zzz);
		//std::cout << zzz << std::endl;
		return ret;
	}

};

class $modify(DailyLevelPage) {

	bool init(GJTimedLevelType p0) {
		auto ret = DailyLevelPage::init(p0);
		return ret;
	}

};

class $modify(CustomSongWidget) {

	void updateWithMultiAssets(gd::string p0, gd::string p1, int p2) {
		CustomSongWidget::updateWithMultiAssets(p0, p1, p2);

		//std::cout << p0.c_str() << "\t" << p1.c_str() << "\t" << p2 << std::endl;

	}

	void updateSongObject(SongInfoObject* obj) {
		CustomSongWidget::updateSongObject(obj);
	}

};

$on_mod(Loaded) {
	//matdash::create_console();
	BetterPauseManager::sharedState()->loadState();
}

$on_mod(DataSaved) {
	BetterPauseManager::sharedState()->saveState();
}




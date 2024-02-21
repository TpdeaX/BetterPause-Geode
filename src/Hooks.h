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
#include <Geode/modify/DialogObject.hpp>
#include "BetterPauseManager.h"
#include "BetterPause.hpp"
#include "CustomSettings.hpp"
#include "SelectQuickSettings.h"

using namespace geode::prelude;

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

	void onResume(cocos2d::CCObject * sender) {

		auto popuBetterPause = Utils::shareDirectorA()->getRunningScene()->getChildByID("popup-betterpause");

		auto betterPause = dynamic_cast<BetterPause*>(this->getChildByID("better-pause-node"));

		if (betterPause) {
			if (betterPause->questMenu) {
				betterPause->questMenu->onClose(nullptr);
			}
		}


		while (popuBetterPause) {
			popuBetterPause->removeFromParentAndCleanup(true);
			popuBetterPause = Utils::shareDirectorA()->getRunningScene()->getChildByID("popup-betterpause");
		}

		PauseLayer::onResume(sender);
	}
};

class $modify(PlayLayer) {
	bool init(GJGameLevel * p0, bool p1, bool p2) {
		BetterPause::totalLevelTimeBackup = 0.f;
		ProgressPlataformerBetter::m_totalPoints = 0;

		if (!PlayLayer::init(p0, p1, p2)) {
			return false;
		}

		return true;
	}

	void addObject(GameObject * p0) {
		PlayLayer::addObject(p0);

		auto effectGameObjectPtr = dynamic_cast<EffectGameObject*>(p0);

		intptr_t offsetTypeObject = 0;
		intptr_t offsetPointsXObj = 0;

#ifdef GEODE_IS_WINDOWS
		offsetTypeObject = 0x31c;
		offsetPointsXObj = 0x5f8;
#endif
#ifdef GEODE_IS_ANDROID64
		offsetTypeObject = 0x388;
		offsetPointsXObj = 0x690;
#endif
#ifdef GEODE_IS_ANDROID32
		offsetTypeObject = 0x308;
		offsetPointsXObj = 0x5e4;
#endif
		if (effectGameObjectPtr) {
			if (Utils::from<int>(effectGameObjectPtr, offsetTypeObject) == 0x1e) {
				ProgressPlataformerBetter::m_totalPoints += Utils::from<int>(effectGameObjectPtr, offsetPointsXObj);
				//std::cout << maximolkjnbv << std::endl;
			}
		}

	}

};

class $modify(GJBaseGameLayer) {
	void update(float dt) {
		GJBaseGameLayer::update(dt);
		if (Utils::getplayLayerA()) {

#ifdef GEODE_IS_ANDROID
			/*
			auto label = dynamic_cast<CCLabelBMFont*>(this->getChildByID("epicooo"));
			if (!label) {
				label = cocos2d::CCLabelBMFont::create("zzz", "bigFont.fnt");
				label->setPosition({ Utils::WinSize().width / 2.f, Utils::WinSize().height / 2.f });
				label->setID("epicooo");
				this->addChild(label);
				label->setScale(0.4f);
			}

			
			std::stringstream ss;
			for (size_t i = 0; i < numIterations; i++) {
				if (i > 0) {
					ss << " | ";
				}
				ss << "Valor " << i + 1 << ": " << std::floor(Utils::from<int>(Utils::getplayLayerA(), offset + (i * sizeof(int)))) << std::endl;
			}
		

			std::stringstream ss;
			ss << "Intentos " << Utils::getplayLayerA()->getCurrentPercent() << std::endl;

			label->setString(ss.str().c_str());
			*/
#endif

			BetterPause::totalLevelTimeBackup += dt;
		}
	}
};

class $modify(MoreOptionsLayer) {
	void addToggle(const char* name, const char* key, const char* info) {
		MoreOptionsLayer::addToggle(name, key, info);

		BetterPause::quickSettingsNames.push_back(name);
		BetterPause::quickSettingsKeys.push_back(key);
		BetterPause::quickSettingsDescriptions.push_back(info);

	}
};


class $modify(GameOptionsLayer) {


	static GameOptionsLayer* create(GJBaseGameLayer * layer) {

		BetterPause::quickSettingsNamesG.clear();
		BetterPause::quickSettingsNumbersG.clear();
		BetterPause::quickSettingsEnabledG.clear();

		if (Utils::getplayLayerA()) {
			SelectQuickSettings::GameOptionsLayer_getSettings = true;
		}

		auto ret = GameOptionsLayer::create(layer);

		SelectQuickSettings::GameOptionsLayer_getSettings = false;

		return ret;
	}

};

class $modify(GJOptionsLayer) {

	void addToggle(char const* p1, int p2, bool p3, char const* p4) {
		GJOptionsLayer::addToggle(p1, p2, p3, p4);

		if (SelectQuickSettings::GameOptionsLayer_getSettings) {
			if (p2 > 0 && p2 <= 10) {
				BetterPause::quickSettingsNamesG.resize(99);
				BetterPause::quickSettingsNumbersG.resize(99);
				BetterPause::quickSettingsEnabledG.resize(99);

				BetterPause::quickSettingsNamesG[p2 - 1] = p1;
				BetterPause::quickSettingsNumbersG[p2 - 1] = p2;
				BetterPause::quickSettingsEnabledG[p2 - 1] = p3;
			}
		}
	}
};
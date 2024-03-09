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
#include <Geode/modify/InfoLayer.hpp>
#include "BetterPauseManager.h"
#include "BetterPause.hpp"
#include "CustomSettings.hpp"
#include "SelectQuickSettings.h"
#include <Geode/modify/CCScrollLayerExt.hpp>
#include "CoinsViewerSprites.hpp"

using namespace geode::prelude;


namespace hooksVariables {
	geode::Hook* GJOptionsLayerr_addToggleHookV = nullptr;
}

class $modify(PauseLayer) {
	static void onModify(auto & self) {
		self.setHookPriority("PauseLayer::create", -99);
	}

	static PauseLayer* create(bool isEditor) {
		auto ret = PauseLayer::create(isEditor);

		if (Mod::get()->getSettingValue<int64_t>("type-pause")) {
			for (size_t i = 1; i < ret->getChildrenCount(); i++)
			{
				auto node = dynamic_cast<cocos2d::CCNode*>(ret->getChildren()->objectAtIndex(i));
				if (node)
				{
					node->setVisible(false);
				}
			}

			auto betterPauseMenu = BetterPause::create(ret);
			ret->addChild(betterPauseMenu, 100);
		}

		return ret;
	}

	void onResume(cocos2d::CCObject * sender) {

		auto popuBetterPause = Utils::shareDirectorA()->getRunningScene()->getChildByID("popup-betterpause");

		auto betterPause = typeinfo_cast<BetterPause*>(this->getChildByID("better-pause-node"));

		if (betterPause) {
			if (betterPause->questMenu) {
				betterPause->questMenu->onClose(nullptr);
			}
			betterPause->unregisterScriptTouchHandler();
		}


		while (popuBetterPause) {
			typeinfo_cast<FLAlertLayer*>(popuBetterPause)->keyBackClicked();

			popuBetterPause = Utils::shareDirectorA()->getRunningScene()->getChildByID("popup-betterpause");
		}

		PauseLayer::onResume(sender);
	}
};

class $modify(PlayLayer) {
	bool init(GJGameLevel * p0, bool p1, bool p2) {
		BetterPause::totalLevelTimeBackup = 0.f;
		ProgressPlataformerBetter::m_totalPoints = 0;
		CoinsViewerSprites::coinsInVector.clear();

		if (!PlayLayer::init(p0, p1, p2)) {
			return false;
		}

		if (this->m_level->isPlatformer()) {
			ProgressPlataformerBetter::timeForLevelStringPlataformerSafe = BetterInfo::timeForLevelString(this->m_level->m_levelString);
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
			}
		}

		bool isCoin = p0->m_objectType == GameObjectType::SecretCoin || p0->m_objectType == GameObjectType::UserCoin;

		if (isCoin) {
			bool isObjectInVector = std::find(CoinsViewerSprites::coinsInVector.begin(), CoinsViewerSprites::coinsInVector.end(), p0) != CoinsViewerSprites::coinsInVector.end();
			if (!isObjectInVector) {
				p0->retain();
				CoinsViewerSprites::coinsInVector.push_back(p0);
				p0->release();
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

	bool init(int idk) {

		BetterPause::quickSettingsNamesG.clear();
		BetterPause::quickSettingsNumbersG.clear();
		BetterPause::quickSettingsEnabledG.clear();

		BetterPause::quickSettingsNamesG.resize(99);
		BetterPause::quickSettingsNumbersG.resize(99);
		BetterPause::quickSettingsEnabledG.resize(99);

		auto ret = GJOptionsLayer::init(idk);

		

		return ret;
	}

	TodoReturn addToggleInternal(char const* p1, int p2, bool p3, char const* p4) {
		GJOptionsLayer::addToggleInternal(p1, p2, p3, p4);
		if (SelectQuickSettings::GameOptionsLayer_getSettings) {
			if (p2 > 0 && p2 <= 10) {
				BetterPause::quickSettingsNamesG[p2 - 1] = p1;
				BetterPause::quickSettingsNumbersG[p2 - 1] = p2;
				BetterPause::quickSettingsEnabledG[p2 - 1] = p3;
			}
		}
	}

	

};

class $modify(CustomSongWidget) {
	void updateSongObject(SongInfoObject * obj) {
		CustomSongWidget::updateSongObject(obj);
		
		if (Utils::getplayLayerA() && this->m_artistLabel && this->m_moreBtn) {
			CCPoint labelPos = this->m_artistLabel->getPosition();
			auto labelWidth = this->m_artistLabel->getContentSize().width * this->m_artistLabel->getScale();

			auto menuItemX = labelPos.x + labelWidth - (Utils::WinSize().width / 2) + 30.f;

			this->m_moreBtn->setPositionX(menuItemX);
		}	
	}

	void updateSongInfo() {
		CustomSongWidget::updateSongInfo();

		if (Utils::getplayLayerA() && this->m_artistLabel && this->m_moreBtn) {
			CCPoint labelPos = this->m_artistLabel->getPosition();
			auto labelWidth = this->m_artistLabel->getContentSize().width * this->m_artistLabel->getScale();

			auto menuItemX = labelPos.x + labelWidth - (Utils::WinSize().width / 2) + 30.f;

			this->m_moreBtn->setPositionX(menuItemX);
		}
	}
};

class $modify(CCScrollLayerExt) {
	void ccTouchMoved(cocos2d::CCTouch * p0, cocos2d::CCEvent * p1) {
		CCScrollLayerExt::ccTouchMoved(p0, p1);
		if (Utils::hasParentWithID(this, "better-pause-node")) {

			auto betterPause = typeinfo_cast<BetterPause*>(Utils::getParentWithID(this, "better-pause-node"));
			betterPause->updateButtons();
			betterPause->setEnabledForButtons(false);
		}
	}

	void ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent) {
		CCScrollLayerExt::ccTouchEnded(pTouch, pEvent);
		if (Utils::hasParentWithID(this, "better-pause-node")) {
			auto betterPause = typeinfo_cast<BetterPause*>(Utils::getParentWithID(this, "better-pause-node"));
			betterPause->setEnabledForButtons(true);
			betterPause->updateButtons();
		}
	}

	void ccTouchCancelled(CCTouch* pTouch, CCEvent* pEvent) {
		CCScrollLayerExt::ccTouchCancelled(pTouch, pEvent);
		if (Utils::hasParentWithID(this, "better-pause-node")) {
			auto betterPause = typeinfo_cast<BetterPause*>(Utils::getParentWithID(this, "better-pause-node"));
			betterPause->setEnabledForButtons(true);
			betterPause->updateButtons();
		}
	}

	TodoReturn scrollLayer(float p0) {
		CCScrollLayerExt::scrollLayer(p0);
		if (Utils::hasParentWithID(this, "better-pause-node")) {
			typeinfo_cast<BetterPause*>(Utils::getParentWithID(this, "better-pause-node"))->updateButtons();
		}
	}
};

class $modify(InfoLayer) {

	void onClose(cocos2d::CCObject * sender) {
		InfoLayer::onClose(sender);
		if (auto pauseLayer = CCScene::get()->getChildByID("PauseLayer"); pauseLayer) {
			if (auto betterPause = typeinfo_cast<BetterPause*>(pauseLayer->getChildByID("better-pause-node")); betterPause) {
				handleTouchPriority(pauseLayer);
				handleTouchPriority(betterPause);
			}
		}
	}

};
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
#include <Geode/modify/GJGameLevel.hpp>
#include "BetterPauseManager.h"
#include "BetterPause.hpp"
#include "CustomSettings.hpp"
#include "SelectQuickSettings.h"
#include "DebugLogger.hpp"
#include <Geode/modify/CCScrollLayerExt.hpp>
#include "CoinsViewerSprites.hpp"
#include "PauseZoom.hpp"
#include "BetterPauseEnhancements.hpp"

using namespace geode::prelude;


namespace hooksVariables {
	geode::Hook* GJOptionsLayerr_addToggleHookV = nullptr;
}

class ButtonsClass : public FLAlertLayerProtocol {

};

class $modify(PauseLayer) {
	struct Fields {
		bool hasPosibleExitHotKey = false;
		bool hasConfirmPopup = false;
		bool isCountingDown = false;
		bool isResuming = false;
		bool isExecutingAction = false;
		~Fields() {
			CCNode* cd = nullptr;
			if (auto scene = CCDirector::sharedDirector()->getRunningScene()) {
				cd = scene->getChildByID("unpause-countdown-node");
			}
			if (cd) {
				cd->removeFromParent();
			}
			PauseMusicManager::get()->onResume();
			LastDeathGhostNode::removeFromPlayLayer(PlayLayer::get());
			PauseZoomManager::get()->onResume();
		}
	};

	static void onModify(auto & self) {
		self.setHookPriority("PauseLayer::customSetup", -99);
	}

	void cleanUpEnhancements() {
		PauseMusicManager::get()->onResume();
		LastDeathGhostNode::removeFromPlayLayer(PlayLayer::get());
		PauseZoomManager::get()->onResume();
	}

	void customSetup() {
		PauseLayer::customSetup();

		if (Mod::get()->getSettingValue<int64_t>("type-pause")) {
			for (size_t i = 1; i < this->getChildrenCount(); i++)
			{
				auto node = dynamic_cast<cocos2d::CCNode*>(this->getChildren()->objectAtIndex(i));
				if (node)
				{
					node->setVisible(false);
				}
			}

			auto betterPauseMenu = BetterPause::create(this);
			this->addChild(betterPauseMenu, 100);
		}

		if (Mod::get()->getSettingValue<bool>("enable-pause-animations")) {
			if (auto bgRgba = typeinfo_cast<CCRGBAProtocol*>(this->getChildByID("background"))) {
				bgRgba->setOpacity(0);
			}
			if (auto bgNode = this->getChildByID("background")) {
				bgNode->runAction(cocos2d::CCFadeTo::create(0.20f, 100));
			}
		}

		PauseMusicManager::get()->onPause();
		LastDeathGhostNode::showInPlayLayer(PlayLayer::get());
		PauseZoomManager::get()->onPause(this);
	}

	void onResume(cocos2d::CCObject * sender) {
		if (m_fields->isCountingDown) {
			return;
		}

		if (m_fields->isExecutingAction) {
			m_fields->isExecutingAction = false;
			m_fields->isResuming = true;
			PauseLayer::onResume(sender);
			return;
		}

		if (m_fields->isResuming) {
			return;
		}

		if (Mod::get()->getSettingValue<bool>("confirm-resume-level") && !m_fields->hasConfirmPopup) {
			geode::createQuickPopup(
				"Resume Level",
				std::string("Are you sure you want to <cr>resume</c>?"),
				"Cancel", "Resume",
				[this, sender](FLAlertLayer* fla, bool btn2) {
					if (btn2) {
						m_fields->hasConfirmPopup = true;
						this->onResume(sender);
						m_fields->hasConfirmPopup = false;
					}
				}
			);
			return;
		}

		auto getBP = [this]() -> BetterPause* {
			if (auto bp = typeinfo_cast<BetterPause*>(this->getChildByID("better-pause-node"))) return bp;
			if (auto bp = typeinfo_cast<BetterPause*>(this->getChildByID("simple-pause-node"))) return bp;
			if (auto bp = typeinfo_cast<BetterPause*>(this->getChildByID("classic-pause-node"))) return bp;
			return nullptr;
		};

		if (Mod::get()->getSettingValue<bool>("enable-unpause-countdown")) {
			this->startCountdown([this, sender]() {
				this->onResume(sender);
			});
			return;
		}

		m_fields->isResuming = true;

		auto popuBetterPause = Utils::shareDirectorA()->getRunningScene()->getChildByID("popup-betterpause");
		while (popuBetterPause) {
			typeinfo_cast<FLAlertLayer*>(popuBetterPause)->keyBackClicked();
			popuBetterPause = Utils::shareDirectorA()->getRunningScene()->getChildByID("popup-betterpause");
		}

		auto betterPause = getBP();
		if (betterPause) {
			betterPause->unregisterScriptTouchHandler();
		}

		this->cleanUpEnhancements();
		PauseLayer::onResume(sender);
	}

	BetterPause* getBP() {
		if (auto bp = typeinfo_cast<BetterPause*>(this->getChildByID("better-pause-node"))) return bp;
		if (auto bp = typeinfo_cast<BetterPause*>(this->getChildByID("simple-pause-node"))) return bp;
		if (auto bp = typeinfo_cast<BetterPause*>(this->getChildByID("classic-pause-node"))) return bp;
		return nullptr;
	}

	void cancelCountdown() {
		if (!m_fields->isCountingDown) return;
		m_fields->isCountingDown = false;
		m_fields->isExecutingAction = false;

		CCNode* cd = nullptr;
		if (auto scene = CCDirector::sharedDirector()->getRunningScene()) {
			cd = scene->getChildByID("unpause-countdown-node");
		}
		if (!cd) {
			cd = this->getChildByID("unpause-countdown-node");
		}

		if (cd) {
			if (auto countdown = typeinfo_cast<UnpauseCountdownNode*>(cd)) {
				countdown->cancel();
			} else {
				cd->removeFromParent();
			}
		}

		this->setVisible(true);

		if (auto bp = getBP()) {
			bp->setVisible(!bp->isHidden);
		}
	}

	void startCountdown(std::function<void()> onComplete) {
		m_fields->isCountingDown = true;
		m_fields->isExecutingAction = false;

		auto popuBetterPause = Utils::shareDirectorA()->getRunningScene()->getChildByID("popup-betterpause");
		while (popuBetterPause) {
			typeinfo_cast<FLAlertLayer*>(popuBetterPause)->keyBackClicked();
			popuBetterPause = Utils::shareDirectorA()->getRunningScene()->getChildByID("popup-betterpause");
		}

		this->setVisible(false);

		auto cd = UnpauseCountdownNode::create([this, onComplete]() {
			m_fields->isCountingDown = false;
			m_fields->isExecutingAction = true;

			auto betterPause = getBP();
			if (betterPause) {
				betterPause->unregisterScriptTouchHandler();
			}

			this->cleanUpEnhancements();
			if (onComplete) {
				onComplete();
			}
		});
		cd->setID("unpause-countdown-node");
		if (auto scene = CCDirector::sharedDirector()->getRunningScene()) {
			scene->addChild(cd, 99999);
		} else {
			this->addChild(cd, 99999);
		}
	}

	void onPracticeMode(cocos2d::CCObject* sender) {
		if (m_fields->isCountingDown) {
			return;
		}

		if (m_fields->isExecutingAction) {
			m_fields->isExecutingAction = false;
			PauseLayer::onPracticeMode(sender);
			return;
		}

		if (m_fields->isResuming) {
			return;
		}

		if (Mod::get()->getSettingValue<bool>("confirm-practice-level") && !m_fields->hasConfirmPopup) {
			auto popupConfirm = geode::createQuickPopup(
				"Practice Level",
				std::string("Do you want to <cg>Enter</c> the practice mode?"),
				"Cancel",
				"Enter",
				[this, sender](FLAlertLayer* fla, bool btn2) {
					if (btn2) {
						m_fields->hasConfirmPopup = true;
						this->onPracticeMode(sender);
						m_fields->hasConfirmPopup = false;
					}
				}
			);

			popupConfirm->m_button2->updateBGImage("GJ_button_02.png");
			return;
		}

		if (Mod::get()->getSettingValue<bool>("enable-unpause-countdown") && Mod::get()->getSettingValue<bool>("countdown-practice-mode")) {
			this->startCountdown([this, sender]() {
				this->onPracticeMode(sender);
			});
			return;
		}

		this->cleanUpEnhancements();
		PauseLayer::onPracticeMode(sender);
	}

	void onNormalMode(cocos2d::CCObject* sender) {
		if (m_fields->isCountingDown) {
			return;
		}

		if (m_fields->isExecutingAction) {
			m_fields->isExecutingAction = false;
			PauseLayer::onNormalMode(sender);
			return;
		}

		if (m_fields->isResuming) {
			return;
		}

		if (Mod::get()->getSettingValue<bool>("confirm-normal-level") && !m_fields->hasConfirmPopup) {
			auto popupConfirm = geode::createQuickPopup(
				"Normal Level",
				std::string("Do you want to <cr>Exit</c> the practice mode?"),
				"Cancel",
				"Exit",
				[this, sender](FLAlertLayer* fla, bool btn2) {
					if (btn2) {
						m_fields->hasConfirmPopup = true;
						this->onNormalMode(sender);
						m_fields->hasConfirmPopup = false;
					}
				}
			);

			popupConfirm->m_button2->updateBGImage("GJ_button_06.png");
			return;
		}

		if (Mod::get()->getSettingValue<bool>("enable-unpause-countdown") && Mod::get()->getSettingValue<bool>("countdown-normal-mode")) {
			this->startCountdown([this, sender]() {
				this->onNormalMode(sender);
			});
			return;
		}

		this->cleanUpEnhancements();
		PauseLayer::onNormalMode(sender);
	}

	void onRestart(cocos2d::CCObject* sender)
	{
		if (m_fields->isCountingDown) {
			return;
		}

		if (m_fields->isExecutingAction) {
			m_fields->isExecutingAction = false;
			PauseLayer::onRestart(sender);
			return;
		}

		if (m_fields->isResuming) {
			return;
		}

		if (Mod::get()->getSettingValue<bool>("confirm-restart-level") && !m_fields->hasConfirmPopup) {
			geode::createQuickPopup(
				"Restart Level",
				"Are you sure you want to <cr>restart</c>?",
				"Cancel", "Restart",
				[this, sender](FLAlertLayer* fla, bool btn2) {

					if (btn2) {
						m_fields->hasConfirmPopup = true;
						this->onRestart(sender);
						m_fields->hasConfirmPopup = false;
					}
				}
			);

			return;
		}

		if (Mod::get()->getSettingValue<bool>("enable-unpause-countdown") && Mod::get()->getSettingValue<bool>("countdown-restart-level")) {
			this->startCountdown([this, sender]() {
				this->onRestart(sender);
			});
			return;
		}

		this->cleanUpEnhancements();
		PauseLayer::onRestart(sender);
	}

	void onRestartFull(cocos2d::CCObject* sender)
	{
		if (m_fields->isCountingDown) {
			return;
		}

		if (m_fields->isExecutingAction) {
			m_fields->isExecutingAction = false;
			PauseLayer::onRestartFull(sender);
			return;
		}

		if (m_fields->isResuming) {
			return;
		}

		if (Mod::get()->getSettingValue<bool>("confirm-restart-full-level") && !m_fields->hasConfirmPopup) {
			geode::createQuickPopup(
				"Restart Full Level",
				"Are you sure you want to <cr>restart</c>?",
				"Cancel", "Restart",
				[this, sender](FLAlertLayer* fla, bool btn2) {

					if (btn2) {
						m_fields->hasConfirmPopup = true;
						this->onRestartFull(sender);
						m_fields->hasConfirmPopup = false;
					}
				}
			);

			return;
		}

		if (Mod::get()->getSettingValue<bool>("enable-unpause-countdown") && Mod::get()->getSettingValue<bool>("countdown-restart-level")) {
			this->startCountdown([this, sender]() {
				this->onRestartFull(sender);
			});
			return;
		}

		this->cleanUpEnhancements();
		PauseLayer::onRestartFull(sender);
	}

	void onEdit(cocos2d::CCObject* sender)
	{
		if (m_fields->isCountingDown || m_fields->isResuming) {
			return;
		}

		if (Mod::get()->getSettingValue<bool>("confirm-edit-level") && !m_fields->hasConfirmPopup) {
			geode::createQuickPopup(
				"Edit Level",
				"Are you sure you want to <cr>edit</c>?",
				"Cancel", "Edit",
				[this, sender](FLAlertLayer* fla, bool btn2) {

					if (btn2) {
						m_fields->hasConfirmPopup = true;
						this->cleanUpEnhancements();
						PauseLayer::onEdit(sender);
						m_fields->hasConfirmPopup = false;
					}
				}
			);

			return;
		}

		this->cleanUpEnhancements();
		PauseLayer::onEdit(sender);
	}

	void keyDown(cocos2d::enumKeyCodes p0, double p1) {
		if (m_fields->isCountingDown) {
			if (p0 == cocos2d::enumKeyCodes::KEY_Escape) {
				this->cancelCountdown();
			}
			return;
		}
		if (m_fields->isResuming) {
			return;
		}

		if (Mod::get()->getSettingValue<bool>("remove-exit-hotkey")) {
			m_fields->hasPosibleExitHotKey = true;
		}

		PauseLayer::keyDown(p0, p1);

		m_fields->hasPosibleExitHotKey = false;
	}

	void keyBackClicked() {
		if (m_fields->isCountingDown) {
			this->cancelCountdown();
			return;
		}
		if (m_fields->isResuming) {
			return;
		}
		PauseLayer::keyBackClicked();
	}

#ifdef GEODE_IS_MACOS
	void onQuit(cocos2d::CCObject* sender) {
		if (m_fields->isCountingDown || m_fields->isResuming || m_fields->hasPosibleExitHotKey) {
			return;
		}

		this->cleanUpEnhancements();
		PauseLayer::onQuit(sender);
	}

#else
	void tryQuit(cocos2d::CCObject* sender) {
		if (m_fields->isCountingDown || m_fields->isResuming || m_fields->hasPosibleExitHotKey) {
			return;
		}

		this->cleanUpEnhancements();
		PauseLayer::tryQuit(sender);
	}

#endif
};

class $modify(PlayLayer) {
	bool init(GJGameLevel * p0, bool p1, bool p2) {
		PauseZoomManager::get()->onResume();
		PauseMusicManager::get()->onResume();
		LastDeathGhostNode::removeFromPlayLayer(this);
		SessionStatsManager::get()->onLevelInit(p0);
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

	void destroyPlayer(PlayerObject* p0, GameObject* p1) {
		PlayLayer::destroyPlayer(p0, p1);
		if (p0 == this->m_player1 && !this->m_isPracticeMode) {
			SessionStatsManager::get()->onPlayerDeath(this->getCurrentPercent(), p0->getPosition());
		}
	}

	void addObject(GameObject * p0) {
		PlayLayer::addObject(p0);

		GameObjectType objTypeV = p0->m_objectType;


		bool isCoin = objTypeV == GameObjectType::SecretCoin || objTypeV == GameObjectType::UserCoin;

		if (isCoin) {
			bool isObjectInVector = std::find(CoinsViewerSprites::coinsInVector.begin(), CoinsViewerSprites::coinsInVector.end(), p0) != CoinsViewerSprites::coinsInVector.end();
			if (!isObjectInVector) {
				p0->retain();
				CoinsViewerSprites::coinsInVector.push_back(p0);
				p0->release();
			}
		}

	}

	void showNewBest(bool p0, int p1, int p2, bool p3, bool p4, bool p5) {

		if (this->m_isTestMode && this->m_level->m_levelType != GJLevelType::Editor) {
			return;
		}

		PlayLayer::showNewBest(p0, p1, p2, p3, p4, p5);
	}

	void levelComplete()
	{
		if (this->m_isTestMode && this->m_level->m_levelType != GJLevelType::Editor)
		{
			if (m_isPracticeMode)
			{
				this->showCompleteText();
			}
			else
			{
				showCompleteEffect();
			}

			return;
		}

		PlayLayer::levelComplete();
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
	bool init() {
		BetterPauseDebug::log("MoreOptionsLayer::init() called");
		bool ret = MoreOptionsLayer::init();
		BetterPauseDebug::log(std::string("MoreOptionsLayer::init() returned ") + (ret ? "true" : "false"));
		return ret;
	}

	void addToggle(const char* name, const char* key, const char* info) {
		MoreOptionsLayer::addToggle(name, key, info);

		BetterPause::quickSettingsNames.push_back(name ? name : "");
		BetterPause::quickSettingsKeys.push_back(key ? key : "");
		BetterPause::quickSettingsDescriptions.push_back(info ? info : "");

		std::string sName = name ? name : "NULL";
		std::string sKey = key ? key : "NULL";
		std::string sInfo = info ? info : "NULL";
		BetterPauseDebug::log("MoreOptionsLayer::addToggle -> Name: '" + sName + "' | Key: '" + sKey + "' | Info: '" + sInfo + "' | Total stored: " + std::to_string(BetterPause::quickSettingsNames.size()));
	}
};


class $modify(GameOptionsLayer) {
	void setupOptions() {
		BetterPause::quickSettingsNamesG.clear();
		BetterPause::quickSettingsNumbersG.clear();
		BetterPause::quickSettingsEnabledG.clear();

		if (Utils::getplayLayerA()) {
			SelectQuickSettings::GameOptionsLayer_getSettings = true;
		}

		GameOptionsLayer::setupOptions();

		SelectQuickSettings::GameOptionsLayer_getSettings = false;
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

	void addToggleInternal(char const* p1, int p2, bool p3, char const* p4) {
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
#ifndef GEODE_IS_WINDOWS
	void updateSongObject(SongInfoObject * obj) {
		CustomSongWidget::updateSongObject(obj);

		if (Utils::getplayLayerA() && this->m_artistLabel && this->m_moreBtn) {
			CCPoint labelPos = this->m_artistLabel->getPosition();
			auto labelWidth = this->m_artistLabel->getContentSize().width * this->m_artistLabel->getScale();

			auto menuItemX = labelPos.x + labelWidth - (Utils::WinSize().width / 2) + 30.f;

			this->m_moreBtn->setPositionX(menuItemX);
		}
	}
#endif

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

	void ccTouchEnded(CCTouch * pTouch, CCEvent * pEvent) {
		CCScrollLayerExt::ccTouchEnded(pTouch, pEvent);
		if (Utils::hasParentWithID(this, "better-pause-node")) {
			auto betterPause = typeinfo_cast<BetterPause*>(Utils::getParentWithID(this, "better-pause-node"));
			betterPause->setEnabledForButtons(true);
			betterPause->updateButtons();
		}
	}

	void ccTouchCancelled(CCTouch * pTouch, CCEvent * pEvent) {
		CCScrollLayerExt::ccTouchCancelled(pTouch, pEvent);
		if (Utils::hasParentWithID(this, "better-pause-node")) {
			auto betterPause = typeinfo_cast<BetterPause*>(Utils::getParentWithID(this, "better-pause-node"));
			betterPause->setEnabledForButtons(true);
			betterPause->updateButtons();
		}
	}

#ifndef GEODE_IS_WINDOWS
	void scrollLayer(float p0) {
		CCScrollLayerExt::scrollLayer(p0);
		if (Utils::hasParentWithID(this, "better-pause-node")) {
			typeinfo_cast<BetterPause*>(Utils::getParentWithID(this, "better-pause-node"))->updateButtons();
		}
	}
#endif
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

class $modify(GJGameLevel) {
	void savePercentage(int p0, bool p1, int p2, int p3, bool p4) {

		if (PlayLayer::get()) {
			if (PlayLayer::get()->m_isTestMode && this->m_levelType != GJLevelType::Editor) {
				return;
			}
		}

		GJGameLevel::savePercentage(p0, p1, p2, p3, p4);
	}
};
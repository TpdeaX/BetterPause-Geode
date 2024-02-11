#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include "BarBetterShow.hpp"
#include "Utils.hpp"
#include "ListButtons.h"
#include "BetterPauseManager.h"
#include "ProgressPlataformerBetter.hpp"
#include "SetVolumenPopup.hpp"
#include "MoreOptionsPauseLayer.hpp"

using namespace geode::prelude;

class BetterPause : public cocos2d::CCLayer
{
public:
	static cocos2d::CCMenu* pauseLayer_MenuToggles;
	static cocos2d::CCPoint pauseLayer_lastPoint;
	static std::vector<std::string> quickSettings_Name;
	static std::vector<std::string> quickSettings_Desc;
	static std::vector<std::string> quickSettings_Key;
	static std::vector<std::string> quickSettings_NameG;
	static std::vector<int> quickSettings_numberG;
	static std::vector<bool> quickSettings_enabledG;
	static float m_timeTotalLevelBackup;

	cocos2d::CCMenu* m_pMenuButtons = nullptr;
	cocos2d::CCMenu* m_pMenuButtons2 = nullptr;
	PauseLayer* pauseLayer = nullptr;
	CCMenuItemSpriteExtra* m_pVisibleButton = nullptr;
	bool m_pIsHide = false;
	Slider* m_pSliderMusic = nullptr;
	Slider* m_pSliderSFX = nullptr;
	ChallengesPage* m_pQuestMenu = nullptr;
	cocos2d::CCLabelBMFont* m_pLabelMusicVPercentage = nullptr;
	cocos2d::CCLabelBMFont* m_pLabelSFXVPercentage = nullptr;
	cocos2d::CCLabelBMFont* m_pNameLevelLabel = nullptr;
	cocos2d::CCLabelBMFont* m_pTypeLevelLabel = nullptr;
	cocos2d::CCLabelBMFont* m_pStatusLevelLabel = nullptr;
	TextArea* m_pAttemptCurrentLevelLabel = nullptr;
	TextArea* m_pTimeCurrentLevelLabel = nullptr;
	BarBetterShow* m_pNormalBarPerB = nullptr;
	BarBetterShow* m_pPracticeBarPerB = nullptr;
	cocos2d::CCMenu* m_pMenuButtonsConfig = nullptr;
	cocos2d::CCLabelBMFont* m_pQuickSettingsLabel = nullptr;
	cocos2d::CCLabelBMFont* m_pVolumenSettingsLabel = nullptr;
	cocos2d::CCLabelBMFont* m_pVolumenMusicSettingsLabel = nullptr;
	cocos2d::CCLabelBMFont* m_pVolumenSFXSettingsLabel = nullptr;
	cocos2d::CCLabelBMFont* m_pQuestsLabel = nullptr;
	// cocos2d::CCMenu* m_MenuSliders = nullptr;//why?????!!!
	cocos2d::CCMenu* m_pMenuButtonsVolSet = nullptr;
	CCMenuItemSpriteExtra* m_pBtnMusicVPercentage = nullptr;
	CCMenuItemSpriteExtra* m_pBtnSFXVPercentage = nullptr;
	TextArea* m_pDownloadMusicLabel = nullptr;
	bool m_pIsTextureEye = true;
	CustomSongWidget* m_pCustomSongWidget = nullptr;
	CCMenuItemSpriteExtra* m_pButtonQuestAlt = nullptr;
	ScrollLayer* m_buttonsList = nullptr;
	cocos2d::CCSprite* upBtnSpriteList = nullptr;
	cocos2d::CCSprite* downBtnSpriteList = nullptr;
	float totalHeightButtonsList = 0.f;

	static BetterPause* create(PauseLayer*);
	bool init(PauseLayer*);
	void update(float);
	void createQuestMenu();
	void createButtonsMenu();
	void createButtonsMenu2();
	void createLabels();
	void createBars();
	void createQuickButtons();
	void clearQuickSettings();
	void handleOptionsLayers();
	void createCustomSongWidget();
	void createTitlesSeccion();
	void createTimesPlatformer();
	void createToggleButton(cocos2d::SEL_MenuHandler callback, bool on,
		cocos2d::CCMenu* menu, std::string caption, cocos2d::CCPoint pos, float size,
		bool twoColumns, int tag);
	void onOptionsLayer(cocos2d::CCObject* pSender);
	void onHide(cocos2d::CCObject* pSender);
	void musicSliderChanged(cocos2d::CCObject* pSender);
	void sfxSliderChanged(cocos2d::CCObject* pSender);
	void onAutoRetryA(cocos2d::CCObject* pSender);
	void onAutoCheckpointsA(cocos2d::CCObject* pSender);
	void onProgressBarA(cocos2d::CCObject* pSender);
	void onProgressBarPercentageA(cocos2d::CCObject* pSender);
	void onDisableShakeEffects(cocos2d::CCObject* pSender);
	void onShowCursorInGame(cocos2d::CCObject* pSender);
	void updatePercentageObjects();
	void onSetValueMusic(cocos2d::CCObject* pSender);
	void onSetValueSFX(cocos2d::CCObject* pSender);
	void onOpenChallenges(cocos2d::CCObject* pSender);
	void onInfoLevelOpen(cocos2d::CCObject* pSender);
	void createToggleButtonWithGameVariable(const char* key, cocos2d::CCMenu* menu, std::string caption, cocos2d::CCPoint pos, float size, bool twoColumns);
	void onToggleWithGameVariable(cocos2d::CCObject* pSender);
	void onInfoLevelLayer(cocos2d::CCObject* pSender);
	void onToogleRedirection(cocos2d::CCObject* pSender);
	void onHideTrue(cocos2d::CCObject* pSender);
	void onEditorHack(cocos2d::CCObject* pSender);
	virtual void ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent) override;
	void onPracticeMusicSyncRedirection(cocos2d::CCObject* pSender);
	void tryGetExternalButtonsMods();
	void findButtonsRecursively(CCNode* node, std::vector<std::string>& buttonIds, std::vector<CCMenuItemSpriteExtra*>& buttonsExternals);
};

#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include "Utils.hpp"
#include "BetterPause.hpp"
#include "AttemptAtReversingDialogObject.hpp"

using namespace geode::prelude;

class SelectQuickSettings : public FLAlertLayer, public cocos2d::CCTextFieldDelegate, public FLAlertLayerProtocol
{
public:
	static bool GameOptionsLayer_getSettings;
	static SelectQuickSettings* create();
	virtual void keyBackClicked();
	virtual bool init();
	void onClose(CCObject* pSender);
	void keyDown(cocos2d::enumKeyCodes key);
	void onToggleWithGameVariable(CCObject* pSender);
	void handleOptionsLayers();
	void clearQuickSettings();

	cocos2d::extension::CCScale9Sprite* m_pBG = nullptr;
	cocos2d::CCSprite* m_pUnderLine = nullptr;
	cocos2d::CCLabelBMFont* m_pTitleLayer = nullptr;
	std::vector<CCMenuItemToggler*> m_toggles;
};
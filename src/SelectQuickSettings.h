#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include <Geode/ui/Popup.hpp>
#include "Utils.hpp"
#include "BetterPause.hpp"

using namespace geode::prelude;

class SelectQuickSettings : public Popup<bool>
{
protected:

public:
	bool m_isInGame;
	bool setup(bool mod) override;

	static bool GameOptionsLayer_getSettings;
	static SelectQuickSettings* create(bool);
	virtual void keyBackClicked();
	virtual void onClose(CCObject* pSender);
	void keyDown(cocos2d::enumKeyCodes key);
	void onToggleWithGameVariable(CCObject* pSender);
	void handleOptionsLayers();

	cocos2d::CCSprite* m_pUnderLine = nullptr;
	cocos2d::CCLabelBMFont* m_pTitleLayer = nullptr;
	std::vector<CCMenuItemToggler*> m_toggles;
};
#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include <Geode/ui/Popup.hpp>
#include "Utils.hpp"
#include "BetterPause.hpp"

using namespace geode::prelude;

class SelectQuickSettings : public geode::Popup
{
public:
	bool m_isInGame;
	bool setup(bool inGame);

	static bool GameOptionsLayer_getSettings;
	static SelectQuickSettings* create(bool inGame);
	void keyBackClicked() override;
	void onClose(CCObject* pSender) override;
	void keyDown(cocos2d::enumKeyCodes key, double p1) override;
	void onToggleWithGameVariable(CCObject* pSender);
	void handleOptionsLayers();

	cocos2d::CCSprite* m_pUnderLine = nullptr;
	cocos2d::CCLabelBMFont* m_pTitleLayer = nullptr;
	std::vector<CCMenuItemToggler*> m_toggles;
};
#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/ui/Popup.hpp>
#include "Utils.hpp"

class BetterPause;

class MoreOptionsPauseLayer : public geode::Popup<CCNode*>, public cocos2d::CCTextFieldDelegate
{
public:
	static MoreOptionsPauseLayer* create(CCNode* ref);
	bool setup(CCNode* ref) override;
	void onClose(CCObject* pSender) override;
	void onOptionsGame(CCObject* pSender);
	void onOptionsPause(CCObject* pSender);
	void keyDown(cocos2d::enumKeyCodes key, double p1) override;
	void onSettingsMod(CCObject* pSender);
	
	CCNode* betterPauseRef = nullptr;
	cocos2d::extension::CCScale9Sprite* m_pBGOptions = nullptr;
	cocos2d::CCSprite* m_pUnderLine = nullptr;
	cocos2d::CCLabelBMFont* m_pTitleLayer = nullptr;
	cocos2d::CCLabelBMFont* m_pExtraTitle = nullptr;
	cocos2d::extension::CCScale9Sprite* m_pBGExtraButtons = nullptr;
	std::vector<CCMenuItemSpriteExtra*> m_addedExtraButtons;
};
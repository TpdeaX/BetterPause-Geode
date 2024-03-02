#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "Utils.hpp"
#include "BetterPause.hpp"

using namespace geode::prelude;

class MoreOptionsPauseLayer : public Popup<CCNode*>, public cocos2d::CCTextFieldDelegate
{
public:
	static MoreOptionsPauseLayer* create(CCNode* ref);
	bool setup(CCNode* ref) override;
	void onClose(CCObject* pSender);
	void onOptionsGame(CCObject* pSender);
	void onOptionsPause(CCObject* pSender);
	void keyDown(cocos2d::enumKeyCodes key);
	void onSettingsMod(CCObject* pSender);
	
	CCNode* betterPauseRef = nullptr;
	cocos2d::extension::CCScale9Sprite* m_pBGOptions = nullptr;
	cocos2d::CCSprite* m_pUnderLine = nullptr;
	cocos2d::CCLabelBMFont* m_pTitleLayer = nullptr;
};
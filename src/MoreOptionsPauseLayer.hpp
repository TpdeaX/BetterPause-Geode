#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include "Utils.hpp"
#include "BetterPause.hpp"

using namespace geode::prelude;

class MoreOptionsPauseLayer : public FLAlertLayer, public cocos2d::CCTextFieldDelegate, public FLAlertLayerProtocol
{
public:
	static MoreOptionsPauseLayer* create(CCNode* ref);
	virtual bool init(CCNode* ref);
	void onClose(CCObject* pSender);
	void onOptionsGame(CCObject* pSender);
	void onOptionsPause(CCObject* pSender);
	void keyDown(cocos2d::enumKeyCodes key);
	
	CCNode* betterPauseRef = nullptr;
	cocos2d::extension::CCScale9Sprite* m_pBG = nullptr;
	cocos2d::extension::CCScale9Sprite* m_pBGOptions = nullptr;
	cocos2d::CCSprite* m_pUnderLine = nullptr;
	cocos2d::CCLabelBMFont* m_pTitleLayer = nullptr;
	cocos2d::CCMenu* m_menuButtons = nullptr;
};
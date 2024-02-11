#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include "Utils.hpp"

using namespace geode::prelude;

class SetVolumenPopup : public FLAlertLayer, public cocos2d::CCTextFieldDelegate, public FLAlertLayerProtocol, public TextInputDelegate
{
public:
	cocos2d::CCMenu* m_pButtonsMenu = nullptr;
	CCMenuItemSpriteExtra* m_pSetValueBtn = nullptr;
	cocos2d::extension::CCScale9Sprite* m_pBGInputTextValue = nullptr;
	CCTextInputNode* m_pInputTextValue = nullptr;
	Slider* m_pSliderRef = nullptr;

	static SetVolumenPopup* create(Slider* ref);
	virtual void keyBackClicked();
	virtual bool init(Slider* ref);
	void keyDown(cocos2d::enumKeyCodes key);
	void onSet(cocos2d::CCObject* pSender);
};
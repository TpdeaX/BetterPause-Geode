#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include <Geode/ui/Popup.hpp>
#include "Utils.hpp"

using namespace geode::prelude;

class SetVolumenPopup : public Popup<Slider*>, public TextInputDelegate
{
public:

	bool setup(Slider* ref) override;

	CCMenuItemSpriteExtra* m_pSetValueBtn = nullptr;
	cocos2d::extension::CCScale9Sprite* m_pBGInputTextValue = nullptr;
	CCTextInputNode* m_pInputTextValue = nullptr;
	Slider* m_pSliderRef = nullptr;

	static SetVolumenPopup* create(Slider* ref);
	virtual void keyBackClicked();
	void keyDown(cocos2d::enumKeyCodes key);
	void onSet(cocos2d::CCObject* pSender);
};
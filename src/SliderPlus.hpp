#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include "Utils.hpp"
#include "SetVolumenPopup.hpp"

using namespace geode::prelude;

class SliderPlus : public CCNode
{
public:
	static SliderPlus* create(std::string labelDesc, cocos2d::CCNode* targetSlider, cocos2d::SEL_MenuHandler handleSlider,
		cocos2d::CCNode* targetBtn, cocos2d::SEL_MenuHandler handleBtn, float value);
	virtual bool init(std::string labelDesc, cocos2d::CCNode* targetSlider, cocos2d::SEL_MenuHandler handleSlider,
		cocos2d::CCNode* targetBtn, cocos2d::SEL_MenuHandler handleBtn, float value);
	void onSliderChanger(cocos2d::CCObject* sender);
	void onBtn(cocos2d::CCObject* sender);

	Slider* sliderMain = nullptr;
	cocos2d::CCLabelBMFont* label = nullptr;
	cocos2d::CCLabelBMFont* valueSlider = nullptr;
	cocos2d::CCMenu* menu = nullptr;
	CCMenuItemSpriteExtra* button = nullptr;
	cocos2d::SEL_MenuHandler handleSlider_ = nullptr;
	cocos2d::SEL_MenuHandler handleBtn_ = nullptr;
	cocos2d::CCNode* targetSlider_ = nullptr;
	cocos2d::CCNode* targetBtn_ = nullptr;
};
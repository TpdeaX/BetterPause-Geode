#include "SliderPlus.hpp"

SliderPlus* SliderPlus::create(std::string labelDesc, cocos2d::CCNode* targetSlider, cocos2d::SEL_MenuHandler handleSlider,
	cocos2d::CCNode* targetBtn, cocos2d::SEL_MenuHandler handleBtn, float value)
{
	auto node = new SliderPlus();
	if (node && node->init(labelDesc, targetSlider, handleSlider, targetBtn, handleBtn, value))
	{
		node->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(node);
	}
	return node;
}

bool SliderPlus::init(std::string labelDesc, cocos2d::CCNode* targetSlider, cocos2d::SEL_MenuHandler handleSlider,
    cocos2d::CCNode* targetBtn, cocos2d::SEL_MenuHandler handleBtn, float value) {

    if (!cocos2d::CCNode::init()) {
        return false;
    }


    handleSlider_ = handleSlider;
    handleBtn_ = handleBtn;
    targetSlider_ = targetSlider;
    targetBtn_ = targetBtn;

    sliderMain = Slider::create(this, (cocos2d::SEL_MenuHandler)&SliderPlus::onSliderChanger, 1.f);
    sliderMain->setValue(value);
    sliderMain->setScale(0.8f);
    sliderMain->setAnchorPoint({ 0.f, 0.f });
    addChild(sliderMain);

    label = cocos2d::CCLabelBMFont::create(labelDesc.c_str(), "bigFont.fnt");
    label->setScale(0.4f);
    label->setAnchorPoint({ 1.f, 0.5f });
    label->setPosition({ this->sliderMain->getPositionX() - 100.f, this->sliderMain->getPositionY() });
    addChild(label);

    valueSlider = cocos2d::CCLabelBMFont::create("0%", "goldFont.fnt");
    valueSlider->setString(
        cocos2d::CCString::createWithFormat("%i%%", static_cast<int>(this->sliderMain->getThumb()->getValue() * 100))->getCString());
    valueSlider->setScale(0.6f);

    menu = cocos2d::CCMenu::create();
    menu->setPosition({ 0.f, 0.f });
    addChild(menu);

    auto button = CCMenuItemSpriteExtra::create(valueSlider, this, (cocos2d::SEL_MenuHandler)&SliderPlus::onBtn);
    button->setPosition({ this->sliderMain->getPositionX() + 120.f, this->sliderMain->getPositionY() });
    menu->addChild(button);

    this->setContentSize({ 160.f, 0.f });

    return true;
}

void SliderPlus::onSliderChanger(cocos2d::CCObject* sender) {
    auto valueV = sliderMain->getThumb()->getValue();
    valueSlider->setString(
        cocos2d::CCString::createWithFormat("%i%%", static_cast<int>(valueV * 100))->getCString());

    if (handleSlider_ && targetSlider_) {
        (targetSlider_->*handleSlider_)(sender);
    }
}

void SliderPlus::onBtn(cocos2d::CCObject* sender) {
    auto popup = SetVolumenPopup::create(sliderMain);
    popup->setID("popup-betterpause");
    popup->show();

    if (handleBtn_ && targetBtn_) {
        (targetBtn_->*handleBtn_)(sender);
    }
}

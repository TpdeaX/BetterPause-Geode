#include "SetVolumenPopup.hpp"

SetVolumenPopup* SetVolumenPopup::create(Slider* m_pSliderRef)
{
	auto node = new SetVolumenPopup();
	if (node && node->init(200.f, 100.f, "GJ_square02.png"))
	{
		node->setup(m_pSliderRef);
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return nullptr;
}

void SetVolumenPopup::onSet(cocos2d::CCObject* pSender)
{
	int numberAs = 0;
	try
	{
		numberAs = std::stoi(this->m_pInputTextValue->getString());
		numberAs = std::clamp(numberAs, 0, 100);
		if (this->m_pSliderRef) {
			this->m_pSliderRef->setValue(static_cast<float>(numberAs) / 100.f);
			if (this->m_pSliderRef->getThumb()) {
				this->m_pSliderRef->getThumb()->activate();
			}
			this->m_pSliderRef->updateBar();
		}
	}
	catch (...)
	{
	}
	this->keyBackClicked();
}

void SetVolumenPopup::keyBackClicked()
{
	this->onClose(nullptr);
}

void SetVolumenPopup::onClose(cocos2d::CCObject* pSender)
{
	Popup::onClose(pSender);
}

bool SetVolumenPopup::setup(Slider* m_pSliderRef)
{
	m_noElasticity = true;
	this->m_pSliderRef = m_pSliderRef;

	float popupW = 230.f;
	float popupH = 125.f;

	this->m_bgSprite->setContentSize({ popupW, popupH });
	this->m_bgSprite->setPosition({ 0.f, 0.f });
	this->m_bgSprite->setColor({ 14, 14, 18 });
	this->m_bgSprite->setOpacity(185);

	m_buttonMenu->setPosition({ 0.f, 0.f });

	m_closeBtn->removeFromParentAndCleanup(true);
	auto closeSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
	closeSpr->setScale(0.8f);
	m_closeBtn = CCMenuItemSpriteExtra::create(closeSpr, this, (cocos2d::SEL_MenuHandler)&SetVolumenPopup::onClose);
	m_closeBtn->setPosition({ -popupW / 2.f + 14.f, popupH / 2.f - 14.f });
	m_buttonMenu->addChild(m_closeBtn);

	auto titleLayer = cocos2d::CCLabelBMFont::create("- Set Value -", "goldFont.fnt");
	titleLayer->setScale(0.7f);
	titleLayer->setPosition({ 0.f, popupH / 2.f - 18.f });
	m_mainLayer->addChild(titleLayer);

	auto underline = cocos2d::CCSprite::createWithSpriteFrameName("floorLine_001.png");
	underline->setPosition({ 0.f, popupH / 2.f - 30.f });
	underline->setScaleX(0.45f);
	underline->setScaleY(0.6f);
	underline->setOpacity(100);
	m_mainLayer->addChild(underline);

	float curVal = 1.0f;
	if (this->m_pSliderRef && this->m_pSliderRef->getThumb()) {
		curVal = this->m_pSliderRef->getThumb()->getValue();
	}
	if (std::isnan(curVal) || std::isinf(curVal)) curVal = 1.0f;
	curVal = std::clamp(curVal, 0.0f, 1.0f);
	int intVal = std::clamp(static_cast<int>(std::round(curVal * 100.f)), 0, 100);

	// Input box background
	m_pBGInputTextValue = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
	m_pBGInputTextValue->setContentSize({ 65.f, 28.f });
	m_pBGInputTextValue->setPosition({ -38.f, -12.f });
	m_pBGInputTextValue->setColor({ 20, 20, 25 });
	m_pBGInputTextValue->setOpacity(160);
	m_mainLayer->addChild(m_pBGInputTextValue);

	m_pInputTextValue = CCTextInputNode::create(45.f, 20.f, "Vol", "bigFont.fnt");
	m_pInputTextValue->setString(gd::string(std::to_string(intVal)));
	m_pInputTextValue->setLabelPlaceholderColor({ 0x75, 0xAA, 0xF0 });
	m_pInputTextValue->m_maxLabelLength = 3;
	m_pInputTextValue->setAllowedChars("0123456789");
	m_pInputTextValue->setMaxLabelScale(0.65f);
	m_pInputTextValue->setPosition({ -38.f, -12.f });
	m_pInputTextValue->setDelegate(this);
	m_mainLayer->addChild(m_pInputTextValue);

	auto perSymbol = cocos2d::CCLabelBMFont::create("%", "bigFont.fnt");
	perSymbol->setScale(0.55f);
	perSymbol->setPosition({ 5.f, -12.f });
	m_mainLayer->addChild(perSymbol);

	auto buttonSetValue = ButtonSprite::create("Set", 0xFC, false, "goldFont.fnt", "GJ_button_01.png", 30.f, 0.6f);
	m_pSetValueBtn = CCMenuItemSpriteExtra::create(buttonSetValue, this, (cocos2d::SEL_MenuHandler)&SetVolumenPopup::onSet);
	m_pSetValueBtn->setPosition({ 50.f, -12.f });
	m_buttonMenu->addChild(m_pSetValueBtn);

	this->setMouseEnabled(true);
	this->setKeyboardEnabled(true);

	return true;
}

void SetVolumenPopup::keyDown(cocos2d::enumKeyCodes key, double p1)
{
	switch (key)
	{
	case cocos2d::enumKeyCodes::KEY_Space:
		this->keyBackClicked();
		break;
	case cocos2d::enumKeyCodes::CONTROLLER_X:
		this->keyBackClicked();
		break;
	case cocos2d::enumKeyCodes::KEY_Left:
	case cocos2d::enumKeyCodes::CONTROLLER_Left:
		break;
	case cocos2d::enumKeyCodes::KEY_Right:
	case cocos2d::enumKeyCodes::CONTROLLER_Right:
		break;
	default:
		Popup::keyDown(key, p1);
	}
}

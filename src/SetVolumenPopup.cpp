#include "SetVolumenPopup.hpp"

SetVolumenPopup* SetVolumenPopup::create(Slider* m_pSliderRef)
{
	auto node = new SetVolumenPopup();
	if (node && node->init(200.f, 100.f, m_pSliderRef))
	{
		node->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(node);
	}
	return node;
}

void SetVolumenPopup::onSet(cocos2d::CCObject* pSender)
{
	int numberAs = 0;
	try
	{
		numberAs = std::stoi(this->m_pInputTextValue->getString());
		if (this->m_pSliderRef) {
			numberAs = std::min(100, std::max(0, numberAs));
			this->m_pSliderRef->setValue(static_cast<float>(numberAs) / 100.f);
			this->m_pSliderRef->getThumb()->activate();
			this->m_pSliderRef->updateBar();
		}
	}
	catch (std::exception const& e)
	{
		//numberAs = this->m_pSliderRef->m_touchLogic->getThumb()->getValue() * 100;
		// FLAlertLayer::create(nullptr, "Error", "OK", nullptr, "There was an error processing the number.")->show();
	}
	this->keyBackClicked();
}

void SetVolumenPopup::keyBackClicked()
{
	FLAlertLayer::keyBackClicked();
}

bool SetVolumenPopup::setup(Slider* m_pSliderRef)
{
	this->m_pSliderRef = m_pSliderRef;

	m_bgSprite->initWithFile("GJ_square02.png");
	m_bgSprite->setContentSize({ 200.f, 100.f });

	m_buttonMenu->setPosition({ Utils::WinSize().width / 2.f - 98.f, Utils::WinSize().height / 2.f + 50.f });

	m_closeBtn->setPosition({ 0.f, 0.f });

	auto titleLayer = cocos2d::CCLabelBMFont::create("- Set Value -", "goldFont.fnt");
	titleLayer->setScale(0.7f);
	titleLayer->setPosition(Utils::WinSize().width / 2.f, Utils::WinSize().height / 2.f + 35.f);
	m_mainLayer->addChild(titleLayer);

	auto underline = cocos2d::CCSprite::createWithSpriteFrameName("floorLine_001.png");
	underline->setPosition({ Utils::WinSize().width / 2.f, Utils::WinSize().height / 2.f + 18.f });
	underline->setScaleX(0.4f);
	underline->setScaleY(0.5f);
	underline->setOpacity(100);
	m_mainLayer->addChild(underline);

	auto buttonSetValue = ButtonSprite::create("Set", 0xFC, false, "goldFont.fnt", "GJ_button_01.png", 30.f, 0.6f);
	m_pSetValueBtn = CCMenuItemSpriteExtra::create(buttonSetValue, this, (cocos2d::SEL_MenuHandler)&SetVolumenPopup::onSet);
	m_pSetValueBtn->setPosition({ 155.f, -65.f });
	m_buttonMenu->addChild(m_pSetValueBtn);

	m_pBGInputTextValue = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
	m_pBGInputTextValue->setContentSize({ 70.f, 30.f });
	m_pBGInputTextValue->setScale(1.f);
	m_pBGInputTextValue->setPosition({ Utils::WinSize().width / 2.f - 40.f, Utils::WinSize().height / 2.f - 14.f });
	m_pBGInputTextValue->setColor({ 255, 255, 255 });
	m_pBGInputTextValue->setOpacity(100);
	m_mainLayer->addChild(m_pBGInputTextValue);

	m_pInputTextValue = CCTextInputNode::create(50.f, 20.f, "Vol", "bigFont.fnt");
	m_pInputTextValue->setString(gd::string(std::to_string(static_cast<int>(this->m_pSliderRef->getThumb()->getValue() * 100))));
	m_pInputTextValue->setLabelPlaceholderColor({ 0x75, 0xAA, 0xF0 });
	m_pInputTextValue->m_maxLabelLength = 3;
	m_pInputTextValue->setAllowedChars("0123456789");
	m_pInputTextValue->setMaxLabelScale(0.7f);
	m_pInputTextValue->setPosition({ Utils::WinSize().width / 2.f - 41.f, Utils::WinSize().height / 2.f - 14.f });
	m_pInputTextValue->setDelegate(this);
	m_mainLayer->addChild(m_pInputTextValue);

	auto perSymbol = cocos2d::CCLabelBMFont::create("%", "bigFont.fnt");
	perSymbol->setScale(0.6f);
	perSymbol->setPosition({ Utils::WinSize().width / 2.f + 9.f, Utils::WinSize().height / 2.f - 15.f });
	m_mainLayer->addChild(perSymbol);

	this->setMouseEnabled(true);
	this->setKeyboardEnabled(true);

	return true;
}

void SetVolumenPopup::keyDown(cocos2d::enumKeyCodes key)
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
		CCLayer::keyDown(key);
	}
}

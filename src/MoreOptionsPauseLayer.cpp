#include "MoreOptionsPauseLayer.hpp"

MoreOptionsPauseLayer* MoreOptionsPauseLayer::create(CCNode* ref)
{
	auto node = new MoreOptionsPauseLayer();
	if (node && node->init(250.f, 200.f, ref))
	{
		node->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(node);
	}
	return node;
}

bool MoreOptionsPauseLayer::setup(CCNode* ref)
{

	m_noElasticity = true;

	this->betterPauseRef = ref;


	m_mainLayer->setPosition({ Utils::WinSize().width * -1.f,  Utils::WinSize().height / 2.f});

	this->m_bgSprite->initWithFile("GJ_square01.png");
	this->m_bgSprite->setPosition({ 0.f, 0.f });
	this->m_bgSprite->setContentSize({ 250.f, 200.f });

	m_closeBtn->removeFromParentAndCleanup(true);

	m_buttonMenu->setPosition({ -120.f, 95.f });
	auto imageClose = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
	imageClose->setScale(0.7f);
	m_closeBtn = CCMenuItemSpriteExtra::create(imageClose, this, (cocos2d::SEL_MenuHandler)&MoreOptionsPauseLayer::onClose);
	m_buttonMenu->addChild(m_closeBtn);
	m_buttonMenu->setZOrder(1);

	auto animationEntry = cocos2d::CCEaseElasticOut::create((cocos2d::CCActionInterval*)cocos2d::CCMoveTo::create(0.5f, { Utils::WinSize().width / 2.f, Utils::WinSize().height / 2.f }), 1.f);
	m_mainLayer->runAction(cocos2d::CCSequence::create((cocos2d::CCFiniteTimeAction*)animationEntry, nullptr));

	this->m_pTitleLayer = cocos2d::CCLabelBMFont::create("- Settings -", "goldFont.fnt");
	this->m_pTitleLayer->setScale(0.7f);
	this->m_pTitleLayer->setPositionY(84.f);
	m_mainLayer->addChild(this->m_pTitleLayer);

	this->m_pUnderLine = cocos2d::CCSprite::createWithSpriteFrameName("floorLine_001.png");
	this->m_pUnderLine->setPosition({ 0.f, 69.f });
	this->m_pUnderLine->setScaleX(0.5f);
	this->m_pUnderLine->setScaleY(0.8f);
	this->m_pUnderLine->setOpacity(100);
	m_mainLayer->addChild(this->m_pUnderLine);

	this->m_pBGOptions = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
	this->m_pBGOptions->setContentSize({ 215.f, 145.f });
	this->m_pBGOptions->setPositionY(-11.f);
	this->m_pBGOptions->setColor({ 30, 30, 30 });
	this->m_pBGOptions->setOpacity((GLubyte)70);
	m_mainLayer->addChild(this->m_pBGOptions);

	auto imageButtonRealPauseSettings = cocos2d::CCSprite::createWithSpriteFrameName("GJ_pauseBtn_001.png");
	imageButtonRealPauseSettings->setPosition({ 175.f, 18.f });
	auto txtRealOptionsPause = cocos2d::CCLabelBMFont::create("Pause Options", "bigFont.fnt");
	txtRealOptionsPause->setScale(0.6f);
	txtRealOptionsPause->setAnchorPoint({ 0.f, 0.5f });
	txtRealOptionsPause->setPosition({ 7.f, 18.f });
	auto m_pBGRealOptionsPause = cocos2d::extension::CCScale9Sprite::create("GJ_button_01.png");
	m_pBGRealOptionsPause->setContentSize({ 180.f, 35.f });
	m_pBGRealOptionsPause->addChild(imageButtonRealPauseSettings);
	m_pBGRealOptionsPause->addChild(txtRealOptionsPause);
	auto m_pRealOptionsPause = CCMenuItemSpriteExtra::create(m_pBGRealOptionsPause, this, (cocos2d::SEL_MenuHandler)&MoreOptionsPauseLayer::onOptionsPause);
	m_pRealOptionsPause->setPosition({ 115.f, -60.f });
	m_buttonMenu->addChild(m_pRealOptionsPause);

	auto imageButtonSettings = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png");
	imageButtonSettings->setPosition({ 175.f, 18.f });
	auto txtOptionGame = cocos2d::CCLabelBMFont::create("Game Options", "bigFont.fnt");
	txtOptionGame->setScale(0.6f);
	txtOptionGame->setAnchorPoint({ 0.f, 0.5f });
	txtOptionGame->setPosition({ 7.f, 18.f });
	auto m_pBGOptionsGamePause = cocos2d::extension::CCScale9Sprite::create("GJ_button_01.png");
	m_pBGOptionsGamePause->setContentSize({ 180.f, 35.f });
	m_pBGOptionsGamePause->addChild(imageButtonSettings);
	m_pBGOptionsGamePause->addChild(txtOptionGame);
	auto m_pOptionsGamePause = CCMenuItemSpriteExtra::create(m_pBGOptionsGamePause, this, (cocos2d::SEL_MenuHandler)&MoreOptionsPauseLayer::onOptionsGame);
	m_pOptionsGamePause->setPosition({ 115.f, -105.f });
	m_buttonMenu->addChild(m_pOptionsGamePause);


	auto imageButtonSettings2 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
	imageButtonSettings2->setPosition({ 175.f, 18.f });
	imageButtonSettings2->setScale(0.8f);
	auto txtOptionGame2 = cocos2d::CCLabelBMFont::create("Mod Options", "bigFont.fnt");
	txtOptionGame2->setScale(0.6f);
	txtOptionGame2->setAnchorPoint({ 0.f, 0.5f });
	txtOptionGame2->setPosition({ 7.f, 18.f });
	auto m_pBGOptionsGamePause2 = cocos2d::extension::CCScale9Sprite::create("GJ_button_01.png");
	m_pBGOptionsGamePause2->setContentSize({ 180.f, 35.f });
	m_pBGOptionsGamePause2->addChild(imageButtonSettings2);
	m_pBGOptionsGamePause2->addChild(txtOptionGame2);
	auto m_pOptionsGamePause2 = CCMenuItemSpriteExtra::create(m_pBGOptionsGamePause2, this, (cocos2d::SEL_MenuHandler)&MoreOptionsPauseLayer::onSettingsMod);
	m_pOptionsGamePause2->setPosition({ 115.f, -150.f });
	m_buttonMenu->addChild(m_pOptionsGamePause2);

	this->setMouseEnabled(true);
	this->setKeyboardEnabled(true);

	return true;
}


void MoreOptionsPauseLayer::onClose(cocos2d::CCObject* pSender)
{
	FLAlertLayer::keyBackClicked();
	Popup::onClose(pSender);
	//Utils::shareDirectorA()->getRunningScene()->addChild(PauseLayer::create(Utils::getplayLayerA()->m_level->m_levelType == GJLevelType::Editor));
}

void MoreOptionsPauseLayer::keyDown(cocos2d::enumKeyCodes key)
{
	switch (key)
	{
	case cocos2d::enumKeyCodes::KEY_Space:
		break;
	case cocos2d::enumKeyCodes::CONTROLLER_X:
		this->onClose(nullptr);
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

void MoreOptionsPauseLayer::onOptionsGame(cocos2d::CCObject* pSender)
{
	auto ol = OptionsLayer::create();
	Utils::shareDirectorA()->getRunningScene()->addChild(ol, Utils::shareDirectorA()->getRunningScene()->getHighestChildZ());
	ol->showLayer(false);
}

void MoreOptionsPauseLayer::onOptionsPause(cocos2d::CCObject* pSender)
{
	auto dropDown = GameOptionsLayer::create(Utils::getplayLayerA());
	this->addChild(dropDown);
	//dropDown->setPosition(0.0f, 0.0f);
}

void MoreOptionsPauseLayer::onSettingsMod(cocos2d::CCObject* pSender) {
	geode::openSettingsPopup(Mod::get());
}

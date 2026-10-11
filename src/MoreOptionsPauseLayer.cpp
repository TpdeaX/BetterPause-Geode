#include "MoreOptionsPauseLayer.hpp"
#include "BetterPause.hpp"

MoreOptionsPauseLayer* MoreOptionsPauseLayer::create(CCNode* ref)
{
	auto bp = typeinfo_cast<BetterPause*>(ref);
	bool hasExtra = bp && !bp->m_extraButtons.empty();
	float width = hasExtra ? 290.f : 250.f;
	float height = hasExtra ? 260.f : 215.f;

	auto node = new MoreOptionsPauseLayer();
	if (node && node->init(width, height, "GJ_square01.png"))
	{
		node->setup(ref);
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return nullptr;
}

bool MoreOptionsPauseLayer::setup(CCNode* ref)
{
	this->betterPauseRef = ref;

	auto bp = typeinfo_cast<BetterPause*>(ref);
	bool hasExtra = bp && !bp->m_extraButtons.empty();
	float popupW = m_size.width;
	float popupH = m_size.height;

	auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
	cocos2d::CCPoint winCenter = { winSize.width / 2.f, winSize.height / 2.f };

	// Geode's Popup centers m_bgSprite and m_buttonMenu at winCenter
	this->m_pTitleLayer = cocos2d::CCLabelBMFont::create("- Settings -", "goldFont.fnt");
	this->m_pTitleLayer->setScale(0.7f);
	this->m_pTitleLayer->setPosition({ winCenter.x, winCenter.y + popupH / 2.f - 18.f });
	m_mainLayer->addChild(this->m_pTitleLayer);

	this->m_pUnderLine = cocos2d::CCSprite::createWithSpriteFrameName("floorLine_001.png");
	this->m_pUnderLine->setPosition({ winCenter.x, winCenter.y + popupH / 2.f - 30.f });
	this->m_pUnderLine->setScaleX(0.5f);
	this->m_pUnderLine->setScaleY(0.8f);
	this->m_pUnderLine->setOpacity(100);
	m_mainLayer->addChild(this->m_pUnderLine);

	float bgOptionsH = hasExtra ? 135.f : 148.f;
	float bgOptionsY = hasExtra ? (winCenter.y + 24.f) : (winCenter.y - 12.f);

	this->m_pBGOptions = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
	this->m_pBGOptions->setContentSize({ popupW - 35.f, bgOptionsH });
	this->m_pBGOptions->setPosition({ winCenter.x, bgOptionsY });
	this->m_pBGOptions->setColor({ 30, 30, 30 });
	this->m_pBGOptions->setOpacity((GLubyte)70);
	m_mainLayer->addChild(this->m_pBGOptions);

	// m_buttonMenu is already centered at winCenter, so (0, y) centers horizontally
	float yBase = hasExtra ? 64.f : 32.f;
	float yStep = 42.f;

	// 1. Pause Options
	auto imageButtonRealPauseSettings = cocos2d::CCSprite::createWithSpriteFrameName("GJ_pauseBtn_001.png");
	imageButtonRealPauseSettings->setPosition({ 156.f, 17.f });
	imageButtonRealPauseSettings->setScale(0.8f);
	auto txtRealOptionsPause = cocos2d::CCLabelBMFont::create("Pause Options", "bigFont.fnt");
	txtRealOptionsPause->setScale(0.55f);
	txtRealOptionsPause->setAnchorPoint({ 0.f, 0.5f });
	txtRealOptionsPause->setPosition({ 12.f, 17.f });
	auto m_pBGRealOptionsPause = cocos2d::extension::CCScale9Sprite::create("GJ_button_01.png");
	m_pBGRealOptionsPause->setContentSize({ 180.f, 34.f });
	m_pBGRealOptionsPause->addChild(imageButtonRealPauseSettings);
	m_pBGRealOptionsPause->addChild(txtRealOptionsPause);
	auto m_pRealOptionsPause = CCMenuItemSpriteExtra::create(m_pBGRealOptionsPause, this, (cocos2d::SEL_MenuHandler)&MoreOptionsPauseLayer::onOptionsPause);
	m_pRealOptionsPause->setPosition({ 0.f, yBase });
	m_buttonMenu->addChild(m_pRealOptionsPause);

	// 2. Game Options
	auto imageButtonSettings = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png");
	imageButtonSettings->setPosition({ 156.f, 17.f });
	imageButtonSettings->setScale(0.8f);
	auto txtOptionGame = cocos2d::CCLabelBMFont::create("Game Options", "bigFont.fnt");
	txtOptionGame->setScale(0.55f);
	txtOptionGame->setAnchorPoint({ 0.f, 0.5f });
	txtOptionGame->setPosition({ 12.f, 17.f });
	auto m_pBGOptionsGamePause = cocos2d::extension::CCScale9Sprite::create("GJ_button_01.png");
	m_pBGOptionsGamePause->setContentSize({ 180.f, 34.f });
	m_pBGOptionsGamePause->addChild(imageButtonSettings);
	m_pBGOptionsGamePause->addChild(txtOptionGame);
	auto m_pOptionsGamePause = CCMenuItemSpriteExtra::create(m_pBGOptionsGamePause, this, (cocos2d::SEL_MenuHandler)&MoreOptionsPauseLayer::onOptionsGame);
	m_pOptionsGamePause->setPosition({ 0.f, yBase - yStep });
	m_buttonMenu->addChild(m_pOptionsGamePause);

	// 3. Mod Options
	auto imageButtonSettings2 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
	imageButtonSettings2->setPosition({ 156.f, 17.f });
	imageButtonSettings2->setScale(0.7f);
	auto txtOptionGame2 = cocos2d::CCLabelBMFont::create("Mod Options", "bigFont.fnt");
	txtOptionGame2->setScale(0.55f);
	txtOptionGame2->setAnchorPoint({ 0.f, 0.5f });
	txtOptionGame2->setPosition({ 12.f, 17.f });
	auto m_pBGOptionsGamePause2 = cocos2d::extension::CCScale9Sprite::create("GJ_button_01.png");
	m_pBGOptionsGamePause2->setContentSize({ 180.f, 34.f });
	m_pBGOptionsGamePause2->addChild(imageButtonSettings2);
	m_pBGOptionsGamePause2->addChild(txtOptionGame2);
	auto m_pOptionsGamePause2 = CCMenuItemSpriteExtra::create(m_pBGOptionsGamePause2, this, (cocos2d::SEL_MenuHandler)&MoreOptionsPauseLayer::onSettingsMod);
	m_pOptionsGamePause2->setPosition({ 0.f, yBase - yStep * 2.f });
	m_buttonMenu->addChild(m_pOptionsGamePause2);

	// Extra Mod Buttons section
	if (hasExtra) {
		this->m_pExtraTitle = cocos2d::CCLabelBMFont::create("Extra Mod Buttons", "goldFont.fnt");
		this->m_pExtraTitle->setScale(0.45f);
		this->m_pExtraTitle->setPosition({ winCenter.x, winCenter.y - popupH / 2.f + 56.f });
		m_mainLayer->addChild(this->m_pExtraTitle);

		this->m_pBGExtraButtons = cocos2d::extension::CCScale9Sprite::create("GJ_square02.png");
		this->m_pBGExtraButtons->setContentSize({ popupW - 35.f, 40.f });
		this->m_pBGExtraButtons->setPosition({ winCenter.x, winCenter.y - popupH / 2.f + 30.f });
		this->m_pBGExtraButtons->setColor({ 25, 25, 25 });
		this->m_pBGExtraButtons->setOpacity(150);
		m_mainLayer->addChild(this->m_pBGExtraButtons);

		float extraY = -popupH / 2.f + 30.f;
		float countExtra = static_cast<float>(bp->m_extraButtons.size());
		float startX = -((countExtra - 1.f) * 35.f) / 2.f;

		for (size_t i = 0; i < bp->m_extraButtons.size(); i++) {
			auto btn = bp->m_extraButtons[i];
			if (btn) {
				btn->removeFromParent();
				btn->setScale(0.55f);
				btn->setPosition({ startX + static_cast<float>(i) * 35.f, extraY });
				m_buttonMenu->addChild(btn);
				m_addedExtraButtons.push_back(btn);
			}
		}
	}

	this->setMouseEnabled(true);
	this->setKeyboardEnabled(true);

	return true;
}

void MoreOptionsPauseLayer::onClose(cocos2d::CCObject* pSender)
{
	for (auto btn : m_addedExtraButtons) {
		if (btn) {
			btn->removeFromParent();
		}
	}
	m_addedExtraButtons.clear();

	Popup::onClose(pSender);
}

void MoreOptionsPauseLayer::keyDown(cocos2d::enumKeyCodes key, double p1)
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
		Popup::keyDown(key, p1);
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
#ifdef GEODE_IS_MACOS
	auto functionPointer = reinterpret_cast<void (*)()>(*reinterpret_cast<uintptr_t*>(dropDown) + 0x508);
	functionPointer();
#else
	dropDown->show();
#endif
}

void MoreOptionsPauseLayer::onSettingsMod(cocos2d::CCObject* pSender) {
	geode::openSettingsPopup(Mod::get());
}
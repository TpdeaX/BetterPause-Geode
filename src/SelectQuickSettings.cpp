#include "SelectQuickSettings.h"

bool SelectQuickSettings::GameOptionsLayer_getSettings = false;

SelectQuickSettings* SelectQuickSettings::create(bool inGame) {
	auto ret = new SelectQuickSettings();
	if (ret && ret->init(440.f, 290.f, inGame)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

void SelectQuickSettings::handleOptionsLayers() {
	SelectQuickSettings::GameOptionsLayer_getSettings = true;

	auto gameOptionsLayer = GameOptionsLayer::create(Utils::getplayLayerA());
#ifdef GEODE_IS_MACOS
	auto functionPointer = reinterpret_cast<void (*)()>(*reinterpret_cast<uintptr_t*>(gameOptionsLayer) + 0x508);
	functionPointer();
#else
	gameOptionsLayer->show();
#endif
	cocos2d::CCTouchDispatcher::get()->unregisterForcePrio(gameOptionsLayer);
#ifdef GEODE_IS_MACOS
	auto functionPointer2 = reinterpret_cast<void (*)(cocos2d::CCObject*)>(*reinterpret_cast<uintptr_t*>(gameOptionsLayer) + 0x529);
	functionPointer2(nullptr);
#else
	gameOptionsLayer->onClose(nullptr);
#endif

	auto moreOptionsLayer = MoreOptionsLayer::create();
#ifdef GEODE_IS_MACOS
	auto functionPointer3 = reinterpret_cast<void (*)()>(*reinterpret_cast<uintptr_t*>(moreOptionsLayer) + 0x508);
	functionPointer3();
#else
	moreOptionsLayer->show();
#endif
	cocos2d::CCTouchDispatcher::get()->unregisterForcePrio(moreOptionsLayer);
	moreOptionsLayer->onClose(nullptr);
}

bool SelectQuickSettings::setup(bool inGame)
{
	m_noElasticity = true;
	m_isInGame = inGame;

	this->m_bgSprite->initWithFile("GJ_square01.png");
	this->m_bgSprite->setContentSize({ 440.f, 290.f });
	this->m_bgSprite->setPosition({ Utils::WinSize().width / 2.f, Utils::WinSize().height / 2.f });

	m_buttonMenu->setPosition({ 0, 0 });
	m_closeBtn->removeFromParentAndCleanup(true);
	auto imageClose = cocos2d::CCSprite::createWithSpriteFrameName("GJ_backBtn_001.png");
	m_closeBtn = CCMenuItemSpriteExtra::create(imageClose, this, (cocos2d::SEL_MenuHandler)&SelectQuickSettings::onClose);
	m_closeBtn->setPosition({ Utils::WinSize().width / 2.f - 210.f, Utils::WinSize().height / 2 + 140.f });
	m_buttonMenu->addChild(m_closeBtn);

	this->m_pTitleLayer = cocos2d::CCLabelBMFont::create("- Select Quick Settings -", "bigFont.fnt");
	this->m_pTitleLayer->setScale(0.7f);
	this->m_pTitleLayer->setPosition({ Utils::WinSize().width / 2.f, Utils::WinSize().height / 2.f + 120.f });
	m_mainLayer->addChild(this->m_pTitleLayer);

	this->m_pUnderLine = cocos2d::CCSprite::createWithSpriteFrameName("floorLine_001.png");
	this->m_pUnderLine->setPosition({ Utils::WinSize().width / 2.f, Utils::WinSize().height / 2.f + 100.f });
	this->m_pUnderLine->setScaleX(0.5f);
	this->m_pUnderLine->setScaleY(0.8f);
	this->m_pUnderLine->setOpacity(100);
	m_mainLayer->addChild(this->m_pUnderLine);

	auto arrayList = cocos2d::CCArray::create();

	this->handleOptionsLayers();

	BetterPause::quickSettingsNamesG[9] = "Practice Music Sync";

	for (size_t i = 0; i < 10; i++) {
		if (i == 0) {
			auto menu = cocos2d::CCMenu::create();
			auto nameList = cocos2d::CCLabelBMFont::create("- Game Settings- ", "goldFont.fnt");
			nameList->setScale(0.8f);
			nameList->setPosition({ 160.f, 15.f });
			nameList->setAnchorPoint({ 0.5f, 0.5f });
			menu->addChild(nameList);
			arrayList->addObject(menu);
		}

		auto menu = cocos2d::CCMenu::create();
		auto nameList = cocos2d::CCLabelBMFont::create(BetterPause::quickSettingsNamesG[i].c_str(), "bigFont.fnt");
		auto btnSelect = CCMenuItemToggler::createWithStandardSprites(this, (cocos2d::SEL_MenuHandler)&SelectQuickSettings::onToggleWithGameVariable, 0.6f);

		for (int j = 0; j < 6; j++) {
			if (((i + 2) * -1) == BetterPauseManager::sharedState()->posQuickA[j])
			{
				btnSelect->toggle(true);
			}
		}

		if (BetterPause::quickSettingsNamesG[i] == "Practice Music Sync" && !GameStatsManager::sharedState()->isItemUnlocked(UnlockType::GJItem, 0x11)) {
			reinterpret_cast<CCSprite*>(reinterpret_cast<CCMenuItemSpriteExtra*>(btnSelect->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0))->setColor({ 150,150,150 });
			reinterpret_cast<CCSprite*>(reinterpret_cast<CCMenuItemSpriteExtra*>(btnSelect->getChildren()->objectAtIndex(1))->getChildren()->objectAtIndex(0))->setColor({ 150,150,150 });
			nameList->setColor({ 150,150,150 });
		}

		btnSelect->setTag((i + 2) * -1);
		btnSelect->setPosition({ 315.f, 15.f });
		//auto buttonButton = CCMenuItemSpriteExtra::create(ButtonSprite::create("View", (int)(90 * 0.5), true, "bigFont.fnt", "GJ_button_01.png", 44 * 0.6f, 0.5f), menu, nullptr);

		nameList->setPosition({ 10.f, 15.f });
		nameList->setAnchorPoint({ 0.f, 0.5f });
		//buttonButton->setPosition({ 300.f, 26.f });

		menu->addChild(nameList);
		menu->addChild(btnSelect);
		m_toggles.push_back(btnSelect);
		nameList->limitLabelWidth(200.f, 0.5f, 0.1f);

		//menu->addChild(buttonButton);

		arrayList->addObject(menu);
	}

	for (size_t i = 0; i < BetterPause::quickSettingsNames.size(); i++)
	{
		if (i == 0) {
			auto menu = cocos2d::CCMenu::create();
			auto nameList = cocos2d::CCLabelBMFont::create("- Gameplay - ", "goldFont.fnt");
			nameList->setScale(0.8f);
			nameList->setPosition({ 160.f, 15.f });
			nameList->setAnchorPoint({ 0.5f, 0.5f });
			menu->addChild(nameList);
			arrayList->addObject(menu);
		}

		if (i == 20) {
			auto menu = cocos2d::CCMenu::create();
			auto nameList = cocos2d::CCLabelBMFont::create("- Practice - ", "goldFont.fnt");
			nameList->setScale(0.8f);
			nameList->setPosition({ 160.f, 15.f });
			nameList->setAnchorPoint({ 0.5f, 0.5f });
			menu->addChild(nameList);
			arrayList->addObject(menu);
		}

		if (i == 28) {
			auto menu = cocos2d::CCMenu::create();
			auto nameList = cocos2d::CCLabelBMFont::create("- Performance - ", "goldFont.fnt");
			nameList->setScale(0.8f);
			nameList->setPosition({ 160.f, 15.f });
			nameList->setAnchorPoint({ 0.5f, 0.5f });
			menu->addChild(nameList);
			arrayList->addObject(menu);
		}

		if (i == 36) {
			auto menu = cocos2d::CCMenu::create();
			auto nameList = cocos2d::CCLabelBMFont::create("- Audio - ", "goldFont.fnt");
			nameList->setScale(0.8f);
			nameList->setPosition({ 160.f, 15.f });
			nameList->setAnchorPoint({ 0.5f, 0.5f });
			menu->addChild(nameList);
			arrayList->addObject(menu);
		}

		if (i == 41) {
			auto menu = cocos2d::CCMenu::create();
			auto nameList = cocos2d::CCLabelBMFont::create("- Other - ", "goldFont.fnt");
			nameList->setScale(0.8f);
			nameList->setPosition({ 160.f, 15.f });
			nameList->setAnchorPoint({ 0.5f, 0.5f });
			menu->addChild(nameList);
			arrayList->addObject(menu);
		}

		auto menu = cocos2d::CCMenu::create();
		auto nameList = cocos2d::CCLabelBMFont::create(BetterPause::quickSettingsNames[i].c_str(), "bigFont.fnt");
		auto btnSelect = CCMenuItemToggler::createWithStandardSprites(this, (cocos2d::SEL_MenuHandler)&SelectQuickSettings::onToggleWithGameVariable, 0.6f);
		
		for (int j = 0; j < 6; j++){
			if (i == BetterPauseManager::sharedState()->posQuickA[j])
			{
				btnSelect->toggle(true);
			}
		}
		
		btnSelect->setTag(i);
		btnSelect->setPosition({ 315.f, 15.f });

		nameList->setPosition({ 10.f, 15.f });
		nameList->setAnchorPoint({ 0.f, 0.5f });

		menu->addChild(nameList);
		menu->addChild(btnSelect);
		m_toggles.push_back(btnSelect);
		nameList->limitLabelWidth(200.f, 0.5f, 0.1f);


		arrayList->addObject(menu);
	}

	BetterPause::clearQuickSettings();

	auto CustomVi = ListView::create(arrayList, 30.f, 340.f, 200.f);

	auto gjCommentBase = GJCommentListLayer::create(CustomVi, nullptr, { 0, 0, 0 }, 340.f, 200.f, 0);
	gjCommentBase->setPosition({ Utils::WinSize().width / 2.f - 165.f, Utils::WinSize().height / 2.f - 120.f });
	gjCommentBase->setZOrder(2);
	m_buttonMenu->addChild(gjCommentBase);

	this->setMouseEnabled(true);
	this->setKeyboardEnabled(true);

	return true;
}

void SelectQuickSettings::onToggleWithGameVariable(CCObject* pSender)
{
	int count = 0;
	auto toggleButton = reinterpret_cast<CCMenuItemToggler*>(pSender);



	if (toggleButton->getTag() == -11 && !GameStatsManager::sharedState()->isItemUnlocked(UnlockType::GJItem, 0x11) && !toggleButton->isToggled()) {
		auto toggleButton = reinterpret_cast<CCMenuItemToggler*>(pSender);
		auto gameOptionsLayer = GameOptionsLayer::create(Utils::getplayLayerA());

		auto arr = CCArray::create();
		arr->addObject(AttemptAtReversingDialogObject::create(gd::string("Scratch"), gd::string("What's the <cl>point</c> of adding it if you don't have it <cr>unlocked?</c>"), 13, 1, false, { 255,255,255 }));
		auto dl = DialogLayer::createDialogLayer(nullptr, arr, 2);
		dl->animateInRandomSide();
		CCScene::get()->addChild(dl, CCScene::get()->getHighestChildZ());


		//auto dialogObj = AttemptAtReversingDialogObject::create("Scratch", "You need a <cg>Music Unlocker</c> to play <cy>normal music</c> in <cl>practice mod e</c>.", 2, 512.f, false, { 0, 0, 0 });
		//auto dialogLayer = DialogLayer::create(reinterpret_cast<DialogObject*>(dialogObj), 2);
		//Utils::shareDirectorA()->getRunningScene()->addChild(dialogLayer, Utils::shareDirectorA()->getRunningScene()->getHighestChildZ());
		//dialogLayer->animateIn(DialogAnimationType::FromCenter);
	}

	for (auto buttonToggle : m_toggles)
	{
		if (buttonToggle->isToggled())
		{
			count++;
		}
	}

	if (reinterpret_cast<CCMenuItemToggler*>(pSender)->isToggled())
	{
		count--;
	}
	else
	{
		count++;
	}

	//std::cout << count << "\t" << reinterpret_cast<CCMenuItemToggler*>(pSender)->isToggled() << std::endl;

	if (count > 6)
	{
		auto flalert = FLAlertLayer::create(
			"Maximum Exceeded",
			gd::string("You've exceeded the maximum number of quick settings."),
			"OK"
		);
		flalert->show();
		reinterpret_cast<CCMenuItemToggler*>(pSender)->toggle(true);
	}

	if (count < 3)
	{
		auto flalert = FLAlertLayer::create(
			 "Minimum Not Met",
			gd::string("You haven't met the minimum number of quick settings."),
			"OK");
		flalert->show();
		reinterpret_cast<CCMenuItemToggler*>(pSender)->toggle(false);
	}
}

void SelectQuickSettings::keyBackClicked()
{
	SelectQuickSettings::onClose(nullptr);
}

void SelectQuickSettings::onClose(cocos2d::CCObject* pSender)
{
	for (int j = 0; j < 6; j++)
	{
		BetterPauseManager::sharedState()->posQuickA[j] = -1;
	}

	int count = 0;

	for (size_t i = 0; i < m_toggles.size(); i++)
	{
		auto buttonToggle = m_toggles[i];
		if (buttonToggle->isToggled())
		{
			BetterPauseManager::sharedState()->posQuickA[count] = m_toggles[i]->getTag();
			count++;
		}
	}

	Popup::onClose(pSender);
}

void SelectQuickSettings::keyDown(cocos2d::enumKeyCodes key)
{
	switch (key)
	{
	case cocos2d::enumKeyCodes::KEY_Space:
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
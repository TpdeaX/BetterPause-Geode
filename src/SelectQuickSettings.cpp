#include "SelectQuickSettings.h"
#include "DebugLogger.hpp"

bool SelectQuickSettings::GameOptionsLayer_getSettings = false;

SelectQuickSettings* SelectQuickSettings::create(bool inGame) {
	auto ret = new SelectQuickSettings();
	if (ret && ret->init(440.f, 290.f, "GJ_square01.png")) {
		ret->setup(inGame);
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

void SelectQuickSettings::handleOptionsLayers() {
	BetterPauseDebug::log("SelectQuickSettings::handleOptionsLayers() START");
	BetterPauseDebug::log("  Initial quickSettingsNames.size() = " + std::to_string(BetterPause::quickSettingsNames.size()));
	BetterPauseDebug::log("  Initial quickSettingsNamesG.size() = " + std::to_string(BetterPause::quickSettingsNamesG.size()));

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

	BetterPauseDebug::log("  After GameOptionsLayer: quickSettingsNamesG.size() = " + std::to_string(BetterPause::quickSettingsNamesG.size()));

	// Clear before opening MoreOptionsLayer so we do not accumulate duplicates
	BetterPause::quickSettingsNames.clear();
	BetterPause::quickSettingsKeys.clear();
	BetterPause::quickSettingsDescriptions.clear();

	auto moreOptionsLayer = MoreOptionsLayer::create();
	if (moreOptionsLayer) {
		BetterPauseDebug::log("  MoreOptionsLayer created successfully. Children count = " + std::to_string(moreOptionsLayer->getChildrenCount()));
		if (moreOptionsLayer->getChildren()) {
			for (int i = 0; i < moreOptionsLayer->getChildrenCount(); i++) {
				auto child = static_cast<CCNode*>(moreOptionsLayer->getChildren()->objectAtIndex(i));
				std::string childID = child->getID().empty() ? "(no ID)" : child->getID();
				BetterPauseDebug::log("    Child[" + std::to_string(i) + "] ID: '" + childID + "' visible: " + (child->isVisible() ? "true" : "false"));
			}
		}

#ifdef GEODE_IS_MACOS
		auto functionPointer3 = reinterpret_cast<void (*)()>(*reinterpret_cast<uintptr_t*>(moreOptionsLayer) + 0x508);
		functionPointer3();
#else
		moreOptionsLayer->show();
#endif
		cocos2d::CCTouchDispatcher::get()->unregisterForcePrio(moreOptionsLayer);
		moreOptionsLayer->onClose(nullptr);
	} else {
		BetterPauseDebug::log("  MoreOptionsLayer::create() returned NULL!");
	}

	BetterPauseDebug::log("  After MoreOptionsLayer: quickSettingsNames.size() = " + std::to_string(BetterPause::quickSettingsNames.size()));
	BetterPauseDebug::log("SelectQuickSettings::handleOptionsLayers() END");
}

bool SelectQuickSettings::setup(bool inGame)
{
	m_noElasticity = true;
	m_isInGame = inGame;

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

	if (BetterPause::quickSettingsNamesG.size() > 9) {
		BetterPause::quickSettingsNamesG[9] = "Practice Music Sync";
	}

	auto& posQuick = BetterPauseManager::sharedState()->posQuickA;
	std::string currentPosStr = "[";
	bool allEmpty = true;
	for (size_t k = 0; k < posQuick.size(); ++k) {
		currentPosStr += std::to_string(posQuick[k]) + (k + 1 < posQuick.size() ? ", " : "");
		if (posQuick[k] != -1) allEmpty = false;
	}
	currentPosStr += "]";
	BetterPauseDebug::log("SelectQuickSettings::setup() - Current posQuickA: " + currentPosStr);

	if (allEmpty || posQuick.size() < 6) {
		BetterPauseDebug::log("  posQuickA was empty or all -1! Restoring posQuickASafe defaults so toggles are selected.");
		BetterPauseManager::sharedState()->posQuickA = BetterPauseManager::sharedState()->posQuickASafe;
	}

	int selectedCount = 0;

	for (size_t i = 0; i < 10; i++) {
		if (i == 0) {
			auto menu = cocos2d::CCMenu::create();
			auto nameList = cocos2d::CCLabelBMFont::create("- Game Settings -", "goldFont.fnt");
			nameList->setScale(0.8f);
			nameList->setPosition({ 160.f, 15.f });
			nameList->setAnchorPoint({ 0.5f, 0.5f });
			menu->addChild(nameList);
			arrayList->addObject(menu);
		}

		auto menu = cocos2d::CCMenu::create();
		std::string sName = (i < BetterPause::quickSettingsNamesG.size()) ? BetterPause::quickSettingsNamesG[i] : "";
		auto nameList = cocos2d::CCLabelBMFont::create(sName.c_str(), "bigFont.fnt");
		auto btnSelect = CCMenuItemToggler::createWithStandardSprites(this, (cocos2d::SEL_MenuHandler)&SelectQuickSettings::onToggleWithGameVariable, 0.6f);

		int expectedTag = static_cast<int>(i + 2) * -1;
		bool isSelected = false;
		for (size_t j = 0; j < 6 && j < BetterPauseManager::sharedState()->posQuickA.size(); j++) {
			if (expectedTag == BetterPauseManager::sharedState()->posQuickA[j]) {
				btnSelect->toggle(true);
				isSelected = true;
				selectedCount++;
				break;
			}
		}

		BetterPauseDebug::log("  GameOption[" + std::to_string(i) + "] '" + sName + "' tag=" + std::to_string(expectedTag) + (isSelected ? " -> SELECTED" : ""));

		if (sName == "Practice Music Sync" && !GameStatsManager::sharedState()->isItemUnlocked(UnlockType::GJItem, 0x11)) {
			reinterpret_cast<CCSprite*>(reinterpret_cast<CCMenuItemSpriteExtra*>(btnSelect->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0))->setColor({ 150,150,150 });
			reinterpret_cast<CCSprite*>(reinterpret_cast<CCMenuItemSpriteExtra*>(btnSelect->getChildren()->objectAtIndex(1))->getChildren()->objectAtIndex(0))->setColor({ 150,150,150 });
			nameList->setColor({ 150,150,150 });
		}

		btnSelect->setTag(expectedTag);
		btnSelect->setPosition({ 315.f, 15.f });

		nameList->setPosition({ 10.f, 15.f });
		nameList->setAnchorPoint({ 0.f, 0.5f });

		menu->addChild(nameList);
		menu->addChild(btnSelect);
		m_toggles.push_back(btnSelect);
		nameList->limitLabelWidth(200.f, 0.5f, 0.1f);

		arrayList->addObject(menu);
	}

	for (size_t i = 0; i < BetterPause::quickSettingsNames.size(); i++)
	{
		if (i == 0) {
			auto menu = cocos2d::CCMenu::create();
			auto nameList = cocos2d::CCLabelBMFont::create("- Gameplay -", "goldFont.fnt");
			nameList->setScale(0.8f);
			nameList->setPosition({ 160.f, 15.f });
			nameList->setAnchorPoint({ 0.5f, 0.5f });
			menu->addChild(nameList);
			arrayList->addObject(menu);
		}

		if (i == 20) {
			auto menu = cocos2d::CCMenu::create();
			auto nameList = cocos2d::CCLabelBMFont::create("- Practice -", "goldFont.fnt");
			nameList->setScale(0.8f);
			nameList->setPosition({ 160.f, 15.f });
			nameList->setAnchorPoint({ 0.5f, 0.5f });
			menu->addChild(nameList);
			arrayList->addObject(menu);
		}

		if (i == 28) {
			auto menu = cocos2d::CCMenu::create();
			auto nameList = cocos2d::CCLabelBMFont::create("- Performance -", "goldFont.fnt");
			nameList->setScale(0.8f);
			nameList->setPosition({ 160.f, 15.f });
			nameList->setAnchorPoint({ 0.5f, 0.5f });
			menu->addChild(nameList);
			arrayList->addObject(menu);
		}

		if (i == 36) {
			auto menu = cocos2d::CCMenu::create();
			auto nameList = cocos2d::CCLabelBMFont::create("- Audio -", "goldFont.fnt");
			nameList->setScale(0.8f);
			nameList->setPosition({ 160.f, 15.f });
			nameList->setAnchorPoint({ 0.5f, 0.5f });
			menu->addChild(nameList);
			arrayList->addObject(menu);
		}

		if (i == 41) {
			auto menu = cocos2d::CCMenu::create();
			auto nameList = cocos2d::CCLabelBMFont::create("- Other -", "goldFont.fnt");
			nameList->setScale(0.8f);
			nameList->setPosition({ 160.f, 15.f });
			nameList->setAnchorPoint({ 0.5f, 0.5f });
			menu->addChild(nameList);
			arrayList->addObject(menu);
		}

		auto menu = cocos2d::CCMenu::create();
		auto nameList = cocos2d::CCLabelBMFont::create(BetterPause::quickSettingsNames[i].c_str(), "bigFont.fnt");
		auto btnSelect = CCMenuItemToggler::createWithStandardSprites(this, (cocos2d::SEL_MenuHandler)&SelectQuickSettings::onToggleWithGameVariable, 0.6f);
		
		bool isSelected = false;
		for (size_t j = 0; j < 6 && j < BetterPauseManager::sharedState()->posQuickA.size(); j++){
			if (static_cast<int>(i) == BetterPauseManager::sharedState()->posQuickA[j])
			{
				btnSelect->toggle(true);
				isSelected = true;
				selectedCount++;
				break;
			}
		}

		BetterPauseDebug::log("  MoreOption[" + std::to_string(i) + "] '" + BetterPause::quickSettingsNames[i] + "' tag=" + std::to_string(i) + (isSelected ? " -> SELECTED" : ""));
		
		btnSelect->setTag(static_cast<int>(i));
		btnSelect->setPosition({ 315.f, 15.f });

		nameList->setPosition({ 10.f, 15.f });
		nameList->setAnchorPoint({ 0.f, 0.5f });

		menu->addChild(nameList);
		menu->addChild(btnSelect);
		m_toggles.push_back(btnSelect);
		nameList->limitLabelWidth(200.f, 0.5f, 0.1f);

		arrayList->addObject(menu);
	}

	BetterPauseDebug::log("SelectQuickSettings::setup() FINISHED - Total items selected: " + std::to_string(selectedCount));

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
	auto toggleButton = reinterpret_cast<CCMenuItemToggler*>(pSender);
	if (!toggleButton) return;

	if (toggleButton->getTag() == -11 && !GameStatsManager::sharedState()->isItemUnlocked(UnlockType::GJItem, 0x11) && !toggleButton->isToggled()) {
		auto arr = CCArray::create();
		arr->addObject(DialogObject::create("Scratch", "What's the <cl>point</c> of adding it if you don't have it <cr>unlocked?</c>", 13, 1.0f, false, { 255, 255, 255 }));
		auto dl = DialogLayer::createDialogLayer(nullptr, arr, 2);
		dl->animateInRandomSide();
		CCScene::get()->addChild(dl, CCScene::get()->getHighestChildZ());
		toggleButton->toggle(false);
		return;
	}

	int count = 0;
	for (auto buttonToggle : m_toggles)
	{
		if (buttonToggle->isToggled())
		{
			count++;
		}
	}

	BetterPauseDebug::log("onToggleWithGameVariable: Tag=" + std::to_string(toggleButton->getTag()) + " isToggled=" + (toggleButton->isToggled() ? "true" : "false") + " TotalToggledNow=" + std::to_string(count));

	if (count > 6)
	{
		auto flalert = FLAlertLayer::create(
			"Maximum Exceeded",
			gd::string("You can select up to 6 quick settings."),
			"OK"
		);
		flalert->show();
		toggleButton->toggle(false);
		BetterPauseDebug::log("  Blocked toggle: Exceeded maximum 6 quick settings.");
	}
}

void SelectQuickSettings::keyBackClicked()
{
	SelectQuickSettings::onClose(nullptr);
}

void SelectQuickSettings::onClose(cocos2d::CCObject* pSender)
{
	auto& posQuick = BetterPauseManager::sharedState()->posQuickA;
	posQuick.assign(6, -1);

	int count = 0;
	for (size_t i = 0; i < m_toggles.size() && count < 6; i++)
	{
		auto buttonToggle = m_toggles[i];
		if (buttonToggle->isToggled())
		{
			posQuick[count] = buttonToggle->getTag();
			count++;
		}
	}

	if (count == 0) {
		BetterPauseDebug::log("SelectQuickSettings::onClose() - 0 items selected! Restoring defaults to avoid empty pause menu.");
		posQuick = BetterPauseManager::sharedState()->posQuickASafe;
	}

	std::string savedStr = "[";
	for (size_t k = 0; k < posQuick.size(); ++k) {
		savedStr += std::to_string(posQuick[k]) + (k + 1 < posQuick.size() ? ", " : "");
	}
	savedStr += "]";
	BetterPauseDebug::log("SelectQuickSettings::onClose() - Saved posQuickA: " + savedStr);

	BetterPauseManager::sharedState()->saveState();
	Popup::onClose(pSender);
}

void SelectQuickSettings::keyDown(cocos2d::enumKeyCodes key, double p1)
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
		Popup::keyDown(key, p1);
	}
}
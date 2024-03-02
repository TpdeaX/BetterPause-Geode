#include "BetterPause.hpp"
#include <optional>

std::vector<std::string> BetterPause::quickSettingsNames = {};
std::vector<std::string> BetterPause::quickSettingsDescriptions = {};
std::vector<std::string> BetterPause::quickSettingsKeys = {};
std::vector<std::string> BetterPause::quickSettingsNamesG = {};
std::vector<int> BetterPause::quickSettingsNumbersG = {};
std::vector<bool> BetterPause::quickSettingsEnabledG = {};
std::vector<GameObject*> BetterPause::coinsObjects = {};
float BetterPause::totalLevelTimeBackup = 0.f;

BetterPause* BetterPause::create(PauseLayer* pauLa)
{
	auto ret = new BetterPause();

	if (ret && ret->init(pauLa))
	{
		ret->autorelease();
		return ret;
	}

	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool BetterPause::init(PauseLayer* pauLa)
{
	if (!cocos2d::CCLayer::init())
	{
		return false;
	}

	this->pauseLayer = pauLa;
	typeMenuCreate = Mod::get()->getSettingValue<int64_t>("type-pause");

	switch (Mod::get()->getSettingValue<int64_t>("type-pause")) {
	case 2: {
		this->createClassicPause();
		this->setID("classic-pause-node");
		break;
	}
	case 3: {
		this->createSimplePause();
		this->setID("simple-pause-node");
		break;
	}
	default: {
		this->createBetterPause();
		this->setID("better-pause-node");
		break;
	}
	}
	

	//this->setMouseEnabled(true);
	//this->setTouchEnabled(true);
	//this->registerScriptTouchHandler(0);

	return true;
}

void BetterPause::createBetterPause() {
	this->clearQuickSettings();
	this->createQuestMenu();
	this->createMainButtonsMenu();
	this->createSecondaryButtonsMenu();
	this->createLabels();

	if (!Utils::getplayLayerA()->m_level->isPlatformer())
	{
		this->createBars();
	}
	else
	{
		this->createPlatformerTimes();
	}

	this->createQuickSettingsButtons();
	this->createCustomSongWidget();
	this->createAudioControls();
	this->createSectionTitles();
	this->adjustLayerForAspectRatio();
}

void BetterPause::createSimplePause() {

	auto bgPause = dynamic_cast<cocos2d::extension::CCScale9Sprite*>(pauseLayer->getChildByID("background"));
	auto titleLevel = dynamic_cast<cocos2d::CCLabelBMFont*>(pauseLayer->getChildByID("level-name"));
	auto barNormal = dynamic_cast<cocos2d::CCSprite*>(pauseLayer->getChildByID("normal-progress-bar"));
	auto barPractice = dynamic_cast<cocos2d::CCSprite*>(pauseLayer->getChildByID("practice-progress-bar"));
	auto perNormal = dynamic_cast<cocos2d::CCLabelBMFont*>(pauseLayer->getChildByID("normal-progress-label"));
	auto perPractice = dynamic_cast<cocos2d::CCLabelBMFont*>(pauseLayer->getChildByID("practice-progress-label"));
	auto buttonsMenu = dynamic_cast<cocos2d::CCMenu*>(pauseLayer->getChildByID("center-button-menu"));
	auto pointsLabel = dynamic_cast<cocos2d::CCLabelBMFont*>(pauseLayer->getChildByID("points-label"));
	auto timeLabel = dynamic_cast<cocos2d::CCLabelBMFont*>(pauseLayer->getChildByID("play-time"));

	bgPause->setVisible(true);
	titleLevel->setVisible(true);

	if(barNormal)barNormal->setVisible(true);
	if(barPractice)barPractice->setVisible(true);
	if(perNormal)perNormal->setVisible(true);
	if(perPractice)perPractice->setVisible(true);

	if(pointsLabel)pointsLabel->setVisible(true);
	if(timeLabel)timeLabel->setVisible(true);

	buttonsMenu->setVisible(true);

	bgPause->setContentSize({ bgPause->getContentSize().width, 70.f });
	bgPause->setPositionY(Utils::WinSize().height - 52.f);
	titleLevel->setAnchorPoint({ 0.f, 0.5f });
	titleLevel->setPosition({ 68.f, Utils::WinSize().height - 52.f });
	titleLevel->limitLabelWidth(250.f, 1.f, 0.1f);
	if (barNormal)barNormal->setPosition({ 115.5f, Utils::WinSize().height - 100.f });
	if (barNormal)barNormal->setScale(0.6f);
	if (barPractice)barPractice->setPosition({ 323.5f, Utils::WinSize().height - 100.f });
	if (barPractice)barPractice->setScale(0.6f);
	if (perNormal)perNormal->setPosition({ 115.5f, Utils::WinSize().height - 100.f });
	if (perPractice)perPractice->setPosition({ 323.5f, Utils::WinSize().height - 100.f });

	if (pointsLabel) {
		pointsLabel->setPosition({ Utils::WinSize().width - 20.f, Utils::WinSize().height - 100.f });
		pointsLabel->setAnchorPoint({ 1.f, 0.5f });
	}

	if (timeLabel) {
		timeLabel->setPosition({ 33.5f, Utils::WinSize().height - 100.f });
		timeLabel->setAnchorPoint({ 0.f, 0.5f });
	}

	auto director = cocos2d::CCDirector::sharedDirector();
	auto size = director->getWinSize();
	float aspectRatio = size.width / size.height;

	float desiredAspectRatio = 570.f / 360.f;

	float posX = size.width - 250.f;
	if (aspectRatio > desiredAspectRatio) {
		float desiredWidth = size.height * desiredAspectRatio;
		posX = (size.width - desiredWidth) / 2 + (desiredWidth - 250.f);
	}

	float posY = size.height - 132.f;

	if (aspectRatio < desiredAspectRatio) {
		titleLevel->limitLabelWidth(150.f, 1.f, 0.1f);
		float desiredHeight = size.width / desiredAspectRatio;
		float bottomOffset = (size.height - desiredHeight) / 2;
		float menuHeight = 180.f;
		posY = bottomOffset + menuHeight;
	}

	buttonsMenu->setPosition({ posX, posY });
	buttonsMenu->setScale(0.5f);




	secondaryMenuButtons = cocos2d::CCMenu::create();
	secondaryMenuButtons->setPosition({ 40.f, Utils::WinSize().height - 50.f });
	this->addChild(secondaryMenuButtons);

	auto settingsButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
	settingsButtonImage->setScale(0.6f);
	auto settingsButton = CCMenuItemSpriteExtra::create(settingsButtonImage, this, (cocos2d::SEL_MenuHandler)&BetterPause::onOptionsLayer);
	secondaryMenuButtons->addChild(settingsButton);


}

void BetterPause::createClassicPause() {
	for (size_t i = 1; i < pauseLayer->getChildrenCount(); i++)
	{
		auto node = dynamic_cast<cocos2d::CCNode*>(pauseLayer->getChildren()->objectAtIndex(i));
		if (node)
		{
			node->setVisible(true);
		}
	}

	menuToggles = cocos2d::CCMenu::create();
	auto pointBeggind = cocos2d::CCPoint(Utils::WinSize().width / 2 - 162.f, 25.f);
	this->createToggleButtonOldMethod((cocos2d::SEL_MenuHandler)&BetterPause::onRedirectionToggle, Utils::shareManager()->getGameVariable("0026"), menuToggles, "Auto-Retry", pointBeggind, -2);
	pointBeggind.x += 130.f;
	this->createToggleButtonOldMethod((cocos2d::SEL_MenuHandler)&BetterPause::onRedirectionToggle, Utils::shareManager()->getGameVariable("0027"), menuToggles, "Auto-Checkpoints", pointBeggind, -3);
	pointBeggind.x += 120.f;
	if (Utils::getplayLayerA()->m_level->isPlatformer()) {
		this->createToggleButtonOldMethod((cocos2d::SEL_MenuHandler)&BetterPause::onRedirectionToggle, Utils::shareManager()->getGameVariable("0145"), menuToggles, "Show Time", pointBeggind, -8);
	}
	else {
		this->createToggleButtonOldMethod((cocos2d::SEL_MenuHandler)&BetterPause::onRedirectionToggle, Utils::shareManager()->m_showProgressBar, menuToggles, "Bar", pointBeggind, -4);
		pointBeggind.x += 70.f;
		this->createToggleButtonOldMethod((cocos2d::SEL_MenuHandler)&BetterPause::onRedirectionToggle, Utils::shareManager()->getGameVariable("0040"), menuToggles, "%", pointBeggind, -5);	
	}
	this->addChild(menuToggles);

	auto settingsButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
	settingsButtonImage->setScale(0.6f);
	auto settingsButton = CCMenuItemSpriteExtra::create(settingsButtonImage, this, (cocos2d::SEL_MenuHandler)&BetterPause::onOptionsLayer);

	if (auto menu = pauseLayer->getChildByID("right-button-menu")) {
		menu->addChild(settingsButton);
		menu->updateLayout();
	}

}

void BetterPause::createSectionTitles() {

	this->quickSettingsLabel = cocos2d::CCLabelBMFont::create("Quick Settings", "goldFont.fnt");
	this->quickSettingsLabel->setScale(0.5f);
	this->quickSettingsLabel->setPosition({ 165.f, 164.f });
	this->addChild(this->quickSettingsLabel);

	this->volumeSettingsLabel = cocos2d::CCLabelBMFont::create("Volume Settings:", "bigFont.fnt");
	this->volumeSettingsLabel->setScale(0.5f);
	this->volumeSettingsLabel->setPosition({ Utils::WinSize().width - 80.f, Utils::WinSize().height - 40.f });
	this->volumeSettingsLabel->setAnchorPoint({ 1.f, 0.f });
	this->addChild(this->volumeSettingsLabel);

	this->questsLabel = cocos2d::CCLabelBMFont::create("Quest:", "bigFont.fnt");
	this->questsLabel->setScale(0.5f);
	this->questsLabel->setPosition({ Utils::WinSize().width - 120.f, Utils::WinSize().height - 120.f });
	this->questsLabel->setAnchorPoint({ 1.f, 0.f });
	this->addChild(this->questsLabel);

}

void BetterPause::handleOptionsLayers() {
	auto gameOptionsLayer = GameOptionsLayer::create(Utils::getplayLayerA());
	gameOptionsLayer->show();
	gameOptionsLayer->unregisterScriptTouchHandler();
	Utils::shareDirectorA()->getTouchDispatcher()->unregisterForcePrio(gameOptionsLayer);
	gameOptionsLayer->onClose(nullptr);

	auto moreOptionsLayer = MoreOptionsLayer::create();
	moreOptionsLayer->show();
	moreOptionsLayer->onClose(nullptr);
}


void BetterPause::createPlatformerTimes() {
	auto platformerProgressBetter = ProgressPlataformerBetter::create();
	platformerProgressBetter->setPosition({ 86.f, Utils::WinSize().height - 90.f });
	this->addChild(platformerProgressBetter);
}

void BetterPause::createCustomSongWidget() {
	int levelSongID = Utils::getplayLayerA()->m_level->m_songID;
	SongInfoObject* songInfoObject = nullptr;

	if (levelSongID == 0) {
		auto zeroIdSong = GameManager::sharedState()->getPlayLayer()->m_level->m_audioTrack;
		songInfoObject = LevelTools::getSongObject(zeroIdSong,this);
	}
	else {
		songInfoObject = SongInfoObject::create(levelSongID);
	}

	this->customSongWidget = CustomSongWidget::create(songInfoObject, 0, 0, 0, 1, levelSongID == 0, 0, 0);
	this->customSongWidget->setPosition(172.f, 50.f);
	this->customSongWidget->setScale(0.6f);
	this->addChild(customSongWidget);

	intptr_t offsetUnkFloatCSW = 0;

#ifdef GEODE_IS_WINDOWS
	offsetUnkFloatCSW = 0x18c;
#endif
#ifdef GEODE_IS_ANDROID64
	offsetUnkFloatCSW = 0x170;
#endif
#ifdef GEODE_IS_ANDROID32
	offsetUnkFloatCSW = 0x128;
#endif

	// Interpretar como void*
	void* voidPtrValue = Utils::from<void*>(songInfoObject, offsetUnkFloatCSW);
	std::cout << "void* value: " << voidPtrValue << std::endl;

	// Interpretar como int
	int intValue = Utils::from<int>(songInfoObject, offsetUnkFloatCSW);
	std::cout << "int value: " << intValue << std::endl;

	// Interpretar como float
	float floatValue = Utils::from<float>(songInfoObject, offsetUnkFloatCSW);
	std::cout << "float value: " << floatValue << std::endl;

	if (Utils::from<void*>(songInfoObject, offsetUnkFloatCSW) != 0) {
		customSongWidget->onGetSongInfo(customSongWidget);
	}

	gd::string songIDs = Utils::getplayLayerA()->m_level->m_songIDs;
	gd::string sfxIDs = Utils::getplayLayerA()->m_level->m_sfxIDs;
	customSongWidget->updateWithMultiAssets(songIDs, sfxIDs, 0);
}


void BetterPause::createQuickSettingsButtons() {
	this->quickSettingsMenu = cocos2d::CCMenu::create();
	this->addChild(this->quickSettingsMenu);

	this->handleOptionsLayers();

	float xPosButtonsToggles = Mod::get()->getSavedValue<bool>("pos-switch-buttons") == 1 ? 196.f : 250.f;

	float xPosRES[6] = { xPosButtonsToggles, xPosButtonsToggles, xPosButtonsToggles,
						 xPosButtonsToggles - 90.f, xPosButtonsToggles - 90.f, xPosButtonsToggles - 90.f };
	float yPosRES[6] = { 140.f, 120.f, 100.f, 140.f, 120.f, 100.f };

	for (size_t i = 0; i < 6; i++) {
		int posA = BetterPauseManager::sharedState()->posQuickA[i];

		if (posA == -1 || (posA < -1 && (-posA - 2 < 0 || -posA - 2 >= quickSettingsEnabledG.size()))) {
			continue;
		}
		else if (posA >= 0 && (posA >= BetterPause::quickSettingsKeys.size())) {
			continue;
		}

		float xPos = xPosRES[i];
		float yPos = yPosRES[i];

		if (posA < -1) {
			if (posA == -11) {
				createToggleButton((cocos2d::SEL_MenuHandler)&BetterPause::onPracticeMusicSyncRedirection,
					GameStatsManager::sharedState()->isItemEnabled(UnlockType::GJItem, 0x11),
					quickSettingsMenu, "Practice Music Sync", { xPos, yPos }, 0.25f, true, posA);
			}
			else {
				createToggleButton((cocos2d::SEL_MenuHandler)&BetterPause::onRedirectionToggle,
					quickSettingsEnabledG[-posA - 2], quickSettingsMenu,
					quickSettingsNamesG[-posA - 2], { xPos, yPos }, 0.25f, true, posA);
			}
		}
		else {
			createToggleButtonWithGameVariable(BetterPause::quickSettingsKeys[posA].c_str(), quickSettingsMenu,
				BetterPause::quickSettingsNames[posA].c_str(), { xPos, yPos },
				0.25f, true);
		}
	}

	this->clearQuickSettings();
}


void BetterPause::createQuestMenu() {

	questMenu = ChallengesPage::create();
	questMenu->setID("quest-menu");
	this->addChild(questMenu);

	questMenu->m_mainLayer->setScale(0.7f);
	questMenu->m_mainLayer->setAnchorPoint({ 1.f, 0.f });
	questMenu->setOpacity(0.f);
	questMenu->m_mainLayer->setPosition({ ((Utils::WinSize().width / 2.f) * questMenu->m_mainLayer->getScale()) - 150.f, 0.f });


	questMenu->setKeyboardEnabled(false);
	questMenu->unregisterScriptKeypadHandler();
	questMenu->setTouchEnabled(false);
	questMenu->setKeypadEnabled(false);
	questMenu->setTouchPriority(-10);
	questMenu->unregisterScriptTouchHandler();
	Utils::shareDirectorA()->getTouchDispatcher()->unregisterForcePrio(this->questMenu);

	for (size_t i = 0; i < questMenu->m_mainLayer->getChildrenCount(); i++)
	{
		auto node = reinterpret_cast<cocos2d::CCNode*>(questMenu->m_mainLayer->getChildren()->objectAtIndex(i));
		if (!typeinfo_cast<ChallengeNode*>(node) && !typeinfo_cast<cocos2d::CCLabelBMFont*>(node) &&
			!typeinfo_cast<LoadingCircle*>(node))
		{
			node->setVisible(false);
		}
	}
}


void BetterPause::createMainButtonsMenu() {
	cocos2d::CCSize LAYER_SIZE = { 50.f, 50.f };
	float totalHeight = 0.0f;

	layerMenuScrollButtons = cocos2d::CCLayerColor::create();
	layerMenuScrollButtons->setPosition({ 0.f, 0.f });
	layerMenuScrollButtons->setContentSize({ 45.f, 180.f });
  layerMenuScrollButtons->setID("layer-menu-scroll-buttons");
	this->addChild(layerMenuScrollButtons);

	buttonsList = ScrollLayer::create(LAYER_SIZE);
	buttonsList->setPosition({ 20.f, 115.f });
	buttonsList->setContentSize({ 45.f, 180.f });
	buttonsList->setTouchEnabled(true);
	buttonsList->setID("button-list-betterPause");
	layerMenuScrollButtons->addChild(buttonsList);

	auto createButtonMenu = [&](CCMenuItemSpriteExtra* button) {
		auto ccnode = cocos2d::CCNode::create();
		ccnode->setVisible(true);
		auto menu = cocos2d::CCMenu::create();
		menu->setVisible(true);
		ccnode->setPosition({ 0.f, -totalHeight });
		menu->setPosition(0.f, 0.f);
		menu->addChild(button);
		ccnode->addChild(menu);
		buttonsList->m_contentLayer->addChild(ccnode);
		button->setPosition(20.f, 20.f);
		ccnode->setAnchorPoint({ 0.f, 0.5f });
		ccnode->setContentSize(button->getContentSize());

		};

	auto resumeButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
	resumeButtonImage->setScale(0.5f);
	auto resumeButton = CCMenuItemSpriteExtra::create(resumeButtonImage, pauseLayer, (cocos2d::SEL_MenuHandler)&PauseLayer::onResume);
	totalHeight += resumeButtonImage->getContentSize().height - 36.f;
	createButtonMenu(resumeButton);

	auto replayButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_replayBtn_001.png");
	replayButtonImage->setScale(0.5f);
	auto replayButton = CCMenuItemSpriteExtra::create(replayButtonImage, pauseLayer, (cocos2d::SEL_MenuHandler)&PauseLayer::onRestart);
	totalHeight += replayButtonImage->getContentSize().height - 20.f;
	createButtonMenu(replayButton);

	if (Utils::getplayLayerA()->m_level->isPlatformer()) {
		auto replayFullButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_replayFullBtn_001.png");
		replayFullButtonImage->setScale(0.5f);
		auto replayFullButton = CCMenuItemSpriteExtra::create(replayFullButtonImage, pauseLayer, (cocos2d::SEL_MenuHandler)&PauseLayer::onRestartFull);
		totalHeight += replayFullButtonImage->getContentSize().height - 24.f;
		createButtonMenu(replayFullButton);
	}

	auto practiceButtonImage = cocos2d::CCSprite::createWithSpriteFrameName(Utils::getplayLayerA()->m_isPracticeMode ? "GJ_normalBtn_001.png" : "GJ_practiceBtn_001.png");
	practiceButtonImage->setScale(0.5f);
	auto practiceButton = CCMenuItemSpriteExtra::create(practiceButtonImage, pauseLayer, (Utils::getplayLayerA()->m_isPracticeMode ? (cocos2d::SEL_MenuHandler)&PauseLayer::onNormalMode : (cocos2d::SEL_MenuHandler)&PauseLayer::onPracticeMode));
	totalHeight += practiceButtonImage->getContentSize().height - 24.f;
	createButtonMenu(practiceButton);

	auto quitButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_menuBtn_001.png");
	quitButtonImage->setScale(0.5f);
	auto quitButton = CCMenuItemSpriteExtra::create(quitButtonImage, pauseLayer, (cocos2d::SEL_MenuHandler)&PauseLayer::tryQuit);
	totalHeight += quitButtonImage->getContentSize().height - 24.f;
	createButtonMenu(quitButton);

	if (Utils::getplayLayerA()->m_level->m_levelType == GJLevelType::Editor || Mod::get()->getSettingValue<bool>("level-editor-hack")) {
		auto editButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_editBtn_001.png");
		editButtonImage->setScale(0.38f);
		auto editButton = CCMenuItemSpriteExtra::create(editButtonImage, this, (cocos2d::SEL_MenuHandler)&BetterPause::onEditorHack);
		totalHeight += editButtonImage->getContentSize().height - 42.f;
		createButtonMenu(editButton);
	}

	if (Mod::get()->getSettingValue<bool>("show-comment-button")) {
		auto showCommentsImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_chatBtn_001.png");
		showCommentsImage->setScale(0.6f);
		auto showCommentsButton = CCMenuItemSpriteExtra::create(showCommentsImage, pauseLayer, (cocos2d::SEL_MenuHandler)&BetterPause::onLevelInfoLayer);
		totalHeight += showCommentsImage->getContentSize().height - 8.f;
		createButtonMenu(showCommentsButton);
	}


	std::vector<CCMenuItemSpriteExtra*> menuButtonsDetected = {};
	std::vector<std::string> buttonIds{ "edit-button", "full-restart-button", "practice-button",
										"play-button", "exit-button", "retry-button", "options-button" };
	this->findButtonsRecursively(this->pauseLayer, buttonIds, menuButtonsDetected);

	for (auto buttonExt : menuButtonsDetected) {
		if (buttonExt) {
			typeinfo_cast<CCSprite*>(buttonExt->getNormalImage())->setScale(0.6f);
			if (!menuButtonsDetected.empty()) {
				totalHeight += (buttonExt->getNormalImage()->getContentSize().height + menuButtonsDetected.back()->getContentSize().height) / 2 + 2.f;
			}
			else {
				totalHeight += buttonExt->getNormalImage()->getContentSize().height + 2.f;
			}
			createButtonMenu(buttonExt);
		}
	}

	auto allChildrens = buttonsList->m_contentLayer->getChildren();
	CCObject* node;
	CCARRAY_FOREACH(allChildrens, node) {
		auto child = dynamic_cast<CCNode*>(node);
		child->setPositionY(child->getPositionY() + totalHeight + 25.f);
	}

	if (totalHeight < LAYER_SIZE.height) {
		totalHeight = LAYER_SIZE.height;
	}

	buttonsList->m_contentLayer->setContentSize({ LAYER_SIZE.width, totalHeight });
	layerMenuScrollButtons->setContentSize({ LAYER_SIZE.width, totalHeight });
	buttonsList->moveToTop();

	float xContentSize = 50.f;

#ifdef GEODE_IS_ANDROID
	xContentSize = 80.0f;
#endif
	buttonsList->setContentSize({ xContentSize, 180.f });

	totalButtonsListHeight = buttonsList->m_contentLayer->getPositionY();
}

void BetterPause::createSecondaryButtonsMenu() {
	secondaryMenuButtons = cocos2d::CCMenu::create();
	secondaryMenuButtons->setPosition({ 40.f, 42.f });
	this->addChild(secondaryMenuButtons);

	auto visibleButtonImage = cocos2d::CCSprite::create("BE_eye-on-btn.png"_spr);

	if (!visibleButtonImage)
	{
		visibleButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_reportBtn_001.png");
		this->areTexturesLoaded = false;
	}
	visibleButtonImage->setScale(0.6f);
	this->visibleButton = CCMenuItemSpriteExtra::create(visibleButtonImage, this, (cocos2d::SEL_MenuHandler)&BetterPause::onHide);
	secondaryMenuButtons->addChild(this->visibleButton, -5);

	auto settingsButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
	settingsButtonImage->setScale(0.6f);
	auto settingsButton = CCMenuItemSpriteExtra::create(settingsButtonImage, this, (cocos2d::SEL_MenuHandler)&BetterPause::onOptionsLayer);
	secondaryMenuButtons->addChild(settingsButton);

	auto firstButtonRect = visibleButton->boundingBox();
	settingsButton->setPositionY(firstButtonRect.getMaxY() + 22.f);


	scrollUpButtonSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_upBtn_001.png");
	scrollUpButtonSprite->setScaleX(2.f);
	scrollUpButtonSprite->setScaleY(1.4f);
	scrollUpButton = CCMenuItemSpriteExtra::create(scrollUpButtonSprite, this, (cocos2d::SEL_MenuHandler)&BetterPause::onScrollUpButton);
	scrollUpButton->setPosition({ 0.f, 263.f });
	secondaryMenuButtons->addChild(scrollUpButton);

	scrollDownButtonSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_downBtn_001.png");
	scrollDownButtonSprite->setScaleX(2.f);
	scrollDownButtonSprite->setScaleY(1.4f);
	scrollDownButton = CCMenuItemSpriteExtra::create(scrollDownButtonSprite, this, (cocos2d::SEL_MenuHandler)&BetterPause::onScrollDownButton);
	scrollDownButton->setPosition({ 0.f, 65.f });
	secondaryMenuButtons->addChild(scrollDownButton);

	if (!Mod::get()->getSettingValue<bool>("disable-arrow-blink")) {
		scrollUpButtonSprite->runAction(cocos2d::CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.4f, 50), CCFadeTo::create(0.4f, 255), nullptr)));
		scrollDownButtonSprite->runAction(cocos2d::CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.4f, 50), CCFadeTo::create(0.4f, 255), nullptr)));
	}

	if (Mod::get()->getSettingValue<bool>("disable-arrow-buttons")) {
		scrollUpButton->setVisible(false);
		scrollDownButton->setVisible(false);
	}

	this->updateButtons();
}

void BetterPause::createAudioControls() {

	auto musicValue = 0.f;
	auto sfxValue = 0.f;

#ifdef GEODE_IS_WINDOWS
	musicValue = Utils::from<float>(Utils::shareFMOD(), 0x168);
#endif
#ifdef  GEODE_IS_ANDROID
	musicValue = Utils::shareFMOD()->getBackgroundMusicVolume();
#endif

#ifdef GEODE_IS_WINDOWS
	sfxValue = Utils::from<float>(Utils::shareFMOD(), 0x16C);
#endif
#ifdef  GEODE_IS_ANDROID
	sfxValue = Utils::shareFMOD()->getEffectsVolume();
#endif

	sliderPlusMusic = SliderPlus::create("Music", this, (cocos2d::SEL_MenuHandler)&PauseLayer::musicSliderChanged,
		nullptr, nullptr, musicValue);
	sliderPlusMusic->setPosition({ Utils::WinSize().width, Utils::WinSize().height - 60.f });
	sliderPlusMusic->setAnchorPoint({ 1.f, 0.f });
	sliderPlusMusic->setScale(0.9f);
	this->addChild(sliderPlusMusic);

	sliderPlusSFX = SliderPlus::create("SFX", this, (cocos2d::SEL_MenuHandler)&PauseLayer::sfxSliderChanged,
		nullptr, nullptr, sfxValue);
	sliderPlusSFX->setPosition({ Utils::WinSize().width, Utils::WinSize().height - 90.f });
	sliderPlusSFX->setAnchorPoint({ 1.f, 0.f });
	sliderPlusSFX->setScale(0.9f);
	this->addChild(sliderPlusSFX);
}

void BetterPause::createLabels() {
	auto levelName = Utils::getplayLayerA()->m_level->m_levelName;
	auto creatorName = Utils::getplayLayerA()->m_level->m_creatorName;
	auto levelType = Utils::getplayLayerA()->m_level->m_levelType;
	auto isPracticeMode = Utils::getplayLayerA()->m_isPracticeMode;

	levelNameLabel = cocos2d::CCLabelBMFont::create(levelName.c_str(), "goldFont.fnt");
	levelNameLabel->limitLabelWidth(150.f, 1.f, 0.1f);
	levelNameLabel->setAnchorPoint({ 0.f, 0.5f });
	levelNameLabel->setPosition({ 86.f, Utils::WinSize().height - 30.f });
	levelNameLabel->setID("level-name");
	this->addChild(levelNameLabel);

	if (!Mod::get()->getSettingValue<bool>("disable-creator-label")) {
		std::string formattedCreatorName = Utils::getFormattedCreatorName(creatorName, levelType);
		creatorNameLabel = cocos2d::CCLabelBMFont::create(formattedCreatorName.c_str(), "bigFont.fnt");
		creatorNameLabel->limitLabelWidth(150.f, 0.3f, 0.1f);
		creatorNameLabel->setAnchorPoint({ 0.f, 0.5f });
		creatorNameLabel->setScale(0.3f);
		creatorNameLabel->setPosition({ 87.f, Utils::WinSize().height - 18.f });
		creatorNameLabel->setID("creator-name");
		this->addChild(creatorNameLabel);
	}

	levelTypeLabel = cocos2d::CCLabelBMFont::create(Utils::getNameLevelType(Utils::getplayLayerA()->m_level->m_levelType).c_str(), "bigFont.fnt");
	levelTypeLabel->setScale(0.3f);
	levelTypeLabel->setPosition({ 86.f, Utils::WinSize().height - 45.f });
	levelTypeLabel->setAnchorPoint({ 0.f, 1.f });
	levelTypeLabel->setID("level-type");
	this->addChild(levelTypeLabel);

	levelStatusLabel = cocos2d::CCLabelBMFont::create(Utils::getplayLayerA()->m_isPracticeMode ? "Practice Mode" : "Normal Mode", "bigFont.fnt");
	levelStatusLabel->setScale(0.3f);
	levelStatusLabel->setColor(isPracticeMode ? cocos2d::ccColor3B{ 0, 255, 255 } : cocos2d::ccColor3B{ 0, 255, 0 });
	levelStatusLabel->setPosition({ 195.f, Utils::WinSize().height - 45.f });
	levelStatusLabel->setAnchorPoint({ 0.f, 1.f });
	levelStatusLabel->setID("level-status");
	this->addChild(levelStatusLabel);

	std::string attemptColor;
	std::string timeLabelFormat;

	if (Utils::getplayLayerA()->m_isPracticeMode) {
		attemptColor = "<cj>";
		timeLabelFormat = Utils::getplayLayerA()->m_level->isPlatformer() ? "Time Total: <cj>%02d:%02d</c>" : "Time: <cj>%02d:%02d</c>";
	}
	else {
		attemptColor = "<cg>";
		timeLabelFormat = Utils::getplayLayerA()->m_level->isPlatformer() ? "Time Total: <cg>%02d:%02d</c>" : "Time: <cg>%02d:%02d</c>";
	}

	std::cout << Utils::getTotalAttemptsPlayLayer() << std::endl;

	auto currentAttemptText = gd::string(cocos2d::CCString::createWithFormat("Attempt: %s%i</c>", attemptColor.c_str(), Utils::getTotalAttemptsPlayLayer())->getCString());
	currentAttemptLabel = TextArea::create(currentAttemptText, "bigFont.fnt", 0.3f, 100.f, { 0.f, 1.f }, 0.f, false);
	currentAttemptLabel->setPosition({ 86.f + (currentAttemptLabel->getContentSize().width / 2), Utils::WinSize().height - 60.f });
	currentAttemptLabel->setID("current-attempt");
	this->addChild(currentAttemptLabel);

	int totalSeconds = Utils::getTotalSecondsPlayLayer();
	if (Utils::getplayLayerA()->m_level->isPlatformer()) {
		totalSeconds = BetterPause::totalLevelTimeBackup;
	}
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;

	auto currentTimeText = gd::string(cocos2d::CCString::createWithFormat(timeLabelFormat.c_str(), minutes, seconds)->getCString());
	currentTimeLabel = TextArea::create(currentTimeText, "bigFont.fnt", 0.3f, 100.f, { 0.f, 1.f }, 0.f, false);
	currentTimeLabel->setPosition({ 195.f + (currentTimeLabel->getContentSize().width / 2), Utils::WinSize().height - 60.f });
	currentTimeLabel->setID("current-time");
	this->addChild(currentTimeLabel);
}

void BetterPause::createBars() {
	createAndSetupBar(normalBarPercentage, { 0, 255, 0 }, !Utils::getplayLayerA()->m_isPracticeMode, Utils::getPercentageNowFix(), Utils::getplayLayerA()->m_level->m_normalPercent, { 86.f, Utils::WinSize().height - 90.f }, "normal-bar");
	createAndSetupBar(practiceBarPercentage, { 0, 255, 255 }, Utils::getplayLayerA()->m_isPracticeMode, Utils::getPercentageNowFix(), Utils::getplayLayerA()->m_level->m_practicePercent, { 86.f, Utils::WinSize().height - 125.f }, "practice-bar");
}

void BetterPause::createAndSetupBar(BarBetterShow*& bar, const cocos2d::ccColor3B& color, bool isVisible, float currentPercentage, float targetPercentage, const cocos2d::CCPoint& position, std::string id) {
	bar = BarBetterShow::create(color, isVisible, isVisible, currentPercentage, targetPercentage);
	bar->setPosition(position);
	bar->setScale(0.5f);
	bar->m_pBarBase->setVisible(true);
	bar->m_pBarBase->setOpacity(Utils::convertOpacitySimplf(0.2f));
	if (id != "not-a-string") bar->setID(id);
	this->addChild(bar);
}


void BetterPause::onLevelInfoLayer(cocos2d::CCObject* pSender) {
	if (Utils::getplayLayerA()) {
		auto infoLayer = InfoLayer::create(Utils::getplayLayerA()->m_level, nullptr, nullptr);
		infoLayer->show();
	}
}

void BetterPause::musicSliderChanged(cocos2d::CCObject* pSender) {
	auto valueV = this->musicSlider->getThumb()->getValue();
	this->pauseLayer->musicSliderChanged(pSender);
	this->musicVolumeSettingsLabel->setString(
		cocos2d::CCString::createWithFormat("%i%%", static_cast<int>(valueV * 100))->getCString());

	return;
}

void BetterPause::sfxSliderChanged(cocos2d::CCObject* pSender) {
	auto valueV = this->sfxSlider->getThumb()->getValue();
	this->pauseLayer->sfxSliderChanged(pSender);
	this->sfxVolumeSettingsLabel->setString(
		cocos2d::CCString::createWithFormat("%i%%", static_cast<int>(valueV * 100))->getCString());

	return;
}

void BetterPause::onSetMusicVolume(cocos2d::CCObject* pSender) {
	auto popup = SetVolumenPopup::create(this->musicSlider);
	popup->setID("popup-betterpause");
	popup->show();
}

void BetterPause::onSetSfxVolume(cocos2d::CCObject* pSender) {
	auto popup = SetVolumenPopup::create(this->sfxSlider);
	popup->setID("popup-betterpause");
	popup->show();
}

void BetterPause::createToggleButtonWithGameVariable(const char* key, cocos2d::CCMenu* menu, std::string caption, cocos2d::CCPoint pos, float size, bool twoColumns, std::string id) {
	auto toggleButton = CCMenuItemToggler::createWithStandardSprites(this, (cocos2d::SEL_MenuHandler)&BetterPause::onToggleWithGameVariable, size + 0.2f);
	toggleButton->toggle(Utils::shareManager()->getGameVariable(key));
	toggleButton->setTag(std::stoi(key));
	toggleButton->setPosition(menu->convertToNodeSpace(pos));
	if (menu)
	{
		menu->addChild(toggleButton);
	}

	auto text = cocos2d::CCLabelBMFont::create(caption.c_str(), "bigFont.fnt");
	float labelWidth = twoColumns ? (250.f * size) : (500.f * size);
	text->limitLabelWidth(labelWidth, size, 0.1f);
	text->setAlignment(cocos2d::kCCTextAlignmentRight);
	text->setPosition({ pos.x - 15.f, pos.y });
	text->setAnchorPoint({ 1.f, 0.5f });
	if (id != "not-a-string") text->setID(id);
	this->addChild(text);
}

void BetterPause::onToggleWithGameVariable(cocos2d::CCObject* pSender) {
	int tagValue = static_cast<int>(reinterpret_cast<cocos2d::CCNode*>(pSender)->getTag());
	std::stringstream ss;
	ss << std::setw(4) << std::setfill('0') << tagValue;
	std::string keyCode = ss.str();
	Utils::shareManager()->setGameVariable(keyCode.c_str(), !Utils::shareManager()->getGameVariable(keyCode.c_str()));
}

void BetterPause::createToggleButton(cocos2d::SEL_MenuHandler callback, bool on,
	cocos2d::CCMenu* menu, std::string caption, cocos2d::CCPoint pos, float size,
	bool twoColumns, int tag, std::string id) {
	auto toggleButton = CCMenuItemToggler::createWithStandardSprites(this, callback, size + 0.2f);
	toggleButton->toggle(on);
	toggleButton->setPosition(menu->convertToNodeSpace(pos));
	if (tag != -1) {
		toggleButton->setTag(tag);
	}

	if (menu)
	{
		menu->addChild(toggleButton);
	}

	auto text = cocos2d::CCLabelBMFont::create(caption.c_str(), "bigFont.fnt");
	float labelWidth = twoColumns ? (250.f * size) : (500.f * size);
	text->limitLabelWidth(labelWidth, size, 0.1f);
	text->setAlignment(cocos2d::kCCTextAlignmentRight);
	text->setPosition({ pos.x - 15.f, pos.y });
	text->setAnchorPoint({ 1.f, 0.5f });
	if (id != "not-a-string") text->setID(id);
	this->addChild(text);

	if (std::string(caption.c_str()) == "Practice Music Sync" && !GameStatsManager::sharedState()->isItemUnlocked(UnlockType::GJItem, 0x11)) {
		reinterpret_cast<CCSprite*>(reinterpret_cast<CCMenuItemSpriteExtra*>(toggleButton->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0))->setColor({ 150,150,150 });
		reinterpret_cast<CCSprite*>(reinterpret_cast<CCMenuItemSpriteExtra*>(toggleButton->getChildren()->objectAtIndex(1))->getChildren()->objectAtIndex(0))->setColor({ 150,150,150 });
		text->setColor({ 150,150,150 });
	}

}

void BetterPause::onOptionsLayer(cocos2d::CCObject* pSender) {
	auto popup = MoreOptionsPauseLayer::create(this);
	popup->setID("popup-betterpause");
	popup->show();
}

void BetterPause::onHide(cocos2d::CCObject* pSender) {
	this->isHidden = !this->isHidden;
	pauseLayer->setOpacity(this->isHidden ? 0 : 75);

	for (size_t i = 0; i < this->getChildrenCount(); i++)
	{
		auto node = reinterpret_cast<cocos2d::CCNode*>(this->getChildren()->objectAtIndex(i));
		node->setVisible(!this->isHidden);
	}

	for (size_t i = 0; i < pauseLayer->getChildrenCount(); i++)
	{
		auto node = reinterpret_cast<cocos2d::CCNode*>(pauseLayer->getChildren()->objectAtIndex(i));
		node->setVisible(false);
	}

	if (!this->isHidden)
	{
		auto Fnode = reinterpret_cast<cocos2d::CCNode*>(pauseLayer->getChildren()->objectAtIndex(0));
		Fnode->setVisible(true);
	}

	this->setVisible(true);
	this->secondaryMenuButtons->setVisible(true);

	for (size_t i = 0; i < this->secondaryMenuButtons->getChildrenCount(); i++)
	{
		auto node = reinterpret_cast<cocos2d::CCNode*>(this->secondaryMenuButtons->getChildren()->objectAtIndex(i));
		node->setVisible(!this->isHidden);
	}

	if (areTexturesLoaded)
	{
		typeinfo_cast<cocos2d::CCSprite*>(visibleButton->getNormalImage())->initWithFile(this->isHidden ? "BE_eye-off-btn.png"_spr : "BE_eye-on-btn.png"_spr);
	}
	visibleButton->setVisible(true);
	visibleButton->setOpacity(this->isHidden ? 50 : 255);
	this->updateButtons();
}

void BetterPause::onEditorHack(cocos2d::CCObject* pSender) {

	GJLevelType origType = Utils::getplayLayerA()->m_level->m_levelType;
	Utils::getplayLayerA()->m_level->m_levelType = GJLevelType::Editor;
	this->pauseLayer->onEdit(pSender);
	if (Utils::getplayLayerA()) {
		Utils::getplayLayerA()->m_level->m_levelType = origType;
	}
}

void BetterPause::scrollWheel(float x, float y) {
	CCLayer::scrollWheel(x, y);

	if (Mod::get()->getSettingValue<int64_t>("type-pause") != 1) {
		return;
	}
	buttonsList->scrollWheel(x, y);

	this->updateButtons();

}

void BetterPause::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent) {
	CCLayer::ccTouchMoved(pTouch, pEvent);
	this->updateButtons();
}

void BetterPause::onRedirectionToggle(cocos2d::CCObject* pSender) {

	auto toggleButton = reinterpret_cast<CCMenuItemToggler*>(pSender);
	auto gameOptionsLayer = GameOptionsLayer::create(Utils::getplayLayerA());
	gameOptionsLayer->show();
	auto toggleTag = toggleButton->getTag();

	if ((toggleTag != -1)) {
#ifdef GEODE_IS_WINDOWS
		gameOptionsLayer->didToggle_(-toggleTag - 1);
#else
		gameOptionsLayer->didToggle(-toggleTag - 1);
#endif
	}

	gameOptionsLayer->onClose(nullptr);

	this->clearQuickSettings();
}

void BetterPause::clearQuickSettings() {
	BetterPause::quickSettingsNamesG.clear();
	BetterPause::quickSettingsNumbersG.clear();
	BetterPause::quickSettingsEnabledG.clear();
	BetterPause::quickSettingsNames.clear();
	BetterPause::quickSettingsDescriptions.clear();
	BetterPause::quickSettingsKeys.clear();
}

void BetterPause::onPracticeMusicSyncRedirection(cocos2d::CCObject* pSender) {
	auto toggleButton = reinterpret_cast<CCMenuItemToggler*>(pSender);
	auto gameOptionsLayer = GameOptionsLayer::create(Utils::getplayLayerA());
	gameOptionsLayer->show();
	gameOptionsLayer->onPracticeMusicSync(pSender);
	gameOptionsLayer->onClose(nullptr);

	if (!GameStatsManager::sharedState()->isItemUnlocked(UnlockType::GJItem, 0x11)) {
		toggleButton->m_offButton->setVisible(true);
		toggleButton->m_onButton->setVisible(false);
		toggleButton->toggle(true);
	}

	this->clearQuickSettings();
}

void BetterPause::findButtonsRecursively(CCNode* node, std::vector<std::string>& buttonIds, std::vector<CCMenuItemSpriteExtra*>& buttonsExternals) {
	if (!node)
		return;


	auto button = dynamic_cast<CCMenuItemSpriteExtra*>(node);
	if (button) {
		auto id = button->getID();
		if (std::find(buttonIds.begin(), buttonIds.end(), id) == buttonIds.end()) {
			buttonsExternals.push_back(button);
		}
	}

	for (size_t i = 0; i < node->getChildrenCount(); i++)
	{
		BetterPause::findButtonsRecursively(typeinfo_cast<CCNode*>(node->getChildren()->objectAtIndex(i)), buttonIds, buttonsExternals);
	}
}


void BetterPause::onScrollUpButton(cocos2d::CCObject* sender) {
	float scrollAmount = buttonsList->m_contentLayer->getPositionY() - static_cast<float>(Mod::get()->getSettingValue<double>("amount-scroll-button"));
	float newContentPosY = 0.f;

	if (scrollAmount < totalButtonsListHeight) {
		newContentPosY = totalButtonsListHeight;
	}
	else {
		newContentPosY = scrollAmount;
	}

	buttonsList->m_contentLayer->setPositionY(newContentPosY);
	this->updateButtons();
}

void BetterPause::onScrollDownButton(cocos2d::CCObject* sender) {

	float scrollAmount = buttonsList->m_contentLayer->getPositionY() + static_cast<float>(Mod::get()->getSettingValue<double>("amount-scroll-button"));
	float newContentPosY = 0.f;

	if (scrollAmount > 0.f) {
		newContentPosY = 0.f;
	}
	else {
		newContentPosY = scrollAmount;
	}

	buttonsList->m_contentLayer->setPositionY(newContentPosY);
	this->updateButtons();
}

void BetterPause::adjustLayerForAspectRatio() {

	auto director = cocos2d::CCDirector::sharedDirector();
	auto size = director->getWinSize();
	float aspectRatio = size.width / size.height;
	float aspectRatioReverse = size.height / size.width;
	auto respect = aspectRatio < 1.7;

	auto scale_value = [](float value) {
		if (value <= 0.2) {
			return 0.1f;
		}
		else if (value <= 2.0) {
			return 0.2f;
		}
		else if (value <= 20.f) {
			return 0.3f;
		}
		else {
			return 0.4f;
		}
		};

	if (aspectRatio > (570.f / 320.f)) {
		float displacementX = (size.width / 2) - (570.f / 2);
		auto allChildrens = this->getChildren();
		CCObject* node;
		CCARRAY_FOREACH(allChildrens, node) {
			auto child = dynamic_cast<CCNode*>(node);
			if (auto child2 = dynamic_cast<ChallengesPage*>(node); child2) {
				child = child2->m_mainLayer;
				float originalX = child->getPositionX();
				float newX = originalX - displacementX;
				child->setPositionX(newX);
				continue;
			}
			float originalX = child->getPositionX();
			float newX = originalX + displacementX;
			if (originalX > size.width / 2) {
				newX = originalX - displacementX;
			}
			child->setPositionX(newX);
		}
	}
	else if (aspectRatio < (570.f / 320.f)) {
		float displacementY = (size.height / 2) - (320.f / 2);
		auto allChildrens = this->getChildren();
		CCObject* node;
		CCARRAY_FOREACH(allChildrens, node) {
			auto child = dynamic_cast<CCNode*>(node);
			if (auto child2 = dynamic_cast<ChallengesPage*>(node); child2) {
				child = child2->m_mainLayer;
				float originalY = child->getPositionY();
				float newY = originalY + (displacementY / 2);
				child->setPositionY(newY);
				continue;
			}
			if (layerMenuScrollButtons == child) {
				child = buttonsList;
				float originalY = child->getPositionY();
				float newY = originalY + displacementY;
				child->setPositionY(newY);
				continue;
			}
			float originalY = child->getPositionY();
			float newY = originalY + displacementY;
			if (originalY > size.height / 2) {
				newY = originalY - displacementY;
			}
			child->setPositionY(newY);
		}
	}


	if (respect) {
		this->volumeSettingsLabel->setPositionX((this->volumeSettingsLabel->getPositionX() + 40.f));
		this->questsLabel->setPositionX(this->questsLabel->getPositionX() + 40.f);
		this->sliderPlusMusic->setScale(this->sliderPlusMusic->getScale() - scale_value(1.7 - round(aspectRatio * 10.f) / 10.f));
		this->sliderPlusSFX->setScale(this->sliderPlusSFX->getScale() - scale_value(1.7 - round(aspectRatio * 10.f) / 10.f));
		this->questMenu->m_mainLayer->setScale(this->questMenu->m_mainLayer->getScale() - (scale_value(1.7 - round(aspectRatio * 10.f) / 10.f) - 0.05f));
	}

	return;

	if (Utils::isRoundAspectRatio(534.0f / 320.0f)) {
		this->musicSlider->setScale(0.7f);
		this->musicSlider->setPosition({ Utils::WinSize().width - 143.f, Utils::WinSize().height - 106.f });
		this->sfxSlider->setScale(0.7f);
		this->sfxSlider->setPosition({ Utils::WinSize().width - 143.f, Utils::WinSize().height - 137.f });
		this->musicVolumeLabel->setPosition({ Utils::WinSize().width - 240.f, Utils::WinSize().height - 57.f });
		this->sfxVolumeSettingsLabel->setPosition({ Utils::WinSize().width - 236.f, Utils::WinSize().height - 87.f });
	}



	if (Utils::isSpecificAspectRatio(4.f / 3.f) || Utils::isSpecificAspectRatio(3.f / 2.f) || Utils::isSpecificAspectRatio(5.f / 4.f)
		|| (Utils::isRoundAspectRatio(4.f / 3.f) || Utils::isRoundAspectRatio(3.f / 2.f) || Utils::isRoundAspectRatio(5.f / 4.f)))
	{

		this->musicVolumeLabel->setScale(0.4f);
		musicVolumeLabel->setPositionX(musicVolumeLabel->getPositionX() + 70.f);
		this->sfxVolumeSettingsLabel->setScale(0.4f);
		sfxVolumeSettingsLabel->setPositionX(sfxVolumeSettingsLabel->getPositionX() + 70.f);
		this->musicSlider->setScale(0.5f);
		musicSlider->setPositionX(musicSlider->getPositionX() + 35.f);
		this->sfxSlider->setScale(0.5f);
		sfxSlider->setPositionX(sfxSlider->getPositionX() + 35.f);
		musicSlider->setPositionY(musicSlider->getPositionY() - 58.f);
		sfxSlider->setPositionY(sfxSlider->getPositionY() - 58.f);
		this->volumeSettingsLabel->setScale(0.4f);
		this->questsLabel->setScale(0.4f);
		if (questMenu)
		{
			questMenu->setScale(0.5f);
			questMenu->setPositionX(questMenu->getPositionX() + 35.f);
		}
		else if (this->questButtonAlt)
		{
			this->questButtonAlt->setScale(0.5f);
		}

		this->musicVolumeLabel->setPosition({ Utils::WinSize().width - 190.f, Utils::WinSize().height - 57.f });
		this->sfxVolumeSettingsLabel->setPosition({ Utils::WinSize().width - 188.f, Utils::WinSize().height - 87.f });
		this->musicSlider->setPosition({ Utils::WinSize().width - 112.f, Utils::WinSize().height - 148.f });
		this->sfxSlider->setPosition({ Utils::WinSize().width - 112.f, Utils::WinSize().height - 178.f });
		this->musicVolumeButton->setPosition({ Utils::WinSize().width - 32.f, Utils::WinSize().height - 57.f });
		this->sfxVolumeButton->setPosition({ Utils::WinSize().width - 32.f, Utils::WinSize().height - 87.f });
		this->volumeSettingsLabel->setPosition({ Utils::WinSize().width - 126.f, Utils::WinSize().height - 29.f });
		this->questsLabel->setPosition({ Utils::WinSize().width - 124.f, Utils::WinSize().height - 140.f });
		if (questMenu) {
			questMenu->setPosition({ 123.f, -50.f });
		}
		else if (this->questButtonAlt) {
			this->questButtonAlt->setPosition(Utils::WinSize().width - 210.f, 120.f);
		}
	}
	if (Utils::isSpecificAspectRatio(3.f / 2.f)) {
		this->musicSlider->setPositionY(this->musicSlider->getPositionY() + 10.f);
		this->sfxSlider->setPositionY(this->sfxSlider->getPositionY() + 10.f);
	}
	if (Utils::isSpecificAspectRatio(5.f / 4.f)) {
		this->musicSlider->setPositionY(this->musicSlider->getPositionY() - 6.f);
		this->sfxSlider->setPositionY(this->sfxSlider->getPositionY() - 6.f);
	}
}

void BetterPause::updateButtons() {

	if (typeMenuCreate != 1) {
		return;
	}


	if (!Mod::get()->getSettingValue<bool>("disable-arrow-buttons") && !this->isHidden) {
		if (totalButtonsListHeight != 0.f) {
			if (totalButtonsListHeight < buttonsList->m_contentLayer->getPositionY() &&
				0.f > buttonsList->m_contentLayer->getPositionY()) {
				this->scrollUpButton->setVisible(true);
				this->scrollDownButton->setVisible(true);
			}
			else if (buttonsList->m_contentLayer->getPositionY() >= 0.f) {
				this->scrollUpButton->setVisible(true);
				this->scrollDownButton->setVisible(false);
			}
			else if (buttonsList->m_contentLayer->getPositionY() <= totalButtonsListHeight) {
				this->scrollUpButton->setVisible(false);
				this->scrollDownButton->setVisible(true);
			}
		}
		else {
			this->scrollUpButton->setVisible(false);
			this->scrollDownButton->setVisible(false);
		}
	}


	auto allChildrens = buttonsList->m_contentLayer->getChildren();
	CCObject* node;
	CCARRAY_FOREACH(allChildrens, node) {
		auto child = dynamic_cast<CCNode*>(node);

		CCPoint nodeWorldPos = buttonsList->m_contentLayer->convertToWorldSpace(child->getPosition());

		float disappearY = buttonsList->getPositionY() + buttonsList->getContentSize().height;
		float disappearY2 = buttonsList->getPositionY() - buttonsList->getContentSize().height;

		if (nodeWorldPos.y > disappearY + child->getContentSize().height / 2) {
			child->setVisible(false);
		}

		if (nodeWorldPos.y < disappearY2 - child->getContentSize().height / 2) {
			child->setVisible(false);
		}

		float topCutOff = buttonsList->getPositionY() + buttonsList->getContentSize().height - (15.f * static_cast<float>(Mod::get()->getSettingValue<double>("tolerance-in-buttons-list")));
		float bottomCutOff = buttonsList->getPositionY() + (15.f * static_cast<float>(Mod::get()->getSettingValue<double>("tolerance-in-buttons-list")));

		if (nodeWorldPos.y > topCutOff || nodeWorldPos.y < bottomCutOff) {
			typeinfo_cast<cocos2d::CCMenu*>(child->getChildren()->objectAtIndex(0))->setEnabled(false);
		}
		else {
			typeinfo_cast<cocos2d::CCMenu*>(child->getChildren()->objectAtIndex(0))->setEnabled(true);
		}
	}

}

void BetterPause::createToggleButtonOldMethod(cocos2d::SEL_MenuHandler callback, bool on,
	cocos2d::CCMenu* menu, std::string caption, cocos2d::CCPoint pos, int tag) {

	auto toggleButton = CCMenuItemToggler::createWithStandardSprites(this, callback, 0.6f);
	toggleButton->toggle(on);
	toggleButton->setSizeMult(0.6f);
	toggleButton->setTag(tag);
	toggleButton->setPosition(menu->convertToNodeSpace(pos));
	if (menu) {
		menu->addChild(toggleButton);
	}

	auto text = cocos2d::CCLabelBMFont::create(caption.c_str(), "bigFont.fnt");
	text->limitLabelWidth(80.f, 0.4f, 0.1f);
	text->setAlignment(cocos2d::kCCTextAlignmentLeft);
	text->setPosition(pos);
	text->setPositionX(pos.x + 18.f);
	text->setAnchorPoint({ 0.f, 0.5f });
	this->addChild(text);
}

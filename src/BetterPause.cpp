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
		ret->scheduleUpdate();
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

	return true;
}


void BetterPause::update(float dt) {

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


	if (this->customSongWidget->m_artistLabel && this->customSongWidget->m_moreBtn) {
		CCPoint labelPos = this->customSongWidget->m_artistLabel->getPosition();
		auto labelWidth = this->customSongWidget->m_artistLabel->getContentSize().width * this->customSongWidget->m_artistLabel->getScale();

		auto menuItemX = labelPos.x + labelWidth - (Utils::WinSize().width / 2) + 30.f;

		this->customSongWidget->m_moreBtn->setPositionX(menuItemX);
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
	this->questsLabel->setPosition({ Utils::WinSize().width - 120.f, 200.f });
	this->questsLabel->setAnchorPoint({ 1.f, 0.f });
	this->addChild(this->questsLabel);

}

void BetterPause::handleOptionsLayers() {
	{
		auto gameOptionsLayer = GameOptionsLayer::create(Utils::getplayLayerA());
		gameOptionsLayer->onClose(nullptr);
		CC_SAFE_DELETE(gameOptionsLayer);
	}

	{
		auto moreOptionsLayer = MoreOptionsLayer::create();
		moreOptionsLayer->keyBackClicked();
		CC_SAFE_DELETE(moreOptionsLayer);
	}
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
		songInfoObject = LevelTools::getSongObject(zeroIdSong);
	}
	else {
		songInfoObject = SongInfoObject::create(levelSongID);
	}

	this->customSongWidget = CustomSongWidget::create(songInfoObject, 0, 0, 1, 1, levelSongID == 0, 0, 0);
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
	offsetUnkFloatCSW = 0x170;
#endif

	if (Utils::from<float>(songInfoObject, offsetUnkFloatCSW) != 0.f) {
		customSongWidget->onGetSongInfo(nullptr);
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
	questMenu->m_mainLayer->setPosition({ ((Utils::WinSize().width / 2.f) * questMenu->m_mainLayer->getScale()) - 150.f, 0.f});
	

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
	cocos2d::CCSize LAYER_SIZE = { 230.f, 180.f };
	float totalHeight = 0.0f;

	auto menuScrollButtons = cocos2d::CCMenu::create();
	menuScrollButtons->setVisible(true);
	menuScrollButtons->setPosition({ 0.f, 0.f });
	menuScrollButtons->setID("menu-scroll-buttons");
	this->addChild(menuScrollButtons);

	buttonsList = ScrollLayer::create(LAYER_SIZE);
	buttonsList->setPosition({ 20.f, Utils::WinSize().height - 205.f });
	buttonsList->setContentSize({ 45.f, 180.f });
	menuScrollButtons->addChild(buttonsList);

	auto createButtonMenu = [&](CCMenuItemSpriteExtra* button) {
		auto menu = cocos2d::CCMenu::create();
		menu->setVisible(true);
		menu->setPosition({ 20.f, -totalHeight });
		buttonsList->m_contentLayer->addChild(menu);
		menu->addChild(button);
		button->setPosition(0.f, 0.f);
	};

	auto resumeButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
	resumeButtonImage->setScale(0.5f);
	auto resumeButton = CCMenuItemSpriteExtra::create(resumeButtonImage, pauseLayer, (cocos2d::SEL_MenuHandler)&PauseLayer::onResume);
	totalHeight += resumeButtonImage->getContentSize().height - 36.f;
	createButtonMenu(resumeButton);

	auto replayButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_replayBtn_001.png");
	replayButtonImage->setScale(0.5f);
	auto replayButton = CCMenuItemSpriteExtra::create(replayButtonImage, pauseLayer, (cocos2d::SEL_MenuHandler)&PauseLayer::onRestart);
	totalHeight += replayButtonImage->getContentSize().height - 24.f;
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
			typeinfo_cast<CCSprite*>(buttonExt->getChildren()->objectAtIndex(0))->setScale(0.6f);
			if (!menuButtonsDetected.empty()) {
				totalHeight += (buttonExt->getContentSize().height + menuButtonsDetected.back()->getContentSize().height) / 2 + 2.f;
			}
			else {
				totalHeight += buttonExt->getContentSize().height + 2.f;
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
	scrollUpButton->setPosition({ 0.f, Utils::WinSize().height - 57.f });
	secondaryMenuButtons->addChild(scrollUpButton);

	scrollDownButtonSprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_downBtn_001.png");
	scrollDownButtonSprite->setScaleX(2.f);
	scrollDownButtonSprite->setScaleY(1.4f);
	scrollDownButton = CCMenuItemSpriteExtra::create(scrollDownButtonSprite, this, (cocos2d::SEL_MenuHandler)&BetterPause::onScrollDownButton);
	scrollDownButton->setPosition({ 0.f, Utils::WinSize().height - 255.f });
	secondaryMenuButtons->addChild(scrollDownButton);

	if (!Mod::get()->getSettingValue<bool>("disable-arrow-blink")) {
		scrollUpButton->runAction(cocos2d::CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.4f, 50), CCFadeTo::create(0.4f, 255), nullptr)));
		scrollDownButton->runAction(cocos2d::CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.4f, 50), CCFadeTo::create(0.4f, 255), nullptr)));

		//scrollUpButton->runAction(cocos2d::CCRepeatForever::create(CCSequence::create(CCMoveTo::create(1.0, { scrollUpButton->getPositionY(), scrollUpButton->getPositionY() + 10.f }), CCMoveTo::create(1.0, { scrollUpButton->getPositionY() + 10.f, scrollUpButton->getPositionY() }), nullptr)));
		//scrollDownButton->runAction(cocos2d::CCRepeatForever::create(CCSequence::create(CCMoveTo::create(1.0, { scrollDownButton->getPositionY(), scrollDownButton->getPositionY() + 10.f }), CCMoveTo::create(1.0, { scrollDownButton->getPositionY() + 10.f, scrollDownButton->getPositionY() }), nullptr)));
	}

	if (Mod::get()->getSettingValue<bool>("disable-arrow-buttons")) {
		scrollUpButton->setVisible(false);
		scrollDownButton->setVisible(false);
	}
	
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

	/*
#ifdef GEODE_IS_WINDOWS
	musicSlider->setValue(Utils::from<float>(Utils::shareFMOD(), 0x168));
#elif GEODE_IS_ANDROID
	musicSlider->setValue(Utils::shareFMOD()->getBackgroundMusicVolume());
#endif
	musicSlider->setScale(0.8f);
	musicSlider->setAnchorPoint({ 0.f, 0.f });
	musicSlider->setPosition({ Utils::WinSize().width - 155.f, Utils::WinSize().height - 90.f });
	this->addChild(musicSlider);
	

	sfxSlider = Slider::create(this, (cocos2d::SEL_MenuHandler)&BetterPause::sfxSliderChanged, 1.f);
#ifdef GEODE_IS_WINDOWS
	sfxSlider->setValue(Utils::from<float>(Utils::shareFMOD(), 0x16C));
#elif GEODE_IS_ANDROID
	sfxSlider->setValue(Utils::shareFMOD()->getEffectsVolume());
#endif
	sfxSlider->setScale(0.8f);
	sfxSlider->setAnchorPoint({ 0.f, 0.f });
	sfxSlider->setPosition({ Utils::WinSize().width - 155.f, Utils::WinSize().height - 120.f });
	this->addChild(sfxSlider);

	volumeSettingsMenuButtons = cocos2d::CCMenu::create();
	volumeSettingsMenuButtons->setPosition({ 0.f, 0.f });
	this->addChild(volumeSettingsMenuButtons);

	musicVolumeSettingsLabel = cocos2d::CCLabelBMFont::create("0%", "goldFont.fnt");
	musicVolumeSettingsLabel->setString(
		cocos2d::CCString::createWithFormat("%i%%", static_cast<int>(this->musicSlider->getThumb()->getValue() * 100))->getCString());
	musicVolumeSettingsLabel->setPosition({ this->musicSlider->getPositionX() + 90.f, this->musicSlider->getPositionY() + 33.f });
	musicVolumeSettingsLabel->setScale(0.6f);

	musicVolumeButton = CCMenuItemSpriteExtra::create(musicVolumeSettingsLabel, this, (cocos2d::SEL_MenuHandler)&BetterPause::onSetMusicVolume);
	musicVolumeButton->setPosition({ this->musicSlider->getPositionX() + 115.f, this->musicSlider->getPositionY() + 33.f });
	musicVolumeButton->setAnchorPoint({ 0.f, 0.5f });
	volumeSettingsMenuButtons->addChild(musicVolumeButton);

	sfxVolumeSettingsLabel = cocos2d::CCLabelBMFont::create("0%", "goldFont.fnt");
	sfxVolumeSettingsLabel->setString(
		cocos2d::CCString::createWithFormat("%i%%", static_cast<int>(this->sfxSlider->getThumb()->getValue() * 100))->getCString());
	sfxVolumeSettingsLabel->setPosition({ this->sfxSlider->getPositionX() + 90.f, this->sfxSlider->getPositionY() + 33.f });
	sfxVolumeSettingsLabel->setScale(0.6f);

	sfxVolumeButton = CCMenuItemSpriteExtra::create(sfxVolumeSettingsLabel, this, (cocos2d::SEL_MenuHandler)&BetterPause::onSetSfxVolume);
	sfxVolumeButton->setPosition({ this->sfxSlider->getPositionX() + 115.f, this->sfxSlider->getPositionY() + 33.f });
	sfxVolumeButton->setAnchorPoint({ 0.f, 0.5f });
	volumeSettingsMenuButtons->addChild(sfxVolumeButton);

	*/
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
	if (id == "not-a-string") bar->setID(id);
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
	if (id == "not-a-string") text->setID(id);
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
	if (id == "not-a-string") text->setID(id);
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
	Utils::shareDirectorA()->getRunningScene()->addChild(popup, Utils::shareDirectorA()->getRunningScene()->getHighestChildZ());
}

void BetterPause::onHide(cocos2d::CCObject* pSender) {
	this->isHidden = !this->isHidden;
	pauseLayer->setOpacity(this->isHidden ? 0 : 75);

	for (size_t i = 0; i < this->getChildrenCount(); i++)
	{
		auto node = reinterpret_cast<cocos2d::CCNode*>(this->getChildren()->objectAtIndex(i));
		node->setVisible(!this->isHidden);
	}

	if (this->isHidden)
	{
		for (size_t i = 0; i < pauseLayer->getChildrenCount(); i++)
		{
			if (!dynamic_cast<BetterPause*>(pauseLayer->getChildren()->objectAtIndex(i)))
			{
				auto node = reinterpret_cast<cocos2d::CCNode*>(pauseLayer->getChildren()->objectAtIndex(i));
				node->setVisible(false);
			}
		}
	}
	else
	{
		auto Fnode = reinterpret_cast<cocos2d::CCNode*>(pauseLayer->getChildren()->objectAtIndex(0));
		Fnode->setVisible(true);
		for (size_t i = 1; i < pauseLayer->getChildrenCount(); i++)
		{
			if (!dynamic_cast<BetterPause*>(pauseLayer->getChildren()->objectAtIndex(i)))
			{
				auto node = reinterpret_cast<cocos2d::CCNode*>(pauseLayer->getChildren()->objectAtIndex(i));
				node->setVisible(false);
			}
		}
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
		reinterpret_cast<cocos2d::CCSprite*>(visibleButton->getChildren()->objectAtIndex(0))->initWithFile(this->isHidden ? "BE_eye-off-btn.png"_spr : "BE_eye-on-btn.png"_spr);
	}
	visibleButton->setVisible(true);
	visibleButton->setOpacity(this->isHidden ? 50 : 255);
}

void BetterPause::onEditorHack(cocos2d::CCObject* pSender) {

	GJLevelType origType = Utils::getplayLayerA()->m_level->m_levelType;
	Utils::getplayLayerA()->m_level->m_levelType = GJLevelType::Editor;
	this->pauseLayer->onEdit(pSender);
	if (Utils::getplayLayerA()) {
		Utils::getplayLayerA()->m_level->m_levelType = origType;
	}
}

void BetterPause::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent) {
	//CCLayer::ccTouchMoved(pTouch, pEvent);

}

void BetterPause::onRedirectionToggle(cocos2d::CCObject* pSender) {

	auto toggleButton = reinterpret_cast<CCMenuItemToggler*>(pSender);
	auto gameOptionsLayer = GameOptionsLayer::create(Utils::getplayLayerA());
	auto toggleTag = toggleButton->getTag();

	if (toggleTag != -1) {
#ifdef GEODE_IS_WINDOWS
		gameOptionsLayer->didToggleAAAA(-toggleTag - 1);
#endif
#ifdef GEODE_IS_ANDROID
		gameOptionsLayer->didToggle(-toggleTag - 1);
#endif
	}

	gameOptionsLayer->onClose(nullptr);
	CC_SAFE_DELETE(gameOptionsLayer);

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

	gameOptionsLayer->onPracticeMusicSync(pSender);

	gameOptionsLayer->onClose(nullptr);
	CC_SAFE_DELETE(gameOptionsLayer);

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

	if (respect) {

		this->volumeSettingsLabel->setPositionX((this->volumeSettingsLabel->getPositionX() + 40.f) );
		this->questsLabel->setPositionX(this->questsLabel->getPositionX() + 40.f);

		this->sliderPlusMusic->setScale(this->sliderPlusMusic->getScale() - scale_value(1.7 - round(aspectRatio * 10.f) / 10.f));
		this->sliderPlusSFX->setScale(this->sliderPlusSFX->getScale() - scale_value(1.7 - round(aspectRatio * 10.f) / 10.f));
		this->questMenu->m_mainLayer->setScale(this->questMenu->m_mainLayer->getScale() - scale_value(1.7 - round(aspectRatio * 10.f) / 10.f));
		questMenu->m_mainLayer->setPosition({ questMenu->m_mainLayer->getPositionX() - 10.f, (-((0.1807f) * questMenu->m_mainLayer->getContentSize().height)) + 57.824f});
		//this->questMenu->m_mainLayer->setPosition({ this->questMenu->m_mainLayer->getPositionX() + (28.5f * ((scale_value(1.7 - round(aspectRatio * 10.f) / 10.f) * 10.f))),
		//	(this->questMenu->m_mainLayer->getPositionY() + (42.5f * (scale_value(1.7 - round(aspectRatio * 10.f) / 10.f) * 10.f))) * -1 });
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



	if (Utils::isSpecificAspectRatio(4.f/ 3.f) || Utils::isSpecificAspectRatio(3.f / 2.f) || Utils::isSpecificAspectRatio(5.f / 4.f)
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
			this->questButtonAlt->setPosition(CCDirector::sharedDirector()->getWinSize().width - 210.f, 120.f);
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

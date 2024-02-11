#include "BetterPause.hpp"

std::vector<std::string> BetterPause::quickSettings_Name = {};
std::vector<std::string> BetterPause::quickSettings_Desc = {};
std::vector<std::string> BetterPause::quickSettings_Key = {};
std::vector<std::string> BetterPause::quickSettings_NameG = {};
std::vector<int> BetterPause::quickSettings_numberG = {};
std::vector<bool> BetterPause::quickSettings_enabledG = {};
float BetterPause::m_timeTotalLevelBackup = 0.f;

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

void BetterPause::update(float dt) {


	if (totalHeightButtonsList < m_buttonsList->m_contentLayer->getPositionY() &&
		0.f > m_buttonsList->m_contentLayer->getPositionY()) {
		this->upBtnSpriteList->setVisible(true);
		this->downBtnSpriteList->setVisible(true);
	}
	else if (m_buttonsList->m_contentLayer->getPositionY() >= 0.f) {
		this->upBtnSpriteList->setVisible(true);
		this->downBtnSpriteList->setVisible(false);
	}
	else if (m_buttonsList->m_contentLayer->getPositionY() <= totalHeightButtonsList) {
		this->upBtnSpriteList->setVisible(false);
		this->downBtnSpriteList->setVisible(true);
	}

	if (this->m_pCustomSongWidget->m_artistLabel && this->m_pCustomSongWidget->m_moreBtn) {
		auto labelPos = this->m_pCustomSongWidget->m_artistLabel->getPosition();
		auto labelWidth = this->m_pCustomSongWidget->m_artistLabel->getContentSize().width * this->m_pCustomSongWidget->m_artistLabel->getScale();

		auto menuItemX = labelPos.x + labelWidth - 250.f;

		this->m_pCustomSongWidget->m_moreBtn->setPositionX(menuItemX);
	}

}

void BetterPause::createTitlesSeccion() {

	this->m_pQuickSettingsLabel = cocos2d::CCLabelBMFont::create("Quick Settings", "goldFont.fnt");
	this->m_pQuickSettingsLabel->setScale(0.5f);
	//if (Mod::get()->getSettingValue<bool>("first-style"))
	//{
	///	this->m_pQuickSettingsLabel->setScale(0.8f);
	//}
	this->m_pQuickSettingsLabel->setPosition({ 165.f, 164.f });
	this->addChild(this->m_pQuickSettingsLabel);

	this->m_pVolumenSettingsLabel = cocos2d::CCLabelBMFont::create("Volume Settings:", "bigFont.fnt");
	this->m_pVolumenSettingsLabel->setScale(0.5f);
	this->m_pVolumenSettingsLabel->setPosition({ this->m_pSliderMusic->getPositionX() + 5.f, this->m_pSliderMusic->getPositionY() + 60.f });
	this->addChild(this->m_pVolumenSettingsLabel);

	this->m_pVolumenMusicSettingsLabel = cocos2d::CCLabelBMFont::create("Music", "bigFont.fnt");
	this->m_pVolumenMusicSettingsLabel->setScale(0.4f);
	this->m_pVolumenMusicSettingsLabel->setPosition({ this->m_pSliderMusic->getPositionX() - 115.f, this->m_pSliderMusic->getPositionY() + 33.f });
	this->addChild(this->m_pVolumenMusicSettingsLabel);

	this->m_pVolumenSFXSettingsLabel = cocos2d::CCLabelBMFont::create("SFX", "bigFont.fnt");
	this->m_pVolumenSFXSettingsLabel->setScale(0.4f);
	this->m_pVolumenSFXSettingsLabel->setPosition({ this->m_pSliderSFX->getPositionX() - 110.f, this->m_pSliderSFX->getPositionY() + 33.f });
	this->addChild(this->m_pVolumenSFXSettingsLabel);

	this->m_pQuestsLabel = cocos2d::CCLabelBMFont::create("Quest:", "bigFont.fnt");
	this->m_pQuestsLabel->setScale(0.5f);
	this->m_pQuestsLabel->setPosition({ CCDirector::sharedDirector()->getWinSize().width - 150.f, this->m_pSliderSFX->getPositionY() + 10.f });
	this->addChild(this->m_pQuestsLabel);
}

void BetterPause::handleOptionsLayers() {
	auto gol = GameOptionsLayer::create(Utils::getplayLayerA());
	gol->onClose(nullptr);
	CC_SAFE_DELETE(gol);

	auto mlt = MoreOptionsLayer::create();
	mlt->keyBackClicked();
	CC_SAFE_DELETE(mlt);
}

bool BetterPause::init(PauseLayer* pauLa)
{
	if (!cocos2d::CCLayer::init())
	{
		return false;
	}

	this->pauseLayer = pauLa;

	this->createQuestMenu();
	this->createButtonsMenu();
	this->createButtonsMenu2();
	this->createLabels();

	if(!Utils::getplayLayerA()->m_level->isPlatformer())
		this->createBars();
	else
		this->createTimesPlatformer();

	this->createQuickButtons();
	this->createCustomSongWidget();
	this->createTitlesSeccion();
	this->tryGetExternalButtonsMods();

	return true;
}

void BetterPause::createTimesPlatformer() {
	auto ppb = ProgressPlataformerBetter::create();
	ppb->setPosition({ 86.f, Utils::WinSize().height - 90.f });
	this->addChild(ppb);
}

void BetterPause::createCustomSongWidget() {
	auto songID_ = GameManager::sharedState()->getPlayLayer()->m_level->m_songID;
	SongInfoObject* songObj = nullptr;

	if (songID_ == 0)
	{
		songObj = LevelTools::getSongObject(GameManager::sharedState()->getPlayLayer()->m_level->m_audioTrack);
	}
	else
	{
		songObj = SongInfoObject::create(songID_);
	}

	this->m_pCustomSongWidget = CustomSongWidget::create(songObj, 0, 0, 1, 1, songID_ == 0, 0, 0);
	this->m_pCustomSongWidget->setPosition(172.f, 50.f);
	this->m_pCustomSongWidget->setScale(0.6f);
	this->addChild(this->m_pCustomSongWidget);

	//std::cout << Utils::from<float>(songObj, 0x18c) << std::endl;

	if (Utils::from<float>(songObj, 0x18c) != 0.f) {
		this->m_pCustomSongWidget->onGetSongInfo(nullptr);
	}

	auto m_songIDs = GameManager::sharedState()->getPlayLayer()->m_level->m_songIDs;
	auto m_sfxIDs = GameManager::sharedState()->getPlayLayer()->m_level->m_sfxIDs;

	this->m_pCustomSongWidget->updateWithMultiAssets(m_songIDs, m_sfxIDs, 0);

	
}

void BetterPause::createQuickButtons() {

	m_pMenuButtonsConfig = cocos2d::CCMenu::create();
	this->addChild(m_pMenuButtonsConfig);

	
	this->handleOptionsLayers();

	auto xPosButtonsToggles = Mod::get()->getSavedValue<bool>("pos-switch-buttons") == 1 ? 196.f : 250.f;

	float xPosRES[6] = { xPosButtonsToggles, xPosButtonsToggles, xPosButtonsToggles, xPosButtonsToggles - 90.f, xPosButtonsToggles - 90.f, xPosButtonsToggles - 90.f };
	float yPosRES[6] = { 140.f, 120.f, 100.f, 140.f, 120.f, 100.f };

	for (size_t i = 0; i < 6; i++) {

		auto posA = BetterPauseManager::sharedState()->posQuickA[i];
		if (posA == -1) {
			continue;
		}

		float xPos, yPos;

		xPos = xPosRES[i];
		yPos = yPosRES[i];
		

		if (posA < -1) {

			if (posA == -10) {
				this->createToggleButton((cocos2d::SEL_MenuHandler)&BetterPause::onPracticeMusicSyncRedirection, GameStatsManager::sharedState()->isItemEnabled(UnlockType::GJItem, 0x11), m_pMenuButtonsConfig, "Practice Music Sync", { xPos, yPos }, 0.25f, true, posA);
			}
			else {
				this->createToggleButton((cocos2d::SEL_MenuHandler)&BetterPause::onToogleRedirection, quickSettings_enabledG[-posA - 2], m_pMenuButtonsConfig, quickSettings_NameG[-posA - 2], { xPos, yPos }, 0.25f, true, posA);
			}
		}
		else {
			this->createToggleButtonWithGameVariable(BetterPause::quickSettings_Key[posA].c_str(), m_pMenuButtonsConfig, BetterPause::quickSettings_Name[posA].c_str(), { xPos, yPos }, 0.25f, true);
		}

		/*
		if (posA == -2) {
			this->createToggleButton((cocos2d::SEL_MenuHandler)&BetterPause::onProgressBarA, Utils::shareManager()->m_showProgressBar, m_pMenuButtonsConfig, "Progress Bar", { xPos, yPos }, 0.25f, true, -1);
		}
		else if (BetterPause::quickSettings_Name[posA].c_str() != nullptr) {
			if (BetterPause::quickSettings_Key[posA] == "0040") {
				this->createToggleButton((cocos2d::SEL_MenuHandler)&BetterPause::onProgressBarPercentageA, "0040", m_pMenuButtonsConfig, "Progress %", { xPos, yPos }, 0.25f, true, -1);
			}
			else {
				this->createToggleButtonWithGameVariable(BetterPause::quickSettings_Key[posA].c_str(), m_pMenuButtonsConfig, BetterPause::quickSettings_Name[posA].c_str(), { xPos, yPos }, 0.25f, true);
			}
		}
		else {
			for (size_t j = 0; j < 6; j++) {
				bool romp = false;
				for (size_t k = 0; k < 6; k++) {
					if (BetterPauseManager::sharedState()->posQuickASafe[j] == BetterPauseManager::sharedState()->posQuickA[k]) {
						break;
					}
					if (j + 1 == 6 && BetterPauseManager::sharedState()->posQuickASafe[j] != BetterPauseManager::sharedState()->posQuickA[k]) {
						BetterPauseManager::sharedState()->posQuickA[i] = BetterPauseManager::sharedState()->posQuickASafe[j];
						romp = true;
						break;
					}
				}
				if (romp) {
					break;
				}
			}
			if (BetterPause::quickSettings_Key[posA] == "0040") {
				this->createToggleButton((cocos2d::SEL_MenuHandler)&BetterPause::onProgressBarPercentageA, "0040", m_pMenuButtonsConfig, "Progress %", { xPos, yPos }, 0.25f, true, -1);
			}
			else {
				this->createToggleButtonWithGameVariable(BetterPause::quickSettings_Key[posA].c_str(), m_pMenuButtonsConfig, BetterPause::quickSettings_Name[posA].c_str(), { xPos, yPos }, 0.25f, true);
			}
				
		}
		*/
	}

	this->clearQuickSettings();

}

void BetterPause::createQuestMenu() {

	m_pQuestMenu = ChallengesPage::create();
	this->addChild(m_pQuestMenu);
	m_pQuestMenu->setScale(0.7f);
	m_pQuestMenu->setOpacity(0);
	m_pQuestMenu->setPosition({ (Utils::WinSize().width / 2) - 150.f, -50.f }); // 140.f, -40.f
	m_pQuestMenu->setKeyboardEnabled(false);
	m_pQuestMenu->setKeypadEnabled(false);

	for (size_t i = 0; i < m_pQuestMenu->m_mainLayer->getChildrenCount(); i++)
	{
		auto node = reinterpret_cast<cocos2d::CCNode*>(m_pQuestMenu->m_mainLayer->getChildren()->objectAtIndex(i));
		if (std::string(Utils::getNameObj(node)) != "ChallengeNode" && !dynamic_cast<cocos2d::CCLabelBMFont*>(node) &&
			std::string(Utils::getNameObj(node)) != "LoadingCircle")
		{
			node->setVisible(false);
		}
	}

}

void BetterPause::createButtonsMenu() {

	CCSize LAYER_SIZE = { 230.f, 180.f };

	auto menuScrollButtons = cocos2d::CCMenu::create();
	menuScrollButtons->setVisible(true);
	menuScrollButtons->setPosition({ 0.f, 0.f });
	this->addChild(menuScrollButtons);

	m_buttonsList = ScrollLayer::create(LAYER_SIZE);
	m_buttonsList->setPosition(
		{ 20.f, Utils::WinSize().height - 205.f }
	);
	m_buttonsList->setContentSize({ 45.f, 180.f });
	menuScrollButtons->addChild(m_buttonsList);

	upBtnSpriteList = cocos2d::CCSprite::createWithSpriteFrameName("edit_upBtn_001.png");
	upBtnSpriteList->setScaleX(2.f);
	upBtnSpriteList->setScaleY(1.4f);
	upBtnSpriteList->setPosition({ 40.f, Utils::WinSize().height - 15.f });
	upBtnSpriteList->runAction(cocos2d::CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.4f, 50), CCFadeTo::create(0.4f, 255), nullptr)));
	menuScrollButtons->addChild(upBtnSpriteList);

	downBtnSpriteList = cocos2d::CCSprite::createWithSpriteFrameName("edit_downBtn_001.png");
	downBtnSpriteList->setScaleX(2.f);
	downBtnSpriteList->setScaleY(1.4f);
	downBtnSpriteList->setPosition({ 40.f, Utils::WinSize().height - 213.f });
	downBtnSpriteList->runAction(cocos2d::CCRepeatForever::create(CCSequence::create(CCFadeTo::create(0.4f, 50), CCFadeTo::create(0.4f, 255), nullptr)));
	menuScrollButtons->addChild(downBtnSpriteList);

	float totalHeight = 0.0f;

	auto createButtonMenu = [&](CCMenuItemSpriteExtra* button) {
		auto menu = cocos2d::CCMenu::create();
		menu->setVisible(true);
		menu->setPosition({ 20.f, -totalHeight });
		m_buttonsList->m_contentLayer->addChild(menu);

		menu->addChild(button);
		button->setPosition(0.f, 0.f);
		};

	// Resume Button
	auto resumeButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
	resumeButtonImage->setScale(0.5f);
	auto resumeButton = CCMenuItemSpriteExtra::create(resumeButtonImage, pauseLayer, (cocos2d::SEL_MenuHandler)&PauseLayer::onResume);
	totalHeight += resumeButtonImage->getContentSize().height - 36.f;
	createButtonMenu(resumeButton);

	// Replay Button
	auto replayButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_replayBtn_001.png");
	replayButtonImage->setScale(0.5f);
	auto replayButton = CCMenuItemSpriteExtra::create(replayButtonImage, pauseLayer, (cocos2d::SEL_MenuHandler)&PauseLayer::onRestart);
	totalHeight += replayButtonImage->getContentSize().height - 24.f;
	createButtonMenu(replayButton);

	if (Utils::getplayLayerA()->m_level->isPlatformer()) {
		// Replay Full Button
		auto replayFullButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_replayFullBtn_001.png");
		replayFullButtonImage->setScale(0.5f);
		auto replayFullButton = CCMenuItemSpriteExtra::create(replayFullButtonImage, pauseLayer, (cocos2d::SEL_MenuHandler)&PauseLayer::onRestartFull);
		totalHeight += replayFullButtonImage->getContentSize().height - 24.f;
		createButtonMenu(replayFullButton);
	}

	// Practice Button
	auto practiceButtonImage = cocos2d::CCSprite::createWithSpriteFrameName(Utils::getplayLayerA()->m_isPracticeMode ? "GJ_normalBtn_001.png" : "GJ_practiceBtn_001.png");
	practiceButtonImage->setScale(0.5f);
	auto practiceButton = CCMenuItemSpriteExtra::create(practiceButtonImage, pauseLayer, (Utils::getplayLayerA()->m_isPracticeMode ? (cocos2d::SEL_MenuHandler)&PauseLayer::onNormalMode : (cocos2d::SEL_MenuHandler)&PauseLayer::onPracticeMode));
	totalHeight += practiceButtonImage->getContentSize().height - 24.f;
	createButtonMenu(practiceButton);

	// Quit Button
	auto quitButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_menuBtn_001.png");
	quitButtonImage->setScale(0.5f);
	auto quitButton = CCMenuItemSpriteExtra::create(quitButtonImage, pauseLayer, (cocos2d::SEL_MenuHandler)&PauseLayer::tryQuit);
	totalHeight += quitButtonImage->getContentSize().height - 24.f;
	createButtonMenu(quitButton);

	// Add edit button if in editor mode
	if (Utils::getplayLayerA()->m_level->m_levelType == GJLevelType::Editor || Mod::get()->getSettingValue<bool>("level-editor-hack")) {
		auto editButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_editBtn_001.png");
		editButtonImage->setScale(0.38f);
		auto editButton = CCMenuItemSpriteExtra::create(editButtonImage, this, (cocos2d::SEL_MenuHandler)&BetterPause::onEditorHack);
		totalHeight += editButtonImage->getContentSize().height - 42.f;
		createButtonMenu(editButton);
	}

	// Show Comments Button
	if (Mod::get()->getSettingValue<bool>("show-comment-button")) {
		auto showCommentsImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_chatBtn_001.png");
		showCommentsImage->setScale(0.6f);
		auto showCommentsButton = CCMenuItemSpriteExtra::create(showCommentsImage, pauseLayer, (cocos2d::SEL_MenuHandler)&BetterPause::onInfoLevelLayer);
		totalHeight += showCommentsImage->getContentSize().height - 8.f;
		createButtonMenu(showCommentsButton);
	}

	std::vector<CCMenuItemSpriteExtra*> menuButtonsDetected = {};

	std::vector<std::string> buttonIds{
	"edit-button",
	"full-restart-button",
	"practice-button",
	"play-button",
	"exit-button",
	"retry-button",
	"options-button"
	};

	this->findButtonsRecursively(this->pauseLayer, buttonIds, menuButtonsDetected);

	for (size_t i = 0; i < menuButtonsDetected.size(); i++)
	{
		auto buttonExt = menuButtonsDetected[i];
		if (buttonExt) {
			typeinfo_cast<CCSprite*>(buttonExt->getChildren()->objectAtIndex(0))->setScale(0.6f);
			totalHeight += buttonExt->getContentSize().height - 10.f;
			createButtonMenu(buttonExt);
		}
	}


	auto allChildrens = m_buttonsList->m_contentLayer->getChildren();
	CCObject* node;
	CCARRAY_FOREACH(allChildrens, node) {
		auto child = dynamic_cast<CCNode*>(node);
		child->setPositionY(child->getPositionY() + totalHeight + 25.f);
	}

	if (totalHeight < LAYER_SIZE.height) {
		totalHeight = LAYER_SIZE.height;
	}


	m_buttonsList->m_contentLayer->setContentSize({ LAYER_SIZE.width, totalHeight });
	m_buttonsList->moveToTop();
	m_buttonsList->setContentSize({ 45.f, 180.f });

	totalHeightButtonsList = m_buttonsList->m_contentLayer->getPositionY();
}



void BetterPause::createButtonsMenu2() {
	m_pMenuButtons2 = cocos2d::CCMenu::create();
	m_pMenuButtons2->setPosition({ 40.f, 42.f });
	this->addChild(m_pMenuButtons2);

	auto visibleButtonImage = cocos2d::CCSprite::create("BE_eye-on-btn.png");
	if (!visibleButtonImage)
	{
		visibleButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_reportBtn_001.png");
		this->m_pIsTextureEye = false;
	}
	visibleButtonImage->setScale(0.6f);
	this->m_pVisibleButton = CCMenuItemSpriteExtra::create(visibleButtonImage, this, (cocos2d::SEL_MenuHandler)&BetterPause::onHide);
	m_pMenuButtons2->addChild(this->m_pVisibleButton, -5);

	auto settingsButtonImage = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
	settingsButtonImage->setScale(0.6f);
	auto settingsButton = CCMenuItemSpriteExtra::create(settingsButtonImage, this, (cocos2d::SEL_MenuHandler)&BetterPause::onOptionsLayer);
	m_pMenuButtons2->addChild(settingsButton);

	auto firstButtonRect = m_pVisibleButton->boundingBox();
	settingsButton->setPositionY(firstButtonRect.getMaxY() + 22.f);

	m_pSliderMusic = Slider::create(this, (cocos2d::SEL_MenuHandler)&BetterPause::musicSliderChanged, 1.f);
	m_pSliderMusic->setValue(Utils::from<float>(Utils::shareFMOD(), 0x168));
	m_pSliderMusic->setScale(0.8f);
	m_pSliderMusic->setAnchorPoint({ 0.f, 0.5f });
	m_pSliderMusic->setPosition({ Utils::WinSize().width - 155.f, Utils::WinSize().height - 90.f });
	this->addChild(m_pSliderMusic);


	m_pSliderSFX = Slider::create(this, (cocos2d::SEL_MenuHandler)&BetterPause::sfxSliderChanged, 1.f);
	m_pSliderSFX->setValue(Utils::from<float>(Utils::shareFMOD(), 0x16C));
	m_pSliderSFX->setScale(0.8f);
	m_pSliderSFX->setAnchorPoint({ 0.f, 0.5f });
	m_pSliderSFX->setPosition({ Utils::WinSize().width - 155.f, Utils::WinSize().height - 120.f });
	this->addChild(m_pSliderSFX);

	m_pMenuButtonsVolSet = cocos2d::CCMenu::create();
	m_pMenuButtonsVolSet->setPosition({ 0.f, 0.f });
	this->addChild(m_pMenuButtonsVolSet);

	m_pLabelMusicVPercentage = cocos2d::CCLabelBMFont::create("0%", "goldFont.fnt");
	m_pLabelMusicVPercentage->setString(
		cocos2d::CCString::createWithFormat("%i%%", static_cast<int>(this->m_pSliderMusic->getThumb()->getValue() * 100))->getCString());
	m_pLabelMusicVPercentage->setPosition({ this->m_pSliderMusic->getPositionX() + 95.f, this->m_pSliderMusic->getPositionY() + 33.f });
	m_pLabelMusicVPercentage->setScale(0.6f);
	m_pLabelMusicVPercentage->setAnchorPoint({ 0.f, 0.5f });
	m_pBtnMusicVPercentage = CCMenuItemSpriteExtra::create(m_pLabelMusicVPercentage, this, (cocos2d::SEL_MenuHandler)&BetterPause::onSetValueMusic);
	m_pBtnMusicVPercentage->setPosition({ this->m_pSliderMusic->getPositionX() + 115.f, this->m_pSliderMusic->getPositionY() + 33.f });
	m_pMenuButtonsVolSet->addChild(m_pBtnMusicVPercentage);

	m_pLabelSFXVPercentage = cocos2d::CCLabelBMFont::create("0%", "goldFont.fnt");
	m_pLabelSFXVPercentage->setString(
		cocos2d::CCString::createWithFormat("%i%%", static_cast<int>(this->m_pSliderSFX->getThumb()->getValue() * 100))->getCString());
	m_pLabelSFXVPercentage->setPosition({ this->m_pSliderSFX->getPositionX() + 95.f, this->m_pSliderSFX->getPositionY() + 33.f });
	m_pLabelSFXVPercentage->setScale(0.6f);
	m_pLabelSFXVPercentage->setAnchorPoint({ 0.f, 0.5f });
	m_pLabelSFXVPercentage->setScale(0.6f);
	m_pLabelSFXVPercentage->setAnchorPoint({ 0.f, 0.5f });
	m_pBtnSFXVPercentage = CCMenuItemSpriteExtra::create(m_pLabelSFXVPercentage, this, (cocos2d::SEL_MenuHandler)&BetterPause::onSetValueSFX);
	m_pBtnSFXVPercentage->setPosition({ this->m_pSliderSFX->getPositionX() + 115.f, this->m_pSliderSFX->getPositionY() + 33.f });
	m_pMenuButtonsVolSet->addChild(m_pBtnSFXVPercentage);
}

void BetterPause::createLabels() {

	m_pNameLevelLabel = cocos2d::CCLabelBMFont::create(Utils::getplayLayerA()->m_level->m_levelName.c_str(), "goldFont.fnt");
	m_pNameLevelLabel->limitLabelWidth(150.f, 1.f, 0.1f);
	m_pNameLevelLabel->setAnchorPoint({ 0.f, 0.5f });
	m_pNameLevelLabel->setPosition({ 86.f, Utils::WinSize().height - 30.f});
	this->addChild(m_pNameLevelLabel);

	auto getNameLevelType = [](GJLevelType type)
		{
			switch (type)
			{
			case GJLevelType::Editor:
			{
				return "Editor Level";
			}
			case GJLevelType::Local:
			{
				return "Official Level";
			}
			case GJLevelType::Saved:
			{
				return "Online Level";
			}
			default:
			{
				return "NA Level";
			}
			}
		};

	cocos2d::ccColor3B green_Color = { 0, 255, 0 };
	cocos2d::ccColor3B bluesky_Color = { 0, 255, 255 };

	m_pTypeLevelLabel = cocos2d::CCLabelBMFont::create(getNameLevelType(Utils::getplayLayerA()->m_level->m_levelType), "bigFont.fnt");
	m_pTypeLevelLabel->setScale(0.3f);
	m_pTypeLevelLabel->setPosition({ 86.f, Utils::WinSize().height - 45.f });
	m_pTypeLevelLabel->setAnchorPoint({ 0.f, 1.f });
	this->addChild(m_pTypeLevelLabel);

	m_pStatusLevelLabel = cocos2d::CCLabelBMFont::create(Utils::getplayLayerA()->m_isPracticeMode ? "Practice Mode" : "Normal Mode", "bigFont.fnt");
	m_pStatusLevelLabel->setScale(0.3f);
	m_pStatusLevelLabel->setColor(Utils::getplayLayerA()->m_isPracticeMode ? bluesky_Color : green_Color);
	m_pStatusLevelLabel->setPosition({ 195.f, Utils::WinSize().height - 45.f });
	m_pStatusLevelLabel->setAnchorPoint({ 0.f, 1.f });
	this->addChild(m_pStatusLevelLabel);

	auto attemptColorD = Utils::getplayLayerA()->m_isPracticeMode ? "Attempt: <cj>%i</c>" : "Attempt: <cg>%i</c>";
	std::string timeColorD = Utils::getplayLayerA()->m_isPracticeMode ? "Time: <cj>%02d:%02d</c>" : "Time: <cg>%02d:%02d</c>";

	if (Utils::getplayLayerA()->m_level->isPlatformer()) {
		timeColorD = Utils::getplayLayerA()->m_isPracticeMode ? "Time Total: <cj>%02d:%02d</c>" : "Time Total: <cg>%02d:%02d</c>";
	}

	m_pAttemptCurrentLevelLabel = TextArea::create(gd::string(cocos2d::CCString::createWithFormat(attemptColorD, Utils::from<int>(Utils::getplayLayerA(), 0x29ac))->getCString()), "bigFont.fnt", 0.3f, 100.f, { 0.f, 1.f }, 100.f, false);
	m_pAttemptCurrentLevelLabel->setPosition({ 186.f, Utils::WinSize().height - 60.f });
	m_pAttemptCurrentLevelLabel->m_label->setAnchorPoint({ 0.f, 1.f });
	this->addChild(m_pAttemptCurrentLevelLabel);

	int totalSeconds = std::floor(Utils::from<double>(Utils::getplayLayerA(), 0x320));

	if (Utils::getplayLayerA()->m_level->isPlatformer()) {
		totalSeconds = BetterPause::m_timeTotalLevelBackup;
	}

	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;


	m_pTimeCurrentLevelLabel = TextArea::create(gd::string(cocos2d::CCString::createWithFormat(timeColorD.c_str(), minutes, seconds)->getCString()), "bigFont.fnt", 0.3f, 100.f, {0.f, 1.f}, 100.f, false);
	m_pTimeCurrentLevelLabel->setPosition({ 295.f,
										   Utils::WinSize().height - 60.f});
	this->addChild(m_pTimeCurrentLevelLabel);
}

void BetterPause::createBars() {

	cocos2d::ccColor3B green_Color = { 0, 255, 0 };
	cocos2d::ccColor3B bluesky_Color = { 0, 255, 255 };

	m_pNormalBarPerB = BarBetterShow::create(green_Color, !Utils::getplayLayerA()->m_isPracticeMode, !Utils::getplayLayerA()->m_isPracticeMode, Utils::getPercentageNowFix(), Utils::getplayLayerA()->m_level->m_normalPercent);
	m_pNormalBarPerB->setPosition({ 86.f, Utils::WinSize().height - 90.f});
	m_pNormalBarPerB->setScale(0.5f);
	m_pNormalBarPerB->m_pBarBase->setVisible(true);
	m_pNormalBarPerB->m_pBarBase->setOpacity(Utils::convertOpacitySimplf(0.2f));
	this->addChild(m_pNormalBarPerB);

	

	m_pPracticeBarPerB = BarBetterShow::create(bluesky_Color, Utils::getplayLayerA()->m_isPracticeMode, Utils::getplayLayerA()->m_isPracticeMode, Utils::getPercentageNowFix(), Utils::getplayLayerA()->m_level->m_practicePercent);
	m_pPracticeBarPerB->setPosition({ 86.f, Utils::WinSize().height - 125.f });
	m_pPracticeBarPerB->setScale(0.5f);
	m_pPracticeBarPerB->m_pBarBase->setVisible(true);
	m_pPracticeBarPerB->m_pBarBase->setOpacity(Utils::convertOpacitySimplf(0.2f));
	this->addChild(m_pPracticeBarPerB);
}

void BetterPause::onInfoLevelLayer(cocos2d::CCObject* pSender) {
	if (Utils::getplayLayerA()) {
		auto infoLayer = InfoLayer::create(Utils::getplayLayerA()->m_level, nullptr, nullptr);
		infoLayer->show();
	}
}

void BetterPause::musicSliderChanged(cocos2d::CCObject* pSender) {

	//auto musicSliderPM = reinterpret_cast<Slider*>(this->pauseLayer->getChildByTag(9));
	auto valueV = this->m_pSliderMusic->getThumb()->getValue();
	//musicSliderPM->setValue(valueV);
	this->pauseLayer->musicSliderChanged(pSender);
	this->m_pLabelMusicVPercentage->setString(
		cocos2d::CCString::createWithFormat("%i%%", static_cast<int>(valueV * 100))->getCString());

	return;

	//this->pauseLayer->musicSliderChanged(pSender);
	//return;
	//auto valueV = this->m_pSliderMusic->getThumb()->getValue();
	//Utils::shareFMOD()->m_globalChannel->setVolume(valueV);


	//float zzz = 0.f;
	//Utils::shareFMOD()->m_globalChannel->getVolume(&zzz);

	//this->m_pLabelMusicVPercentage->setString(
	//	cocos2d::CCString::createWithFormat("%i%%", static_cast<int>(zzz * 100))->getCString());

	
}

void BetterPause::sfxSliderChanged(cocos2d::CCObject* pSender) {

	//auto sfxSliderPM = reinterpret_cast<Slider*>(this->pauseLayer->getChildByTag(10));
	auto valueV = this->m_pSliderSFX->getThumb()->getValue();
	//sfxSliderPM->setValue(valueV);
	this->pauseLayer->sfxSliderChanged(pSender);
	this->m_pLabelSFXVPercentage->setString(
		cocos2d::CCString::createWithFormat("%i%%", static_cast<int>(valueV * 100))->getCString());

	return;


	//this->pauseLayer->sfxSliderChanged(pSender);
	//return;
	//MessageBoxA(0, std::to_string((float)Utils::from<float>(Utils::shareFMOD(), 0x16C)).c_str(), "e", 0);
	//return;
	//MessageBoxA(0, std::to_string((float)Utils::from<float>(Utils::shareFMOD(), 0x16C)).c_str(), "e", 0);

	//Utils::from<float>(Utils::shareFMOD(), 0x16C) = valueV;
	//Utils::shareFMOD()->m_currentSoundChannel->setVolume(valueV);

	//float zzz = 0.f;
	//Utils::shareFMOD()->m_currentSoundChannel->getVolume(&zzz);


	
}

void BetterPause::onSetValueMusic(cocos2d::CCObject* pSender) {
	auto popup = SetVolumenPopup::create(this->m_pSliderMusic);
	popup->setID("popup-betterpause");
	popup->show();
}

void BetterPause::onSetValueSFX(cocos2d::CCObject* pSender) {
	auto popup = SetVolumenPopup::create(this->m_pSliderSFX);
	popup->setID("popup-betterpause");
	popup->show();
}

void BetterPause::onOpenChallenges(cocos2d::CCObject* pSender) {

}

void BetterPause::onInfoLevelOpen(cocos2d::CCObject* pSender) {

}

void BetterPause::createToggleButtonWithGameVariable(const char* key, cocos2d::CCMenu* menu, std::string caption, cocos2d::CCPoint pos, float size, bool twoColumns) {
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
	this->addChild(text);
}

void BetterPause::onToggleWithGameVariable(cocos2d::CCObject* pSender) {
	int tagValue = static_cast<int>(reinterpret_cast<cocos2d::CCNode*>(pSender)->getTag());
	std::stringstream ss;
	ss << std::setw(4) << std::setfill('0') << tagValue;
	std::string keyCode = ss.str();
	Utils::shareManager()->setGameVariable(keyCode.c_str(), !Utils::shareManager()->getGameVariable(keyCode.c_str()));
}

void BetterPause::onHideTrue(cocos2d::CCObject* pSender) {

}

void BetterPause::createToggleButton(cocos2d::SEL_MenuHandler callback, bool on,
	cocos2d::CCMenu* menu, std::string caption, cocos2d::CCPoint pos, float size,
	bool twoColumns, int tag) {
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
	this->addChild(text);
}

void BetterPause::onOptionsLayer(cocos2d::CCObject* pSender) {
	auto popup = MoreOptionsPauseLayer::create(this);
	popup->setID("popup-betterpause");
	Utils::shareDirectorA()->getRunningScene()->addChild(popup, Utils::shareDirectorA()->getRunningScene()->getHighestChildZ());
}

void BetterPause::onHide(cocos2d::CCObject* pSender) {
	this->m_pIsHide = !this->m_pIsHide;
	pauseLayer->setOpacity(this->m_pIsHide ? 0 : 75);

	for (size_t i = 0; i < this->getChildrenCount(); i++)
	{
		auto node = reinterpret_cast<cocos2d::CCNode*>(this->getChildren()->objectAtIndex(i));
		node->setVisible(!this->m_pIsHide);
	}

	if (this->m_pIsHide)
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
	this->m_pMenuButtons2->setVisible(true);

	for (size_t i = 0; i < this->m_pMenuButtons2->getChildrenCount(); i++)
	{
		auto node = reinterpret_cast<cocos2d::CCNode*>(this->m_pMenuButtons2->getChildren()->objectAtIndex(i));
		node->setVisible(!this->m_pIsHide);
	}

	if (m_pIsTextureEye)
	{
		reinterpret_cast<cocos2d::CCSprite*>(m_pVisibleButton->getChildren()->objectAtIndex(0))->initWithFile(this->m_pIsHide ? "BE_eye-off-btn.png" : "BE_eye-on-btn.png");
	}
	m_pVisibleButton->setVisible(true);
	m_pVisibleButton->setOpacity(this->m_pIsHide ? 50 : 255);
}

void BetterPause::onAutoRetryA(cocos2d::CCObject* pSender) {

}

void BetterPause::onAutoCheckpointsA(cocos2d::CCObject* pSender) {

}

void BetterPause::onProgressBarA(cocos2d::CCObject* pSender) {

}

void BetterPause::onProgressBarPercentageA(cocos2d::CCObject* pSender) {

}

void BetterPause::onDisableShakeEffects(cocos2d::CCObject* pSender) {

}

void BetterPause::onShowCursorInGame(cocos2d::CCObject* pSender) {

}

void BetterPause::updatePercentageObjects() {

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

void BetterPause::onToogleRedirection(cocos2d::CCObject* pSender) {

	auto toggleButton = reinterpret_cast<CCMenuItemToggler*>(pSender);
	auto gameOptionsLayer = GameOptionsLayer::create(Utils::getplayLayerA());
	auto toggleTag = toggleButton->getTag();

	if (toggleTag != -1) {
		gameOptionsLayer->didToggle(-toggleTag - 1);
	}

	gameOptionsLayer->onClose(nullptr);
	CC_SAFE_DELETE(gameOptionsLayer);

	this->clearQuickSettings();
}

void BetterPause::clearQuickSettings() {
	BetterPause::quickSettings_NameG.clear();
	BetterPause::quickSettings_numberG.clear();
	BetterPause::quickSettings_enabledG.clear();
	BetterPause::quickSettings_Name.clear();
	BetterPause::quickSettings_Desc.clear();
	BetterPause::quickSettings_Key.clear();
}

void BetterPause::onPracticeMusicSyncRedirection(cocos2d::CCObject* pSender) {
	auto toggleButton = reinterpret_cast<CCMenuItemToggler*>(pSender);
	auto gameOptionsLayer = GameOptionsLayer::create(Utils::getplayLayerA());

	gameOptionsLayer->onPracticeMusicSync(pSender);

	gameOptionsLayer->onClose(nullptr);
	CC_SAFE_DELETE(gameOptionsLayer);

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

void BetterPause::tryGetExternalButtonsMods() {
	
}
#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include "BarBetterShow.hpp"
#include "Utils.hpp"
#include "BetterPauseManager.h"
#include "ProgressPlataformerBetter.hpp"
#include "SetVolumenPopup.hpp"
#include "MoreOptionsPauseLayer.hpp"
#include "SliderPlus.hpp"

using namespace geode::prelude;

class BetterPause : public cocos2d::CCLayer
{
public:
    static cocos2d::CCMenu* pauseMenuToggles;
    static cocos2d::CCPoint lastPausePoint;
    static std::vector<std::string> quickSettingsNames;
    static std::vector<std::string> quickSettingsDescriptions;
    static std::vector<std::string> quickSettingsKeys;
    static std::vector<std::string> quickSettingsNamesG;
    static std::vector<int> quickSettingsNumbersG;
    static std::vector<bool> quickSettingsEnabledG;
    static float totalLevelTimeBackup;
    static std::vector<GameObject*> coinsObjects;

    cocos2d::CCMenu* mainMenuButtons = nullptr;
    cocos2d::CCMenu* secondaryMenuButtons = nullptr;
    PauseLayer* pauseLayer = nullptr;
    CCMenuItemSpriteExtra* visibleButton = nullptr;
    bool isHidden = false;
    Slider* musicSlider = nullptr;
    Slider* sfxSlider = nullptr;
    ChallengesPage* questMenu = nullptr;
    cocos2d::CCLabelBMFont* volumeSettingsLabel = nullptr;
    cocos2d::CCLabelBMFont* musicVolumeLabel = nullptr;
    cocos2d::CCLabelBMFont* sfxVolumeLabel = nullptr;
    cocos2d::CCLabelBMFont* levelNameLabel = nullptr;
    cocos2d::CCLabelBMFont* levelTypeLabel = nullptr;
    cocos2d::CCLabelBMFont* levelStatusLabel = nullptr;
    TextArea* currentAttemptLabel = nullptr;
    TextArea* currentTimeLabel = nullptr;
    BarBetterShow* normalBarPercentage = nullptr;
    BarBetterShow* practiceBarPercentage = nullptr;
    cocos2d::CCMenu* quickSettingsMenu = nullptr;
    cocos2d::CCLabelBMFont* quickSettingsLabel = nullptr;
    cocos2d::CCLabelBMFont* musicVolumeSettingsLabel = nullptr;
    cocos2d::CCLabelBMFont* sfxVolumeSettingsLabel = nullptr;
    cocos2d::CCLabelBMFont* questsLabel = nullptr;
    cocos2d::CCMenu* volumeSettingsMenuButtons = nullptr;
    CCMenuItemSpriteExtra* musicVolumeButton = nullptr;
    CCMenuItemSpriteExtra* sfxVolumeButton = nullptr;
    TextArea* downloadMusicLabel = nullptr;
    bool areTexturesLoaded = true;
    CustomSongWidget* customSongWidget = nullptr;
    CCMenuItemSpriteExtra* questButtonAlt = nullptr;
    ScrollLayer* buttonsList = nullptr;
    cocos2d::CCSprite* scrollUpButtonSprite = nullptr;
    cocos2d::CCSprite* scrollDownButtonSprite = nullptr;
    CCMenuItemSpriteExtra* scrollUpButton = nullptr;
    CCMenuItemSpriteExtra* scrollDownButton = nullptr;
    float totalButtonsListHeight = 0.f;
    cocos2d::CCLabelBMFont* creatorNameLabel = nullptr;
    SliderPlus* sliderPlusMusic = nullptr;
    SliderPlus* sliderPlusSFX = nullptr;

    static BetterPause* create(PauseLayer*);
    static void clearQuickSettings();
    bool init(PauseLayer*);
    void update(float);
    void createQuestMenu();
    void createMainButtonsMenu();
    void createSecondaryButtonsMenu();
    void createLabels();
    void createBars();
    void createQuickSettingsButtons();
    void createCustomSongWidget();
    void createSectionTitles();
    void createPlatformerTimes();
    void createAudioControls();
    void createAndSetupBar(BarBetterShow*& bar, const cocos2d::ccColor3B& color, bool isVisible,
        float currentPercentage, float targetPercentage, const cocos2d::CCPoint& position, std::string id = "not-a-string");
    void createToggleButton(cocos2d::SEL_MenuHandler callback, bool toggledOn,
        cocos2d::CCMenu* menu, std::string caption, cocos2d::CCPoint position, float fontSize,
        bool twoColumns, int tag, std::string id = "not-a-string");
    void handleOptionsLayers();
    void onOptionsLayer(cocos2d::CCObject* sender);
    void onHide(cocos2d::CCObject* sender);
    void musicSliderChanged(cocos2d::CCObject* sender);
    void sfxSliderChanged(cocos2d::CCObject* sender);
    void onSetMusicVolume(cocos2d::CCObject* sender);
    void onSetSfxVolume(cocos2d::CCObject* sender);
    void createToggleButtonWithGameVariable(const char* key, cocos2d::CCMenu* menu, std::string caption, cocos2d::CCPoint position, float fontSize, bool twoColumns, std::string id = "not-a-string");
    void onToggleWithGameVariable(cocos2d::CCObject* sender);
    void onLevelInfoLayer(cocos2d::CCObject* sender);
    void onRedirectionToggle(cocos2d::CCObject* sender);
    void onEditorHack(cocos2d::CCObject* sender);
    virtual void ccTouchMoved(CCTouch* touch, CCEvent* event) override;
    void onPracticeMusicSyncRedirection(cocos2d::CCObject* sender);
    void findButtonsRecursively(CCNode* node, std::vector<std::string>& buttonIds, std::vector<CCMenuItemSpriteExtra*>& externalButtons);
    void onScrollUpButton(cocos2d::CCObject* sender);
    void onScrollDownButton(cocos2d::CCObject* sender);
    void adjustLayerForAspectRatio();
};

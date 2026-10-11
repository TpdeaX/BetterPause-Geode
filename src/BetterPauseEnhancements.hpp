#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include <string>

using namespace geode::prelude;

class SessionStatsManager {
public:
    static SessionStatsManager* get();

    float m_sessionBest = 0.0f;
    float m_lastDeathPercent = 0.0f;
    CCPoint m_lastDeathPos = {0.f, 0.f};
    bool m_hasLastDeath = false;
    int m_sessionAttempts = 0;
    int m_currentLevelID = -1;
    std::string m_currentLevelName = "";

    void onLevelInit(GJGameLevel* level);
    void onPlayerDeath(float percent, CCPoint pos);
    void reset();
};

namespace FMOD {
    class Channel;
    class Sound;
}

class PauseMusicManager {
public:
    static PauseMusicManager* get();

    bool m_isPlaying = false;
    FMOD::Channel* m_channel = nullptr;
    FMOD::Sound* m_sound = nullptr;

    void onPause();
    void onResume();
    static std::string getTrackFilename(int trackIndex);
};

class LastDeathGhostNode : public CCNode {
public:
    static LastDeathGhostNode* create(CCPoint pos, float percent);
    bool init(CCPoint pos, float percent);

    static void showInPlayLayer(PlayLayer* pl);
    static void removeFromPlayLayer(PlayLayer* pl);
};

class UnpauseCountdownNode : public CCLayer {
public:
    std::function<void()> m_onResumeCallback;
    CCLabelBMFont* m_label = nullptr;
    cocos2d::extension::CCScale9Sprite* m_backdrop = nullptr;
    cocos2d::extension::CCScale9Sprite* m_progressBarTrack = nullptr;
    cocos2d::extension::CCScale9Sprite* m_progressBarFill = nullptr;
    cocos2d::CCSprite* m_orbitRing = nullptr;
    int m_currentCount = 3;
    bool m_finished = false;
    int m_animStyle = 0;
    float m_stepTimer = 0.0f;
    float m_stepInterval = 0.55f;

    static UnpauseCountdownNode* create(std::function<void()> onResumeCallback);
    bool init(std::function<void()> onResumeCallback);
    virtual ~UnpauseCountdownNode();

    virtual void onEnter() override;
    virtual void update(float dt) override;
    virtual void registerWithTouchDispatcher() override;
    virtual bool ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent) override;
    virtual void ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent) override;
    virtual void ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent) override;
    virtual void ccTouchCancelled(CCTouch* pTouch, CCEvent* pEvent) override;
    virtual void keyBackClicked() override;
    virtual void onExit() override;

    void stepCountdown();
    void playStyleTickAnimation();
    void playStyleGoAnimation();
    void createShockwaveRipple(const cocos2d::ccColor3B& color);
    void spawnStarBurst(int count, float radius);
    void finish();
    void cancel();
};

namespace BetterPauseTheme {
    cocos2d::ccColor3B getPlayerCol1();
    cocos2d::ccColor3B getPlayerCol2();
    cocos2d::ccColor4F getPlayerCol1F(float alpha = 1.0f);
    cocos2d::ccColor4F getPlayerCol2F(float alpha = 1.0f);
}

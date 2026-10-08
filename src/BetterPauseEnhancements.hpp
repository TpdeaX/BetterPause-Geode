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

class PauseMusicManager {
public:
    static PauseMusicManager* get();

    bool m_isPlaying = false;
    int m_musicChannel = 1;

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
    int m_currentCount = 3;
    bool m_finished = false;

    static UnpauseCountdownNode* create(std::function<void()> onResumeCallback);
    bool init(std::function<void()> onResumeCallback);

    virtual bool ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent) override;
    virtual void keyBackClicked() override;

    void stepCountdown();
    void finish();
};

namespace BetterPauseTheme {
    cocos2d::ccColor3B getPlayerCol1();
    cocos2d::ccColor3B getPlayerCol2();
    cocos2d::ccColor4F getPlayerCol1F(float alpha = 1.0f);
    cocos2d::ccColor4F getPlayerCol2F(float alpha = 1.0f);
}

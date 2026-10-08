#include "BetterPauseEnhancements.hpp"
#include <fmt/format.h>

using namespace geode::prelude;

// ==========================================
// SessionStatsManager
// ==========================================
static SessionStatsManager* s_sessionStats = nullptr;

SessionStatsManager* SessionStatsManager::get() {
    if (!s_sessionStats) {
        s_sessionStats = new SessionStatsManager();
    }
    return s_sessionStats;
}

void SessionStatsManager::onLevelInit(GJGameLevel* level) {
    if (!level) return;

    int newID = level->m_levelID;
    std::string newName = level->m_levelName;

    if (newID != m_currentLevelID || (newID == 0 && newName != m_currentLevelName)) {
        m_sessionBest = 0.0f;
        m_lastDeathPercent = 0.0f;
        m_lastDeathPos = ccp(0.f, 0.f);
        m_hasLastDeath = false;
        m_sessionAttempts = 0;
        m_currentLevelID = newID;
        m_currentLevelName = newName;
    }
    m_sessionAttempts++;
}

void SessionStatsManager::onPlayerDeath(float percent, CCPoint pos) {
    m_lastDeathPercent = std::clamp(percent, 0.0f, 100.0f);
    m_lastDeathPos = pos;
    m_hasLastDeath = true;
    if (m_lastDeathPercent > m_sessionBest) {
        m_sessionBest = m_lastDeathPercent;
    }
}

void SessionStatsManager::reset() {
    m_sessionBest = 0.0f;
    m_lastDeathPercent = 0.0f;
    m_lastDeathPos = ccp(0.f, 0.f);
    m_hasLastDeath = false;
    m_sessionAttempts = 0;
    m_currentLevelID = -1;
    m_currentLevelName = "";
}

// ==========================================
// PauseMusicManager
// ==========================================
static PauseMusicManager* s_pauseMusic = nullptr;

PauseMusicManager* PauseMusicManager::get() {
    if (!s_pauseMusic) {
        s_pauseMusic = new PauseMusicManager();
    }
    return s_pauseMusic;
}

std::string PauseMusicManager::getTrackFilename(int trackIndex) {
    switch (trackIndex) {
        case 1: return "StayInsideMe.mp3";
        case 2: return "secretLoop.mp3";
        case 3: return "secretLoop02.mp3";
        case 4: return "shop.mp3";
        case 5: return "secretShop.mp3";
        default: return "";
    }
}

void PauseMusicManager::onPause() {
    int trackIdx = static_cast<int>(Mod::get()->getSettingValue<int64_t>("pause-music-track"));
    if (trackIdx <= 0) return;

    std::string track = getTrackFilename(trackIdx);
    if (track.empty()) return;

    float vol = static_cast<float>(Mod::get()->getSettingValue<double>("pause-music-volume"));
    if (vol <= 0.0f) return;

    auto fmod = FMODAudioEngine::sharedEngine();
    if (!fmod) return;

    fmod->playMusic(track, true, 0.35f, m_musicChannel);
    fmod->setChannelVolume(m_musicChannel, AudioTargetType::MusicChannel, vol);
    m_isPlaying = true;
}

void PauseMusicManager::onResume() {
    if (m_isPlaying) {
        auto fmod = FMODAudioEngine::sharedEngine();
        if (fmod) {
            fmod->fadeOutMusic(0.25f, m_musicChannel);
            fmod->stopChannel(m_musicChannel);
        }
        m_isPlaying = false;
    }
}

// ==========================================
// LastDeathGhostNode (Only shown inside Pause Menu)
// ==========================================
LastDeathGhostNode* LastDeathGhostNode::create(CCPoint pos, float percent) {
    auto ret = new LastDeathGhostNode();
    if (ret && ret->init(pos, percent)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool LastDeathGhostNode::init(CCPoint pos, float percent) {
    if (!CCNode::init()) return false;

    this->setPosition(pos);

    // Skull sprite
    CCSprite* skull = CCSprite::createWithSpriteFrameName("d_skull_01_001.png");
    if (!skull) {
        skull = CCSprite::createWithSpriteFrameName("edit_downBtn2_001.png");
    }
    if (skull) {
        skull->setColor({ 255, 80, 80 });
        skull->setOpacity(200);
        skull->setScale(0.8f);
        this->addChild(skull);

        // Ambient pulse animation
        auto pulse = CCRepeatForever::create(CCSequence::create(
            CCEaseSineInOut::create(CCScaleTo::create(0.9f, 0.90f)),
            CCEaseSineInOut::create(CCScaleTo::create(0.9f, 0.72f)),
            nullptr
        ));
        skull->runAction(pulse);
    }

    // Percentage badge
    auto label = CCLabelBMFont::create(fmt::format("{:.0f}%", percent).c_str(), "goldFont.fnt");
    if (label) {
        label->setScale(0.42f);
        label->setPosition({ 0.f, 22.f });
        this->addChild(label);
    }

    return true;
}

void LastDeathGhostNode::showInPlayLayer(PlayLayer* pl) {
    if (!pl) return;
    if (!Mod::get()->getSettingValue<bool>("enable-death-marker")) return;

    auto stats = SessionStatsManager::get();
    if (!stats->m_hasLastDeath) return;

    if (pl->getChildByID("last-death-ghost-node")) return;

    auto ghost = LastDeathGhostNode::create(stats->m_lastDeathPos, stats->m_lastDeathPercent);
    if (ghost) {
        ghost->setID("last-death-ghost-node");
        pl->addChild(ghost, 9999);
    }
}

void LastDeathGhostNode::removeFromPlayLayer(PlayLayer* pl) {
    if (!pl) return;
    if (auto ghost = pl->getChildByID("last-death-ghost-node")) {
        ghost->removeFromParent();
    }
}

// ==========================================
// UnpauseCountdownNode
// ==========================================
UnpauseCountdownNode* UnpauseCountdownNode::create(std::function<void()> onResumeCallback) {
    auto ret = new UnpauseCountdownNode();
    if (ret && ret->init(onResumeCallback)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool UnpauseCountdownNode::init(std::function<void()> onResumeCallback) {
    if (!CCLayer::init()) return false;

    m_onResumeCallback = onResumeCallback;
    m_currentCount = 3;
    m_finished = false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    this->setContentSize(winSize);
    this->setPosition(ccp(0.f, 0.f));

    m_label = CCLabelBMFont::create("3", "goldFont.fnt");
    m_label->setScale(0.0f);
    m_label->setPosition(ccp(winSize.width * 0.5f, winSize.height * 0.5f));
    this->addChild(m_label);

    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -99999, true);

    stepCountdown();
    return true;
}

bool UnpauseCountdownNode::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent) {
    finish();
    return true;
}

void UnpauseCountdownNode::keyBackClicked() {
    finish();
}

void UnpauseCountdownNode::stepCountdown() {
    if (m_finished) return;

    if (m_currentCount > 0) {
        m_label->setString(fmt::format("{}", m_currentCount).c_str());
        m_label->setScale(0.3f);
        m_label->runAction(CCEaseBackOut::create(CCScaleTo::create(0.22f, 1.6f)));

        if (auto fmod = FMODAudioEngine::sharedEngine()) {
            fmod->playEffect("quitSound_01.ogg", 1.0f, 0.0f, 0.6f);
        }

        m_currentCount--;
        this->runAction(CCSequence::create(
            CCDelayTime::create(0.55f),
            CCCallFunc::create(this, callfunc_selector(UnpauseCountdownNode::stepCountdown)),
            nullptr
        ));
    } else if (m_currentCount == 0) {
        m_label->setString("GO!");
        m_label->setScale(0.4f);
        m_label->setColor({ 100, 255, 120 });
        m_label->runAction(CCEaseBackOut::create(CCScaleTo::create(0.20f, 1.8f)));

        if (auto fmod = FMODAudioEngine::sharedEngine()) {
            fmod->playEffect("gold02.ogg", 1.0f, 0.0f, 0.8f);
        }

        m_currentCount = -1;
        this->runAction(CCSequence::create(
            CCDelayTime::create(0.25f),
            CCCallFunc::create(this, callfunc_selector(UnpauseCountdownNode::finish)),
            nullptr
        ));
    }
}

void UnpauseCountdownNode::finish() {
    if (m_finished) return;
    m_finished = true;
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    this->stopAllActions();
    if (m_onResumeCallback) {
        m_onResumeCallback();
    }
    this->removeFromParent();
}

// ==========================================
// BetterPauseTheme
// ==========================================
namespace BetterPauseTheme {
    cocos2d::ccColor3B getPlayerCol1() {
        if (Mod::get()->getSettingValue<bool>("enable-player-theme")) {
            if (auto gm = GameManager::sharedState()) {
                return gm->colorForIdx(gm->getPlayerColor());
            }
        }
        return { 0, 255, 0 };
    }

    cocos2d::ccColor3B getPlayerCol2() {
        if (Mod::get()->getSettingValue<bool>("enable-player-theme")) {
            if (auto gm = GameManager::sharedState()) {
                return gm->colorForIdx(gm->getPlayerColor2());
            }
        }
        return { 0, 255, 255 };
    }

    cocos2d::ccColor4F getPlayerCol1F(float alpha) {
        auto c = getPlayerCol1();
        return ccc4f(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, alpha);
    }

    cocos2d::ccColor4F getPlayerCol2F(float alpha) {
        auto c = getPlayerCol2();
        return ccc4f(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, alpha);
    }
}

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

#include <Geode/fmod/fmod.hpp>

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
        case 1: return "menuLoop.mp3";
        case 2: return "StayInsideMe.mp3";
        case 3: return "secretLoop.mp3";
        case 4: return "secretLoop02.mp3";
        case 5: return "shop.mp3";
        case 6: return "secretShop.mp3";
        default: return "";
    }
}

void PauseMusicManager::onPause() {
    // If third-party PauseMenuLoop mod is active, don't double play
    if (Loader::get()->isModLoaded("raydeeux.pausemenuloop") &&
        Loader::get()->getLoadedMod("raydeeux.pausemenuloop")->getSettingValue<bool>("enabled")) {
        return;
    }

    int trackIdx = static_cast<int>(Mod::get()->getSettingValue<int64_t>("pause-music-track"));
    if (trackIdx <= 0) return;

    std::string track = getTrackFilename(trackIdx);
    if (track.empty()) return;

    float vol = static_cast<float>(Mod::get()->getSettingValue<double>("pause-music-volume"));
    if (vol <= 0.0f) return;

    auto fmodEngine = FMODAudioEngine::sharedEngine();
    if (!fmodEngine || !fmodEngine->m_system) return;

    std::string fullPath = cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename(track.c_str(), false);
    if (fullPath.empty()) {
        fullPath = track;
    }

    this->onResume();

    FMOD_RESULT res = fmodEngine->m_system->createSound(fullPath.c_str(), FMOD_LOOP_NORMAL, nullptr, &m_sound);
    if (res == FMOD_OK && m_sound) {
        m_sound->setLoopCount(-1);
        fmodEngine->m_system->playSound(m_sound, nullptr, false, &m_channel);
        if (m_channel) {
            m_channel->setVolume(vol);
            m_isPlaying = true;
        }
    }
}

void PauseMusicManager::onResume() {
    if (m_channel) {
        m_channel->stop();
        m_channel = nullptr;
    }
    if (m_sound) {
        m_sound->release();
        m_sound = nullptr;
    }
    m_isPlaying = false;
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
    m_animStyle = static_cast<int>(Mod::get()->getSettingValue<int64_t>("countdown-animation-style"));

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    this->setContentSize(winSize);
    this->setPosition(ccp(0.f, 0.f));

    CCPoint center = ccp(winSize.width * 0.5f, winSize.height * 0.5f);

    if (m_animStyle == 4) {
        m_backdrop = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
        if (m_backdrop) {
            m_backdrop->setContentSize({ 136.f, 64.f });
            m_backdrop->setColor({ 15, 15, 18 });
            m_backdrop->setOpacity(190);
            m_backdrop->setPosition(center);
            this->addChild(m_backdrop, 0);
        }

        m_progressBarTrack = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
        if (m_progressBarTrack) {
            m_progressBarTrack->setContentSize({ 96.f, 4.5f });
            m_progressBarTrack->setColor({ 40, 40, 45 });
            m_progressBarTrack->setOpacity(210);
            m_progressBarTrack->setPosition(ccp(center.x, center.y - 19.f));
            this->addChild(m_progressBarTrack, 1);
        }

        m_progressBarFill = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
        if (m_progressBarFill) {
            m_progressBarFill->setContentSize({ 96.f, 4.5f });
            m_progressBarFill->setScaleX(0.0f);
            m_progressBarFill->setColor({ 255, 200, 50 });
            m_progressBarFill->setAnchorPoint(ccp(0.f, 0.5f));
            m_progressBarFill->setPosition(ccp(center.x - 48.f, center.y - 19.f));
            this->addChild(m_progressBarFill, 2);
        }
    } else {
        m_backdrop = cocos2d::extension::CCScale9Sprite::create("square02_small.png");
        if (m_backdrop) {
            m_backdrop->setContentSize({ 95.f, 95.f });
            if (m_animStyle == 1) {
                m_backdrop->setColor({ 12, 22, 28 });
                m_backdrop->setOpacity(180);
            } else {
                m_backdrop->setColor({ 0, 0, 0 });
                m_backdrop->setOpacity(140);
            }
            m_backdrop->setPosition(center);
            this->addChild(m_backdrop, 0);
        }

        if (m_animStyle == 3) {
            m_orbitRing = CCSprite::createWithSpriteFrameName("GJ_selectRing_001.png");
            if (!m_orbitRing) {
                m_orbitRing = CCSprite::createWithSpriteFrameName("GJ_compactCoin_001.png");
            }
            if (m_orbitRing) {
                m_orbitRing->setPosition(center);
                m_orbitRing->setScale(1.22f);
                m_orbitRing->setColor({ 0, 240, 255 });
                m_orbitRing->setOpacity(185);
                m_orbitRing->runAction(CCRepeatForever::create(CCRotateBy::create(2.4f, 360.f)));
                this->addChild(m_orbitRing, 1);
            }
        }
    }

    m_label = CCLabelBMFont::create("3", "goldFont.fnt");
    m_label->setScale(0.0f);
    m_label->setPosition(center);
    this->addChild(m_label, 3);

    this->setTouchEnabled(true);
    this->setKeypadEnabled(true);

    stepCountdown();
    return true;
}

void UnpauseCountdownNode::registerWithTouchDispatcher() {
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -999999, true);
}

bool UnpauseCountdownNode::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent) {
    return true;
}

void UnpauseCountdownNode::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent) {}
void UnpauseCountdownNode::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent) {}
void UnpauseCountdownNode::ccTouchCancelled(CCTouch* pTouch, CCEvent* pEvent) {}

void UnpauseCountdownNode::keyBackClicked() {}

void UnpauseCountdownNode::onExit() {
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    CCLayer::onExit();
}

void UnpauseCountdownNode::createShockwaveRipple(const cocos2d::ccColor3B& color) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    CCPoint center = ccp(winSize.width * 0.5f, winSize.height * 0.5f);

    auto ripple = CCSprite::createWithSpriteFrameName("GJ_selectRing_001.png");
    if (!ripple) {
        ripple = CCSprite::createWithSpriteFrameName("GJ_compactCoin_001.png");
    }
    if (ripple) {
        ripple->setPosition(center);
        ripple->setColor(color);
        ripple->setScale(0.7f);
        ripple->setOpacity(230);
        this->addChild(ripple, 2);

        ripple->runAction(CCSequence::create(
            CCSpawn::create(
                CCEaseSineOut::create(CCScaleTo::create(0.48f, 2.5f)),
                CCFadeOut::create(0.48f),
                nullptr
            ),
            CCCallFunc::create(ripple, callfunc_selector(CCNode::removeFromParent)),
            nullptr
        ));
    }
}

void UnpauseCountdownNode::spawnStarBurst(int count, float radius) {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    CCPoint center = ccp(winSize.width * 0.5f, winSize.height * 0.5f);

    float angleStep = (2.0f * 3.14159265f) / static_cast<float>(count);

    for (int i = 0; i < count; i++) {
        float angle = angleStep * static_cast<float>(i);
        float destX = std::cos(angle) * radius;
        float destY = std::sin(angle) * radius;

        auto star = CCSprite::createWithSpriteFrameName("star_small01_001.png");
        if (!star) {
            star = CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
        }
        if (star) {
            star->setPosition(center);
            star->setScale(0.5f);
            star->setOpacity(255);
            star->setColor({ 255, 235, 100 });
            this->addChild(star, 4);

            star->runAction(CCSequence::create(
                CCSpawn::create(
                    CCEaseSineOut::create(CCMoveBy::create(0.42f, ccp(destX, destY))),
                    CCRotateBy::create(0.42f, 360.f),
                    CCFadeOut::create(0.42f),
                    CCScaleTo::create(0.42f, 0.1f),
                    nullptr
                ),
                CCCallFunc::create(star, callfunc_selector(CCNode::removeFromParent)),
                nullptr
            ));
        }
    }
}

void UnpauseCountdownNode::playStyleTickAnimation() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    CCPoint center = ccp(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::ccColor3B tickColor = { 255, 230, 80 };
    if (m_currentCount == 3) {
        tickColor = { 255, 80, 80 };
    } else if (m_currentCount == 2) {
        tickColor = { 255, 195, 45 };
    } else if (m_currentCount == 1) {
        tickColor = { 120, 255, 75 };
    }

    m_label->setString(fmt::format("{}", m_currentCount).c_str());
    m_label->setColor(tickColor);
    m_label->stopAllActions();
    m_label->setPosition(center);
    m_label->setRotation(0.f);

    switch (m_animStyle) {
        case 1: {
            m_label->setScale(0.7f);
            m_label->setColor({ 255, 255, 255 });

            m_label->runAction(CCSequence::create(
                CCEaseBackOut::create(CCScaleTo::create(0.12f, 1.8f)),
                CCTintTo::create(0.06f, tickColor.r, tickColor.g, tickColor.b),
                CCMoveBy::create(0.03f, ccp(4.f, -3.f)),
                CCMoveBy::create(0.03f, ccp(-6.f, 5.f)),
                CCMoveBy::create(0.03f, ccp(2.f, -2.f)),
                CCMoveTo::create(0.02f, center),
                nullptr
            ));

            if (m_backdrop) {
                m_backdrop->setColor({ 20, 45, 55 });
                m_backdrop->runAction(CCSequence::create(
                    CCScaleTo::create(0.06f, 1.15f),
                    CCScaleTo::create(0.12f, 1.0f),
                    nullptr
                ));
            }
            break;
        }

        case 2: {
            m_label->setScale(3.5f);
            m_label->setOpacity(40);
            m_label->runAction(CCSpawn::create(
                CCEaseExponentialIn::create(CCScaleTo::create(0.16f, 1.6f)),
                CCFadeIn::create(0.14f),
                nullptr
            ));

            if (m_backdrop) {
                m_backdrop->setScale(0.85f);
                m_backdrop->runAction(CCEaseBackOut::create(CCScaleTo::create(0.20f, 1.0f)));
            }

            this->spawnStarBurst(4, 38.f);
            break;
        }

        case 3: {
            m_label->setScale(0.3f);
            m_label->setRotation(-60.f);
            m_label->runAction(CCSpawn::create(
                CCEaseBackOut::create(CCScaleTo::create(0.20f, 1.7f)),
                CCEaseBackOut::create(CCRotateTo::create(0.20f, 0.f)),
                nullptr
            ));

            if (m_orbitRing) {
                m_orbitRing->stopActionByTag(88);
                auto snap = CCEaseBackOut::create(CCScaleTo::create(0.18f, 1.45f));
                auto returnBack = CCEaseSineOut::create(CCScaleTo::create(0.22f, 1.22f));
                auto seq = CCSequence::create(snap, returnBack, nullptr);
                seq->setTag(88);
                m_orbitRing->runAction(seq);
            }
            break;
        }

        case 4: {
            m_label->setScale(1.2f);
            m_label->setPositionY(center.y - 8.f);
            m_label->setOpacity(60);
            m_label->runAction(CCSpawn::create(
                CCEaseSineOut::create(CCMoveTo::create(0.18f, center)),
                CCFadeIn::create(0.16f),
                nullptr
            ));

            if (m_progressBarFill) {
                float targetFraction = (4 - m_currentCount) / 3.0f;
                m_progressBarFill->setColor(tickColor);
                m_progressBarFill->runAction(CCEaseSineOut::create(CCScaleTo::create(0.20f, targetFraction, 1.0f)));
            }
            break;
        }

        case 0:
        default: {
            m_label->setScale(0.25f);
            m_label->runAction(CCEaseBackOut::create(CCScaleTo::create(0.20f, 1.75f)));

            if (m_backdrop) {
                m_backdrop->setScale(0.85f);
                m_backdrop->runAction(CCEaseBackOut::create(CCScaleTo::create(0.20f, 1.0f)));
            }

            this->createShockwaveRipple(tickColor);
            break;
        }
    }
}

void UnpauseCountdownNode::playStyleGoAnimation() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    CCPoint center = ccp(winSize.width * 0.5f, winSize.height * 0.5f);

    cocos2d::ccColor3B goColor = { 70, 255, 160 };

    m_label->setString("GO!");
    m_label->setColor(goColor);
    m_label->stopAllActions();
    m_label->setPosition(center);
    m_label->setRotation(0.f);

    switch (m_animStyle) {
        case 1: {
            m_label->setScale(0.5f);
            m_label->runAction(CCSequence::create(
                CCEaseBackOut::create(CCScaleTo::create(0.14f, 2.2f)),
                CCTintTo::create(0.06f, 255, 255, 255),
                CCTintTo::create(0.06f, goColor.r, goColor.g, goColor.b),
                nullptr
            ));

            if (m_backdrop) {
                m_backdrop->runAction(CCSequence::create(
                    CCScaleTo::create(0.10f, 1.25f),
                    CCScaleTo::create(0.12f, 1.1f),
                    nullptr
                ));
            }
            break;
        }

        case 2: {
            m_label->setScale(3.8f);
            m_label->setOpacity(60);
            m_label->runAction(CCSpawn::create(
                CCEaseBackOut::create(CCScaleTo::create(0.20f, 2.05f)),
                CCFadeIn::create(0.15f),
                nullptr
            ));

            if (m_backdrop) {
                m_backdrop->runAction(CCEaseBackOut::create(CCScaleTo::create(0.20f, 1.2f)));
            }

            this->spawnStarBurst(10, 80.f);

            this->runAction(CCSequence::create(
                CCMoveBy::create(0.04f, ccp(0.f, 3.5f)),
                CCMoveBy::create(0.04f, ccp(0.f, -4.5f)),
                CCMoveTo::create(0.03f, ccp(0.f, 0.f)),
                nullptr
            ));
            break;
        }

        case 3: {
            m_label->setScale(0.4f);
            m_label->setRotation(-90.f);
            m_label->runAction(CCSpawn::create(
                CCEaseBackOut::create(CCScaleTo::create(0.20f, 2.0f)),
                CCEaseBackOut::create(CCRotateTo::create(0.20f, 0.f)),
                nullptr
            ));

            if (m_orbitRing) {
                m_orbitRing->runAction(CCSpawn::create(
                    CCEaseSineOut::create(CCScaleTo::create(0.25f, 2.3f)),
                    CCFadeOut::create(0.25f),
                    nullptr
                ));
            }
            break;
        }

        case 4: {
            m_label->setScale(1.4f);
            m_label->setPositionY(center.y - 10.f);
            m_label->runAction(CCSpawn::create(
                CCEaseBackOut::create(CCMoveTo::create(0.18f, center)),
                CCEaseBackOut::create(CCScaleTo::create(0.18f, 1.7f)),
                nullptr
            ));

            if (m_progressBarFill) {
                m_progressBarFill->setColor(goColor);
                m_progressBarFill->runAction(CCScaleTo::create(0.16f, 1.0f, 1.0f));
            }
            break;
        }

        case 0:
        default: {
            m_label->setScale(0.35f);
            m_label->runAction(CCEaseBackOut::create(CCScaleTo::create(0.20f, 2.0f)));

            if (m_backdrop) {
                m_backdrop->runAction(CCEaseBackOut::create(CCScaleTo::create(0.20f, 1.15f)));
            }

            this->createShockwaveRipple(goColor);
            break;
        }
    }
}

void UnpauseCountdownNode::stepCountdown() {
    if (m_finished) return;

    if (m_currentCount > 0) {
        this->playStyleTickAnimation();

        if (auto fmod = FMODAudioEngine::sharedEngine()) {
            fmod->playEffect("quitSound_01.ogg", 1.0f, 0.0f, 0.65f);
        }

        m_currentCount--;
        this->runAction(CCSequence::create(
            CCDelayTime::create(0.55f),
            CCCallFunc::create(this, callfunc_selector(UnpauseCountdownNode::stepCountdown)),
            nullptr
        ));
    } else if (m_currentCount == 0) {
        this->playStyleGoAnimation();

        if (auto fmod = FMODAudioEngine::sharedEngine()) {
            fmod->playEffect("gold02.ogg", 1.0f, 0.0f, 0.85f);
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

    auto cb = m_onResumeCallback;
    m_onResumeCallback = nullptr;
    this->removeFromParent();

    if (cb) {
        cb();
    }
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

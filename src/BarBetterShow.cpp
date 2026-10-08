#include "BarBetterShow.hpp"

BarBetterShow* BarBetterShow::create(cocos2d::ccColor3B color, bool enabledSecondBar, bool showCurrentPer, float perCurrent, float bestCurrent)
{
	auto ret = new BarBetterShow();

	if (ret && ret->init(color, enabledSecondBar, showCurrentPer, perCurrent, bestCurrent))
	{
		ret->autorelease();
		return ret;
	}

	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool BarBetterShow::init(cocos2d::ccColor3B color, bool enabledSecondBar, bool showCurrentPer, float perCurrent, float bestCurrent)
{
	if (!cocos2d::CCNode::init())
	{
		return false;
	}

	m_pBarBase = cocos2d::CCSprite::create("GJ_progressBar_001.png");
	m_pBarBase->setVisible(false);
	m_pBarBase->setAnchorPoint({ 0.f, 0.5f });
	this->addChild(m_pBarBase);

	m_pBarMain = cocos2d::CCSprite::create("GJ_progressBar_001.png");
	this->updateBar(m_pBarMain, perCurrent);
	m_pBarMain->setColor(color);
	m_pBarMain->setAnchorPoint({ 0.f, 0.5f });
	this->addChild(m_pBarMain);

	m_pBarBG = cocos2d::CCSprite::create("GJ_progressBar_001.png");
	this->updateBar(m_pBarBG, bestCurrent);
	m_pBarBG->setColor(color);
	m_pBarBG->setOpacity(Utils::convertOpacitySimplf(0.4f));
	m_pBarBG->setAnchorPoint({ 0.f, 0.5f });
	this->addChild(m_pBarBG);

	float rightPosXCurrentPer = 1.f + m_pBarBase->getContentSize().width * std::min(std::max(perCurrent / 100.f, 0.f), 100.f);
	float rightPosXBestPer = 1.f + m_pBarBase->getContentSize().width * std::min(std::max(bestCurrent / 100.f, 0.f), 100.f);

	m_pPercentageCurrent = cocos2d::CCLabelBMFont::create(
		cocos2d::CCString::createWithFormat("%i%%", static_cast<int>(perCurrent))->getCString(), "bigFont.fnt");
	m_pPercentageCurrent->setScale(0.7f);
	m_pPercentageCurrent->setPosition({ rightPosXCurrentPer, -25.f });
	m_pPercentageCurrent->setVisible(showCurrentPer);
	this->addChild(m_pPercentageCurrent);

	m_pPercentageBest = cocos2d::CCLabelBMFont::create(
		cocos2d::CCString::createWithFormat("%i%% Best", static_cast<int>(bestCurrent))->getCString(), "goldFont.fnt");
	m_pPercentageBest->setScale(0.7f);
	m_pPercentageBest->setAnchorPoint({ 1.f, 0.5f });
	m_pPercentageBest->setPosition({ rightPosXBestPer, -25.f });
	this->addChild(m_pPercentageBest);

	m_pPercentageMax = cocos2d::CCLabelBMFont::create(
		cocos2d::CCString::createWithFormat("%i%%", static_cast<int>(100))->getCString(), "bigFont.fnt");
	m_pPercentageMax->setScale(0.7f);
	m_pPercentageMax->setOpacity(Utils::convertOpacitySimplf(0.4f));
	m_pPercentageMax->setPosition({ 340.f, -25.f });
	this->addChild(m_pPercentageMax);

	auto currentBox = m_pPercentageCurrent->boundingBox();
	auto bestBox = m_pPercentageBest->boundingBox();
	auto maxBox = m_pPercentageMax->boundingBox();

	if ((currentBox.getMaxX() >= bestBox.getMinX()) && m_pPercentageBest->isVisible() && m_pPercentageCurrent->isVisible())
	{
		m_pPercentageBest->setPositionY(25.f);
	}
	if (currentBox.getMaxX() >= maxBox.getMinX() && m_pPercentageCurrent->isVisible())
	{
		m_pPercentageMax->setPositionY(25.f);
	}

	if (bestCurrent >= 100.f && currentBox.getMaxX() >= (maxBox.getMinX() - 80.f))
	{
		m_pPercentageMax->setPositionY(25.f);
	}

	if ((bestBox.getMaxX() >= maxBox.getMinX()) && m_pPercentageBest->isVisible())
	{
		if (m_pPercentageMax->getPositionY() == 25.f)
		{
			m_pPercentageBest->setAnchorPoint({ 1.f, 0.5f });
			m_pPercentageBest->setPosition(m_pPercentageMax->getPosition());
			m_pPercentageBest->setPositionX(m_pPercentageMax->getPositionX() - m_pPercentageMax->getContentSize().width);
		}
		else
		{
			m_pPercentageBest->setPositionY(25.f);
		}
	}

	if (bestCurrent >= 100.f)
	{
		m_pPercentageMax->setFntFile("goldFont.fnt");
		m_pPercentageMax->setScale(0.7f);
		m_pPercentageMax->setOpacity(Utils::convertOpacitySimplf(1.f));
		m_pPercentageMax->setString("100% Best");
		m_pPercentageMax->setAnchorPoint({ 1.f, 0.5f });
		m_pPercentageBest->setVisible(false);
	}

	if (perCurrent >= bestCurrent)
	{
		m_pPercentageCurrent->setFntFile("goldFont.fnt");
		m_pPercentageCurrent->setScale(0.7f);
		m_pPercentageBest->setVisible(false);
	}

	if (!enabledSecondBar)
	{
		m_pBarMain->setVisible(false);
		m_pBarBG->setOpacity(Utils::convertOpacitySimplf(1.f));
		m_pPercentageCurrent->setVisible(false);
		if (m_pPercentageMax->getOpacity() != 255)
		{
			m_pPercentageBest->setVisible(true);
			m_pPercentageBest->setAnchorPoint({ 0.5f, 0.5f });
		}
	}

	if (bestCurrent <= 30.f)
	{
		m_pPercentageBest->setAnchorPoint({ 0.f, 0.5f });
	}

	return true;
}

void BarBetterShow::updateBar(cocos2d::CCSprite* m_pBar, float per)
{
	auto mProgressBarSize = cocos2d::CCSize(m_pBarBase->getTextureRect().size.width - 4.0f, 8.0f);
	float width = mProgressBarSize.width;
	if (per >= 100.f)
	{
		return;
	}
	width = width * static_cast<float>(per) / 100.0f;
	m_pBar->setTextureRect(cocos2d::CCRect(0.0f, 0.0f, width, m_pBarBase->getTextureRect().size.height));
}

void BarBetterShow::addMilestones(float sessionBest, float lastDeathPercent, const std::vector<GameObject*>& coins) {
	if (!m_pBarBase) return;
	float barWidth = m_pBarBase->getContentSize().width;
	if (barWidth <= 0.0f) return;

	// 1. Session Best Marker (on top of the bar)
	if (Mod::get()->getSettingValue<bool>("enable-progress-milestones") && sessionBest > 0.0f && sessionBest <= 100.0f) {
		auto marker = cocos2d::CCSprite::createWithSpriteFrameName("checkpoint_01_001.png");
		if (marker) {
			marker->setScale(0.42f);
			float markerX = (sessionBest / 100.0f) * barWidth;
			marker->setPosition({ markerX, 13.0f });
			marker->setColor({ 255, 220, 90 });
			marker->setOpacity(230);

			auto floatAction = cocos2d::CCRepeatForever::create(cocos2d::CCSequence::create(
				cocos2d::CCEaseSineInOut::create(cocos2d::CCMoveBy::create(1.2f, ccp(0.f, 2.0f))),
				cocos2d::CCEaseSineInOut::create(cocos2d::CCMoveBy::create(1.2f, ccp(0.f, -2.0f))),
				nullptr
			));
			marker->runAction(floatAction);
			this->addChild(marker, 10);
		}
	}

	// 2. Coin Locations on the bar
	if (Mod::get()->getSettingValue<bool>("enable-progress-milestones") && PlayLayer::get()) {
		float levelLength = PlayLayer::get()->m_levelLength;
		if (levelLength > 0.0f) {
			for (auto coin : coins) {
				if (!coin) continue;
				float coinPct = std::clamp((coin->getPositionX() / levelLength) * 100.0f, 0.0f, 100.0f);
				auto coinSpr = cocos2d::CCSprite::createWithSpriteFrameName("secretCoin_01_001.png");
				if (!coinSpr) {
					coinSpr = cocos2d::CCSprite::createWithSpriteFrameName("secretCoin_2_01_001.png");
				}
				if (coinSpr) {
					coinSpr->setScale(0.32f);
					coinSpr->setPosition({ (coinPct / 100.0f) * barWidth, 12.0f });
					coinSpr->setOpacity(210);
					this->addChild(coinSpr, 9);
				}
			}
		}
	}

	// 3. Last Death Marker (below the bar)
	if (Mod::get()->getSettingValue<bool>("enable-death-marker") && lastDeathPercent > 0.0f && lastDeathPercent <= 100.0f) {
		auto deathSpr = cocos2d::CCSprite::createWithSpriteFrameName("d_skull_01_001.png");
		if (!deathSpr) {
			deathSpr = cocos2d::CCSprite::createWithSpriteFrameName("edit_downBtn2_001.png");
		}
		if (deathSpr) {
			deathSpr->setScale(0.38f);
			deathSpr->setPosition({ (lastDeathPercent / 100.0f) * barWidth, -13.0f });
			deathSpr->setColor({ 255, 90, 90 });
			deathSpr->setOpacity(220);

			auto pulseAction = cocos2d::CCRepeatForever::create(cocos2d::CCSequence::create(
				cocos2d::CCEaseSineInOut::create(cocos2d::CCScaleTo::create(0.8f, 0.44f)),
				cocos2d::CCEaseSineInOut::create(cocos2d::CCScaleTo::create(0.8f, 0.35f)),
				nullptr
			));
			deathSpr->runAction(pulseAction);
			this->addChild(deathSpr, 10);
		}
	}
}
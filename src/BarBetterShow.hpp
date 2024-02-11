#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include "Utils.hpp"

using namespace geode::prelude;

class BarBetterShow : public CCNode
{
public:
	cocos2d::CCSprite* m_pBarBase = nullptr;
	cocos2d::CCSprite* m_pBarMain = nullptr;
	float m_pBarMainW = 0.f;
	cocos2d::CCSprite* m_pBarBG = nullptr;
	cocos2d::CCLabelBMFont* m_pPercentageCurrent = nullptr;
	cocos2d::CCLabelBMFont* m_pPercentageBest = nullptr;
	cocos2d::CCLabelBMFont* m_pPercentageMax = nullptr;

	static BarBetterShow* create(cocos2d::ccColor3B color, bool enabledSecondBar, bool showCurrentPer, float perCurrent, float bestCurrent);
	bool init(cocos2d::ccColor3B color, bool enabledSecondBar, bool showCurrentPer, float perCurrent, float bestCurrent);
	void updateBar(cocos2d::CCSprite* m_pBar, float per);
};
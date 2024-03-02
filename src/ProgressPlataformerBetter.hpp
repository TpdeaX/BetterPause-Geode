#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include "Utils.hpp"
#include "BetterInfoUtils.hpp"

using namespace geode::prelude;

class ProgressPlataformerBetter : public CCNode
{
public:


	cocos2d::CCSprite* m_clockSprite = nullptr;
	cocos2d::CCSprite* m_diamondsSprite = nullptr;
	TextArea* m_timeLabelLevel = nullptr;
	TextArea* m_pointsLabelLevel = nullptr;
	static int m_totalPoints;
	static float timeForLevelStringPlataformerSafe;

	static ProgressPlataformerBetter* create();
	bool init();
};
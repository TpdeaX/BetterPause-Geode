#include "Utils.hpp"


namespace Utils
{
	const char* getNameObj(cocos2d::CCNode* obj)
	{
		return (typeid(*obj).name() + 6);
	}

	bool returnZeroBool(int i)
	{
		return i == 0;
	}
	FMODAudioEngine* shareFMOD() { return FMODAudioEngine::sharedEngine(); }
	GameManager* shareManager() { return GameManager::sharedState(); }

	PlayLayer* getplayLayerA() { return shareManager()->getPlayLayer(); }
	GLubyte convertOpacitySimplf(float opaTM) { return static_cast<GLubyte>(255 * opaTM); }

	cocos2d::CCDirector* shareDirectorA() { return cocos2d::CCDirector::sharedDirector(); }

	float getPercentagePerXpos()
	{
		if (auto playLayer = Utils::getplayLayerA()) {
			return playLayer->getCurrentPercent();
		}
		return 0.0f;
	}

	float getPercentageReversePerXpos()
	{
		return 0;(getplayLayerA()->m_levelLength - getplayLayerA()->m_player1->getPositionX()) / getplayLayerA()->m_levelLength * 100;
	}

	float getPercentageNow()
	{
		return getPercentagePerXpos();
	}
	float getPercentageReverseNow()
	{
		return getPercentageReversePerXpos();
	}

	float getPercentageNowFix() { return std::min(getPercentageNow(), 100.f); }
	float getPercentageReverseNowFix() { return std::max(getPercentageReverseNow(), 0.f); }

	float getPercentagePerTime()
	{
		return 0.f;
	}



	void updatePercentageObjects()
	{
		//Utils::getplayLayerA()->m_percentLabel->setVisible(Utils::shareManager()->getGameVariable("0040"));
		//Utils::getplayLayerA()->m_sliderBarSprite->getParent()->setVisible(Utils::shareManager()->m_showProgressBar);

		float ypos = cocos2d::CCDirector::sharedDirector()->getWinSize().height - 8.f;

		if (Utils::shareManager()->m_showProgressBar)
		{
			//Utils::getplayLayerA()->m_sliderBarSprite->getParent()->setPositionY(ypos);
			//float xPer = (CCDirector::sharedDirector()->getWinSize().width / 2.f + (110.f * Utils::getplayLayerA()->m_sliderBarSprite->getParent()->getScale()));
			//float yPer = ypos;
			//Utils::getplayLayerA()->m_percentLabel->setPosition(xPer, yPer);
			//Utils::getplayLayerA()->m_percentLabel->setAnchorPoint({0.f, 0.5f});
		}
		else
		{
			//Utils::getplayLayerA()->m_percentLabel->setAnchorPoint({0.5f, 0.5f});
			//Utils::getplayLayerA()->m_percentLabel->setPosition((CCDirector::sharedDirector()->getWinSize().width / 2.f), ypos);
		}
	}

	cocos2d::CCSize WinSize() {
		return Utils::shareDirectorA()->getWinSize();
	}

	std::string decodeBase64Gzip(const std::string& input) {
		return ZipUtils::decompressString(input, false, 0);
	}

	bool isSpecificAspectRatio(float targetRatio) {
		auto director = cocos2d::CCDirector::sharedDirector();
		auto glview = director->getOpenGLView();
		auto size = glview->getFrameSize();
		float aspectRatio = size.width / size.height;
		return (aspectRatio - targetRatio) == 0;
	}

	bool isRoundAspectRatio(float targetRatio) {
		auto director = cocos2d::CCDirector::sharedDirector();
		auto glview = director->getOpenGLView();
		auto size = glview->getFrameSize();
		float aspectRatio = size.width / size.height;
		return std::abs(aspectRatio - targetRatio) < 0.01f;
	}

	std::string getFormattedCreatorName(const std::string& creatorName, GJLevelType levelType) {
		if (creatorName.empty()) {
			if (levelType == GJLevelType::Local)
				return "By RobTop";
			else
				return "By -";
		}
		return "By " + creatorName;
	}

	std::string buildStarRatingString(int rating, int requested, bool platformer) {
		auto starRating = rating;
		std::string starsOrMoons = " star";
		std::string requestedSuffix = " requested";
		if (platformer) { starsOrMoons = " moon"; }
		if (rating == 0) { starRating = requested; }
		else { requestedSuffix = ""; }
		if (starRating != 1) starsOrMoons = starsOrMoons + "s";
		return std::to_string(starRating) + starsOrMoons + requestedSuffix;
	}

	std::string getNameLevelType(GJLevelType type) {
		switch (type) {
		case GJLevelType::Editor:
			return "Editor Level";
		case GJLevelType::Local:
			return "Official Level";
		case GJLevelType::Saved:
			return "Online Level";
		default:
			return "NA Level";
		}
	}


	double getTotalSecondsPlayLayer() {
		if (auto pl = Utils::getplayLayerA()) {
			return std::floor(pl->m_time);
		}
		return 0.0;
	}

	int getTotalAttemptsPlayLayer() {
		if (auto pl = Utils::getplayLayerA()) {
			return pl->m_attempts;
		}
		return 0;
	}

	bool hasParentWithID(cocos2d::CCNode* node, const std::string& parentID) {
		cocos2d::CCNode* parentNode = node->getParent();
		while (parentNode) {
			if (parentNode->getID() == parentID) {
				return true;
			}
			parentNode = parentNode->getParent();
		}
		return false;
	}

	cocos2d::CCNode* getParentWithID(cocos2d::CCNode* node, const std::string& parentID) {
		cocos2d::CCNode* parentNode = node->getParent();
		while (parentNode) {
			if (parentNode->getID() == parentID) {
				return parentNode;
			}
			parentNode = parentNode->getParent();
		}
		return nullptr; // Si no se encuentra un nodo padre con el ID especificado
	}

}

#include <fmt/format.h>

std::string TimeUtils::timeToString(time_t input) {
	char dateString[255];
	strftime(dateString, sizeof(dateString), "%F %H:%M", localtime(&input));
	return dateString;
}

std::string TimeUtils::isoTimeToString(const std::string& input) {
	if (input.empty()) return "NA";
	int y, M, d;
	sscanf(input.c_str(), "%d-%d-%dT", &y, &M, &d);
	return fmt::format("{}-{:02}-{:02}", y, M, d);
}

std::string TimeUtils::workingTime(int value) {
	if (value <= 0) return "NA";

	int hours = value / 3600;
	int minutes = (value % 3600) / 60;
	int seconds = value % 60;

	std::ostringstream stream;
	if (hours > 0) stream << hours << "h ";
	if (minutes > 0) stream << minutes << "m ";
	stream << seconds << "s";

	return stream.str();
}

std::string TimeUtils::platformerTime(int value) {
	int milliseconds = value % 1000;
	int seconds = (value / 1000) % 60;
	int minutes = (value / 60000) % 60;
	int hours = (value / 3600000);

	if (hours > 0) return fmt::format("{}:{}:{}.{}", hours, minutes, seconds, milliseconds);
	if (minutes > 0) return fmt::format("{}:{}.{}", minutes, seconds, milliseconds);
	return fmt::format("{}.{}", seconds, milliseconds);
}

std::string TimeUtils::timestampToHumanReadable(time_t timestamp) {
	auto diff = difftime(time(nullptr), timestamp);

	int years = diff / 31536000;
	int months = diff / 2592000;
	int days = diff / 86400;
	int hours = diff / 3600;
	int minutes = diff / 60;

	if (years > 0) return fmt::format("{} year{}", years, years > 1 ? "s" : "");
	if (months > 0) return fmt::format("{} month{}", months, months > 1 ? "s" : "");
	if (days > 0) return fmt::format("{} day{}", days, days > 1 ? "s" : "");
	if (hours > 0) return fmt::format("{} hour{}", hours, hours > 1 ? "s" : "");
	if (minutes > 0) return fmt::format("{} minute{}", minutes, minutes > 1 ? "s" : "");
	return "Less than 1 minute";
}
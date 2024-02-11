#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include <time.h>
#include <string>

namespace Utils
{
	const char* getNameObj(cocos2d::CCNode* obj);
	bool returnZeroBool(int i);
	FMODAudioEngine* shareFMOD();
	GameManager* shareManager();

	PlayLayer* getplayLayerA();
	GLubyte convertOpacitySimplf(float opaTM);

	cocos2d::CCDirector* shareDirectorA();

	float getPercentagePerXpos();
	float getPercentageReversePerXpos();
	float getPercentageNow();
	float getPercentageReverseNow();
	float getPercentageNowFix();
	float getPercentageReverseNowFix();
	float getPercentagePerTime();

	void updatePercentageObjects();

	cocos2d::CCSize WinSize();

	template <class R, class T>
	R& from(T base, intptr_t offset) {
		return *reinterpret_cast<R*>(reinterpret_cast<uintptr_t>(base) + offset);
	}

	template<typename T>
	std::string vectorToString(const std::vector<T>& vec) {
		std::ostringstream oss;
		for (size_t i = 0; i < vec.size(); ++i) {
			if (i != 0) {
				oss << " ";
			}
			oss << vec[i];
		}
		return oss.str();
	}

	template<typename T>
	std::vector<T> stringToVector(const std::string& str) {
		std::vector<T> vec;
		std::istringstream iss(str);
		T val;
		while (iss >> val) {
			vec.push_back(val);
		}
		return vec;
	}
}


//thx cvolton :3
namespace TimeUtils {
	std::string timeToString(time_t input);
	std::string isoTimeToString(const std::string& input);
	std::string workingTime(int value);
	std::string platformerTime(int value);
	std::string timestampToHumanReadable(time_t timestamp);
}

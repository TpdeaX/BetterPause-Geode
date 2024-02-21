#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <time.h>
#include <string>

using namespace geode::prelude;

#ifdef GEODE_IS_WINDOWS
namespace matdash {
	struct Console {
		std::ofstream out, in;
		Console() {
			AllocConsole();
			out = decltype(out)("CONOUT$", std::ios::out);
			in = decltype(in)("CONIN$", std::ios::in);
			std::cout.rdbuf(out.rdbuf());
			std::cin.rdbuf(in.rdbuf());

			FILE* dummy;
			freopen_s(&dummy, "CONOUT$", "w", stdout);
		}
		~Console() {
			out.close();
			in.close();
		}
	};

	inline void create_console() {
		static Console console;
	}
}
#endif

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

	std::string decodeBase64Gzip(const std::string& input);
	bool isSpecificAspectRatio(float targetRatio);
	bool isRoundAspectRatio(float targetRatio);

	std::string getFormattedCreatorName(const std::string& creatorName, GJLevelType levelType);
	std::string getNameLevelType(GJLevelType type);
	double getTotalSecondsPlayLayer();
	int getTotalAttemptsPlayLayer();
}


//thx cvolton :3
namespace TimeUtils {
	std::string timeToString(time_t input);
	std::string isoTimeToString(const std::string& input);
	std::string workingTime(int value);
	std::string platformerTime(int value);
	std::string timestampToHumanReadable(time_t timestamp);
}

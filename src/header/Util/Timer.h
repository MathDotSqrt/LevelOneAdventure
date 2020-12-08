#pragma once

#include <string>
#include <chrono>
#include <unordered_map>

namespace LOA::Util {

	class Timer {
	public:
		Timer(std::string name);
		~Timer();
	private:
		std::string name;
		std::chrono::time_point<std::chrono::steady_clock> start;
	};

	class TimerManager {
	public:
		friend class Timer;
		static TimerManager& getInstance();

		void display();
		void clear();
	private:
		std::unordered_map<std::string, std::chrono::nanoseconds> durationMap;
		TimerManager();
	};
}
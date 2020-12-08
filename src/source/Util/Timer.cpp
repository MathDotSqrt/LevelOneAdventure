#include "Util/Timer.h"
#include <iostream>


using namespace LOA::Util;

static TimerManager* manager = nullptr;

TimerManager::TimerManager() {

}

TimerManager& TimerManager::getInstance() {
	if (manager == nullptr) {
		manager = new TimerManager();
	}

	return *manager;
}

void TimerManager::clear() {
	durationMap.clear();
}

void TimerManager::display() {
	using fMilli = std::chrono::duration<float, std::milli>;

	for (auto& [key, value] : durationMap) {
		std::cout << key << ": " << std::chrono::duration_cast<fMilli>(value).count() << "ms \n";
	}
}

Timer::Timer(std::string name) : name(name){
	start = std::chrono::steady_clock::now();
}

Timer::~Timer() {
	auto duration(std::chrono::steady_clock::now() - start);

	auto& manager = TimerManager::getInstance();
	
	if (manager.durationMap.find(name) != manager.durationMap.end()) {
		manager.durationMap[name] += duration;
	}
	else{
		manager.durationMap[name] = duration;
	}
}
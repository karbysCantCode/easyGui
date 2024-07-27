#pragma once
#include <chrono>
#include <thread>

/*
TRME_getTickInterval returns the total interval between ticks in chrono::milliseconds 

TRME_getTimePoint returns a timepoint of the time when it is called

TRME_getTimepointDifference returns the time between end and start time, or 0 if the result is negative

TRME_sleepUntilNextTick makes the thread it was called on sleep until tick interval - elapsed time (time between start time and time of call) passes
*/

std::chrono::milliseconds TRME_getTickInterval(int ticksPerSecond) {
	return std::chrono::milliseconds(1000 / ticksPerSecond);
}

std::chrono::time_point<std::chrono::high_resolution_clock> TRME_getTimePoint() {
	return std::chrono::high_resolution_clock::now();
}

std::chrono::duration<double, std::milli> TRME_getTimepointDifference(std::chrono::time_point<std::chrono::high_resolution_clock> startTime,
																std::chrono::time_point<std::chrono::high_resolution_clock> endTime) {
	std::chrono::duration<double, std::milli> product = endTime - startTime;
	if (product < std::chrono::milliseconds(0)) {
		return std::chrono::milliseconds(0);
	}
	return product;
}
//
void TRME_sleepUntilNextTick(std::chrono::time_point<std::chrono::high_resolution_clock> startTime , std::chrono::milliseconds tickInterval) {
	auto elapsed = TRME_getTimePoint() - startTime;
	auto timeToSleep = tickInterval - std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
	if (timeToSleep > std::chrono::milliseconds(0)) {
		std::this_thread::sleep_for(timeToSleep);
	}
}
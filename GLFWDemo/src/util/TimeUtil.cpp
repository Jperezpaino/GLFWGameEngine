#include "TimeUtil.h"

#include <thread>

std::chrono::steady_clock::time_point TimeUtil::timeStarted = std::chrono::steady_clock::now();

void TimeUtil::init() {
  timeStarted = std::chrono::steady_clock::now();
}

float TimeUtil::getTime() {
  const auto now = std::chrono::steady_clock::now();
  const auto elapsed = now - timeStarted;
  return std::chrono::duration<float>(elapsed).count();
}

float TimeUtil::getTimeMillis() {
  const auto now = std::chrono::steady_clock::now();
  const auto elapsed = now - timeStarted;
  return std::chrono::duration<float, std::milli>(elapsed).count();
}

long long TimeUtil::getTimeNanos() {
  const auto now = std::chrono::steady_clock::now();
  const auto elapsed = now - timeStarted;
  return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
}

void TimeUtil::sleep(
    float milliseconds) {
  std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(milliseconds));
}

float TimeUtil::deltaTime(
    float previousTime) {
  return TimeUtil::getTime() - previousTime;
}

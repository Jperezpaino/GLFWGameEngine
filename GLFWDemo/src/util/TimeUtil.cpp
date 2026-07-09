#include "TimeUtil.h"

std::chrono::steady_clock::time_point TimeUtil::m_timeStarted = std::chrono::steady_clock::now();

void TimeUtil::init() {
  m_timeStarted = std::chrono::steady_clock::now();
}

float TimeUtil::getTime() {
  const auto now = std::chrono::steady_clock::now();
  const auto elapsed = now - m_timeStarted;
  return std::chrono::duration<float>(elapsed).count();
}

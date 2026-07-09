#pragma once

#include <chrono>

class TimeUtil {

  public:

    static void init();
    static float getTime();

  private:

    static std::chrono::steady_clock::time_point m_timeStarted;

};

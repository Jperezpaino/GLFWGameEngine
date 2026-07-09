#pragma once

#include <chrono>

class TimeUtil {

  public:

    static void init();
    static float getTime();
    static float getTimeMillis();
    static long long getTimeNanos();
    static void sleep(float milliseconds);
    static float deltaTime(float previousTime);

  private:

    static std::chrono::steady_clock::time_point timeStarted;

};

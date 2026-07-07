#ifndef APPLICATION_H
#define APPLICATION_H

#include "Window.h"

class Application
{
public:
    Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    void run();

private:
    Window m_window;
};

#endif // APPLICATION_H

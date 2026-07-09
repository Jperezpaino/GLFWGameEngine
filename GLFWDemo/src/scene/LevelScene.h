#pragma once

#include "Scene.h"

class LevelScene
    : public Scene {

  public:

    LevelScene();
    ~LevelScene() override = default;

    void init() override;
    void update(float deltaTime) override;
    void render() override;

  private:

    bool m_changingScene = false;
    float m_timeToChangeScene = 1.0f;

};

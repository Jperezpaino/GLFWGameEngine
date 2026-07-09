#pragma once

#include "Scene.h"

class LevelScene
    : public Scene {

  public:

    LevelScene();
    ~LevelScene() override = default;

    void update(float deltaTime) override;

};

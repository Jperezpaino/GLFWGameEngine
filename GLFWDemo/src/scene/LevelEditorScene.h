#pragma once

#include "Scene.h"

class LevelEditorScene
    : public Scene {

  private:

    bool m_changingScene = false;
    float m_timeToChangeScene = 2.0f;

  public:

    LevelEditorScene();
    ~LevelEditorScene() override = default;

    void update(float deltaTime) override;

};

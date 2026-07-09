#pragma once

#include "Scene.h"

class LevelEditorScene
    : public Scene {

  public:

    LevelEditorScene();
    ~LevelEditorScene() override = default;

    void init() override;
    void update(float deltaTime) override;
    void render() override;

  private:

    bool m_changingScene = false;
    float m_timeToChangeScene = 1.0f;

};

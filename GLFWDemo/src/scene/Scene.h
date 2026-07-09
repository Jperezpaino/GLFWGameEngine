#pragma once

class Scene {

  public:

    Scene() = default;
    virtual ~Scene() = default;

    virtual void update(float deltaTime) = 0;

};

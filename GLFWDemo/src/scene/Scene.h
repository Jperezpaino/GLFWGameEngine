#pragma once

class Scene {

  public:

    Scene() = default;
    virtual ~Scene() = default;

    virtual void init() {}
    virtual void update(float deltaTime) = 0;
    virtual void render() {}

};

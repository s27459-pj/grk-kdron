#ifndef CUBE_H
#define CUBE_H

#include <GL/glew.h>

#include "indexmodel.h"
#include "modelprogram.h"
#include "movablemodel.h"

class Cube : public IndexModel, public MovableModel {
  public:
    Cube(float init_velocity = 15, float init_angle = 45);
    void Initialize();
    void Draw(const ModelProgram& program) const;
    void Update(float delta_t);
    void SpeedUp();
    void SlowDown();
    void ToggleAnimated();

  private:
    float angle_;
    float velocity_;
    bool animated_;
};

#endif // CUBE_H

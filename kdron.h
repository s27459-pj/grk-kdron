#ifndef KDRON_H
#define KDRON_H

#include <GL/glew.h>

#include "indexmodel.h"
#include "modelprogram.h"
#include "movablemodel.h"

class KDron : public IndexModel, public MovableModel {
  public:
    KDron(float init_velocity = 15, float init_angle = 45);
    void Initialize();
    void Draw(const ModelProgram& program) const;
    void Update(float delta_t);
    void RotateVertical(float amount);
    void RotateHorizontal(float amount);
    void Left() { RotateHorizontal(-2.0f); }
    void Right() { RotateHorizontal(2.0f); }
    void Up() { RotateVertical(-2.0f); }
    void Down() { RotateVertical(2.0f); }
    void SpeedUp();
    void SlowDown();
    void ToggleAnimated();

  private:
    void ApplyTransform();

    float angle_x_;
    float angle_y_;
    float velocity_;
    bool animated_;
};

#endif // KDRON_H

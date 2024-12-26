#include <GL/glew.h>

#include "kdron.h"
#include "vertices.h"

#define TRIANGLE_COUNT 17

KDron::KDron(float init_velocity, float init_angle) {
    velocity_ = init_velocity;
    angle_ = init_angle;
    animated_ = true;
}

void KDron::Move(float delta_t) {
    if (!animated_)
        return;
    angle_ += delta_t * velocity_;
    if (angle_ > 360)
        angle_ -= 360;
    if (angle_ < -360)
        angle_ += 360;
    model_matrix_.SetUnitMatrix();
    model_matrix_.RotateAboutX(angle_);
    model_matrix_.RotateAboutY(angle_);
}

void KDron::SpeedUp() { velocity_ *= 1.09544511501; }

void KDron::SlowDown() { velocity_ /= 1.09544511501; }

void KDron::ToggleAnimated() { animated_ = !animated_; }

void KDron::Initialize() {
    // clang-format off
    const ColorVertex kVertices[] = {
       { { -0.5f, -0.5f, -0.5f,  1.0f }, {1, 0, 0, 1} },
       { {  0.5f, -0.5f, -0.5f,  1.0f }, {1, 1, 0, 1} },
       { { -0.5f,  0.5f, -0.5f,  1.0f }, {0, 0, 1, 1} },
       { {  0.5f,  0.5f, -0.5f,  1.0f }, {0, 1, 1, 1} },
       { {  0.0f, -0.5f, -0.5f,  1.0f }, {1, 0, 1, 1} },
       { { -0.5f, -0.5f,  0.0f,  1.0f }, {0, 1, 0, 1} },
       { {  0.5f, -0.5f,  0.0f,  1.0f }, {1, 1, 1, 1} },
       { { -0.5f,  0.5f,  0.0f,  1.0f }, {0, 1, 1, 1} },
       { {  0.5f,  0.5f,  0.0f,  1.0f }, {1, 0, 1, 1} },
       { {  0.0f,  0.5f,  0.5f,  1.0f }, {1, 1, 1, 1} },
       { { -0.5f,  0.0f,  0.0f,  1.0f }, {1, 1, 0, 1} },
       { {  0.5f,  0.0f,  0.0f,  1.0f }, {0, 1, 0, 1} },
    };

    const Triangle kIndices[TRIANGLE_COUNT] = {
        {  2,   8,   7 },
        {  3,   8,   2 },
        {  3,   0,   1 },
        {  0,   3,   2 },
        { 11,  10,   9 },
        {  4,  11,  10 },
        {  1,   6,   4 },
        {  5,   4,  10 },
        {  7,   0,   5 },
        {  2,   7,   0 },
        {  9,   8,   7 },
        { 11,   4,   6 },
        {  1,   8,   6 },
        {  4,   5,   0 },
        {  3,   1,   8 },
        {  9,   7,  10 },
        { 11,   8,   9 },
    };
    // clang-format on

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kVertices), kVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(kVertices[0]),
                          (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(kVertices[0]),
                          (GLvoid*)sizeof(kVertices[0].position));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &index_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kIndices), kIndices,
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void KDron::Draw(const ModelProgram& program) const {

    glUseProgram(program);
    glBindVertexArray(vao_);

    program.SetModelMatrix(model_matrix_);

    glDrawElements(GL_TRIANGLES, TRIANGLE_COUNT * sizeof(Triangle),
                   GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);
}

#include "window.h"

#include <cstdlib>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glerror.h"
#include "kdron.h"

const char* kVertexShader = "SimpleShader.vertex.glsl";
const char* kFragmentShader = "SimpleShader.fragment.glsl";

Window::Window(const char* title, int width, int height) {
    title_ = title;
    width_ = width;
    height_ = height;
    projection_ = Perspective;
    active_model_ = 1; // Start on k-dron
    last_time_ = 0;
}

void Window::Initialize(int major_gl_version, int minor_gl_version) {

    InitGlfwOrDie(major_gl_version, minor_gl_version);
    InitGlewOrDie();

    std::cout << "OpenGL initialized: OpenGL version: "
              << glGetString(GL_VERSION)
              << " GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
              << std::endl;

    InitModels();
    InitPrograms();

    view_matrix_.Translate(0, 0, -2);
    SetViewMatrix();

    SetProjectionMatrix();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Window::InitGlfwOrDie(int major_gl_version, int minor_gl_version) {
    if (!glfwInit()) {
        std::cerr << "ERROR: Could not initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major_gl_version);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor_gl_version);
#ifdef DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
    window_ = glfwCreateWindow(width_, height_, title_, nullptr, nullptr);
    if (!window_) {
        std::cerr << "ERROR: Could not create a new rendering window"
                  << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window_);
}

void Window::InitGlewOrDie() {
    GLenum glew_init_result;
    glewExperimental = GL_TRUE;
    glew_init_result = glewInit();

    if (GLEW_OK != glew_init_result) {
        std::cerr << "Glew ERROR: " << glewGetErrorString(glew_init_result)
                  << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
#ifdef DEBUG
    if (glDebugMessageCallback) {
        std::cout << "Register OpenGL debug callback " << std::endl;
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback((GLDEBUGPROC)OpenglCallbackFunction, NULL);
        GLuint unused_ids = 0;
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                              &unused_ids, GL_FALSE);
    } else
        std::cout << "glDebugMessageCallback not available" << std::endl;
#endif
}

void Window::InitModels() {
    cube_.Initialize();
    kdron_.Initialize();
}

void Window::InitPrograms() {
    program_.Initialize(kVertexShader, kFragmentShader);
}

void Window::SetViewMatrix() const {
    glUseProgram(program_);
    program_.SetViewMatrix(view_matrix_);
}

void Window::SetProjectionMatrix() {
    glUseProgram(program_);
    if (projection_ == Orthographic) {
        projection_matrix_ = Mat4::CreateOrthoProjectionMatrix(
            -1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    } else {
        projection_matrix_ = Mat4::CreatePerspectiveProjectionMatrix(
            60, (float)width_ / (float)height_, 0.1f, 100.0f);
    }
    program_.SetProjectionMatrix(projection_matrix_);
}

void Window::SetProjection(Projection projection) {
    projection_ = projection;
    SetProjectionMatrix();
}

void Window::Zoom(float amount) {
    view_matrix_.Translate(0, 0, amount);
    SetViewMatrix();
}

void Window::Resize(int new_width, int new_height) {
    width_ = new_width;
    height_ = new_height;
    SetProjectionMatrix();
    glViewport(0, 0, width_, height_);
}

void Window::KeyEvent(int key, int /*scancode*/, int action, int /*mods*/) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window_, GLFW_TRUE);
            break;
        // Speed up/slow down animation
        case GLFW_KEY_LEFT_BRACKET:
            cube_.SlowDown();
            kdron_.SlowDown();
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            cube_.SpeedUp();
            kdron_.SpeedUp();
            break;
        // Play/pause animation
        case GLFW_KEY_SPACE:
            cube_.ToggleAnimated();
            kdron_.ToggleAnimated();
            break;
        // Rotation
        case GLFW_KEY_LEFT:
            kdron_.Left();
            break;
        case GLFW_KEY_RIGHT:
            kdron_.Right();
            break;
        case GLFW_KEY_UP:
            kdron_.Up();
            break;
        case GLFW_KEY_DOWN:
            kdron_.Down();
            break;
        // Zoom
        case GLFW_KEY_PAGE_UP:
            Zoom(0.1f);
            break;
        case GLFW_KEY_PAGE_DOWN:
            Zoom(-0.1f);
            break;
        // Zoom for keyboards without pgup/pgdown
        case GLFW_KEY_EQUAL:
            Zoom(0.1f);
            break;
        case GLFW_KEY_MINUS:
            Zoom(-0.1f);
            break;
        // Change model
        case GLFW_KEY_TAB:
            active_model_ = (active_model_ + 1) % 2;
            std::cout << "Changed model to " << active_model_ << std::endl;
            break;
        // Change projection
        case GLFW_KEY_HOME:
            SetProjection(Perspective);
            break;
        case GLFW_KEY_END:
            SetProjection(Orthographic);
            break;
        // Change projection for keyboards without home/end
        case GLFW_KEY_P:
            SetProjection(Perspective);
            break;
        case GLFW_KEY_O:
            SetProjection(Orthographic);
            break;
        default:
            break;
        }
    } else if (action == GLFW_REPEAT) {
        switch (key) {
        // Speed up/slow down animation
        case GLFW_KEY_LEFT_BRACKET:
            cube_.SlowDown();
            kdron_.SlowDown();
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            cube_.SpeedUp();
            kdron_.SpeedUp();
            break;
        // Rotation
        case GLFW_KEY_LEFT:
            kdron_.Left();
            break;
        case GLFW_KEY_RIGHT:
            kdron_.Right();
            break;
        case GLFW_KEY_UP:
            kdron_.Up();
            break;
        case GLFW_KEY_DOWN:
            kdron_.Down();
            break;
        // Zoom
        case GLFW_KEY_PAGE_UP:
            Zoom(0.1f);
            break;
        case GLFW_KEY_PAGE_DOWN:
            Zoom(-0.1f);
            break;
        // Zoom for keyboards without pgup/pgdown
        case GLFW_KEY_EQUAL:
            Zoom(0.1f);
            break;
        case GLFW_KEY_MINUS:
            Zoom(-0.1f);
            break;
        default:
            break;
        }
    }
}

void Window::Run(void) {
    while (!glfwWindowShouldClose(window_)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        clock_t now = clock();
        if (last_time_ == 0)
            last_time_ = now;
        float delta_time = (float)(now - last_time_) / CLOCKS_PER_SEC;
        cube_.Update(delta_time);
        kdron_.Update(delta_time);
        last_time_ = now;

        if (active_model_ == 0)
            cube_.Draw(program_);
        else if (active_model_ == 1)
            kdron_.Draw(program_);
        else
            std::cerr << "ERROR: Unknown model index: " << active_model_
                      << std::endl;

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}

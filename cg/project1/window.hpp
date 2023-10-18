#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <random>

#include "abcgOpenGL.hpp"

#include "bubbles.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  /* void onEvent(SDL_Event const &event) override; */
  void onCreate() override;
  void onUpdate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  glm::ivec2 m_viewportSize{};
  GLuint m_objectsProgram{};
  GLuint m_bubblesProgram{};


  Bubbles m_bubbles;

  abcg::Timer m_restartWaitTimer;

  int m_numOfBubbles{5};
  int m_avgBubbleSpeed{20};

  std::default_random_engine m_randomEngine;

  void restart();
};

#endif

#include "window.hpp"

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  // Create program to render the bubbles
  m_bubblesProgram = 
      abcg::createOpenGLProgram({{.source = assetsPath + "bubbles.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "bubbles.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  abcg::glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  restart();
}

void Window::restart() {
  onDestroy();
  m_bubbles.create(m_bubblesProgram, m_numOfBubbles, m_avgBubbleSpeed);
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  m_bubbles.update(deltaTime);
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);
  m_bubbles.paint();
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  {
    auto const size{ImVec2(400, 100)};
    auto const position{ImVec2((m_viewportSize.x - size.x) / 2.0f,
                               (m_viewportSize.y - size.y) / 2.0f)};
    ImGui::SetNextWindowSize(size);
    ImGui::Begin(" ", nullptr, ImGuiWindowFlags_NoDecoration);

    if(ImGui::Button("Restart", ImVec2(150,30))) {
      Window::restart();
    }

    ImGui::SliderInt("Number of Bubbles", &m_numOfBubbles, 5, 20);
    ImGui::SliderInt("Avarage Bubble Speed", &m_avgBubbleSpeed,- 1000, 1000);

    ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onDestroy() {
  abcg::glDeleteProgram(m_objectsProgram);
  m_bubbles.destroy();
}

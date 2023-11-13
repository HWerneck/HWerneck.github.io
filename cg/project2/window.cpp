#include "window.hpp"

#include <glm/gtc/random.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Window::onEvent(SDL_Event const &event)
{
  if (event.type == SDL_KEYDOWN)
  {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      m_speed = 1.0f;
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
      m_speed= -1.0f;
  }
  if (event.type == SDL_KEYUP)
  {
    if ((event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) &&
        m_speed> 0)
      m_speed = 0.0f;
    if ((event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) &&
        m_speed < 0)
      m_speed = 0.0f;
  }
}

void Window::onCreate()
{
  auto const assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "depth.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "depth.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_ground.create(m_program);

  m_model.loadObj(assetsPath + "box.obj");
  m_model.setupVAO(m_program);

  // Camera at (0,0,0) and looking towards the negative z
  glm::vec3 const eye{10.0f, 10.0f, 10.0f};
  glm::vec3 const at{0.0f, 0.0f, 0.0f};
  glm::vec3 const up{0.0f, 1.0f, 0.0f};
  m_viewMatrix = glm::lookAt(eye, at, up);

  // Setup car
  setupCar(m_car)
}

void Window::setupCar(Car &m_car)
{
  glm::vec3 const initPos{0.0f, 1.0f, 0.0f};
  m_car.m_position = initPos;
}

void Window::onPaint()
{
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  // Get location of uniform variables
  auto const viewMatrixLoc{abcg::glGetUniformLocation(m_program, "viewMatrix")};
  auto const projMatrixLoc{abcg::glGetUniformLocation(m_program, "projMatrix")};
  auto const modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  auto const colorLoc{abcg::glGetUniformLocation(m_program, "color")};

  // Set uniform variables that have the same value for every model
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform4f(colorLoc, 0.3f, 0.3f, 0.3f, 1.0f); //Grey

  // Renders the car
  // Compute model matrix of the current m_car
  glm::mat4 modelMatrix{1.0f};
  modelMatrix = glm::translate(modelMatrix, m_car.m_position);
  modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
  /* modelMatrix = glm::rotate(modelMatrix, m_angle, m_car.m_rotationAxis); */

  // Set uniform variable
  abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

  m_model.render();

  m_ground.paint();

  abcg::glUseProgram(0);
}

void Window::onPaintUI()
{
  abcg::OpenGLWindow::onPaintUI();

  auto const widgetSize{ImVec2(218, 62)};
  ImGui::SetNextWindowPos(ImVec2(m_viewportSize.x - widgetSize.x - 5, 5));
  ImGui::SetNextWindowSize(widgetSize);
  ImGui::Begin("Widget window", nullptr, ImGuiWindowFlags_NoDecoration);

  ImGui::PushItemWidth(120);
  static std::size_t currentIndex{};
  std::vector<std::string> const comboItems{"Perspective", "Orthographic"};

  if (ImGui::BeginCombo("Projection",
                        comboItems.at(currentIndex).c_str()))
                        {
    for (auto const index : iter::range(comboItems.size()))
    {
      auto const isSelected{currentIndex == index};
      if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
        currentIndex = index;
      if (isSelected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  ImGui::PopItemWidth();

  ImGui::PushItemWidth(170);
  auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
                    gsl::narrow<float>(m_viewportSize.y)};
  if (currentIndex == 0)
  {
    m_projMatrix =
        glm::perspective(glm::radians(m_FOV), aspect, 0.01f, 100.0f);

    ImGui::SliderFloat("FOV", &m_FOV, 5.0f, 179.0f, "%.0f degrees");
  } else
  {
    m_projMatrix = glm::ortho(-20.0f * aspect, 20.0f * aspect, -20.0f,
                              20.0f, 0.01f, 100.0f);
  }
  ImGui::PopItemWidth();

  ImGui::End();
}

void Window::onResize(glm::ivec2 const &size) { m_viewportSize = size; }

void Window::onUpdate()
{
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  /*m_angle = glm::wrapAngle(m_angle + glm::radians(90.0f) * deltaTime);*/

  //Update car position
  m_car.m_position.z += deltaTime * m_speed;

  if (m_car.m_position.z > 10.0f)
  {
    setupCar(car);
    m_car.m_position.z = -10.0f;
  }
}

void Window::onDestroy()
{
  m_ground.destroy();
  m_model.destroy();
  abcg::glDeleteProgram(m_program);
}

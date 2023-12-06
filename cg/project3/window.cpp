#include "window.hpp"

#include <glm/gtc/random.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Window::onEvent(SDL_Event const &event)
{
  if (event.type == SDL_KEYDOWN)
  {
    if (event.key.keysym.sym == SDLK_UP)
    {
      m_speed += -0.5f;
      if (m_speed < -20.0f) m_speed = -20.0f;
    }
    if (event.key.keysym.sym == SDLK_DOWN)
    {
      m_speed += 0.5f;
      if (m_speed > 10.0f) m_speed = 10.0f;
    }
    if (event.key.keysym.sym == SDLK_LEFT && m_speed !=0) 
      m_angleRot = 0.07f;
    if (event.key.keysym.sym == SDLK_RIGHT && m_speed !=0)
      m_angleRot = -0.07f;
  }
  if (event.type == SDL_KEYUP)
  {
    if ((event.key.keysym.sym == SDLK_UP) &&
        m_speed < 0)
    {
      m_speed += 0.1f;
      if (m_speed > 0.0f) m_speed = 0.0f;
    }
    if ((event.key.keysym.sym == SDLK_DOWN) &&
        m_speed > 0)
    {
      m_speed += -0.1f;
      if (m_speed < 0.0f) m_speed = 0.0f;
    }
    if ((event.key.keysym.sym == SDLK_LEFT) &&
        m_angleRot > 0)
      m_angleRot = 0.0f;
    if ((event.key.keysym.sym == SDLK_RIGHT) &&
        m_angleRot < 0)
      m_angleRot = 0.0f;
  }
}

void Window::onCreate()
{
  auto const assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  m_randomEngine.seed(
    std::chrono::steady_clock::now().time_since_epoch().count());

  m_carProgram =
      abcg::createOpenGLProgram({{.source = assetsPath + "depth.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "depth.frag",
                                  .stage = abcg::ShaderStage::Fragment}});


  m_blockProgram =
      abcg::createOpenGLProgram({{.source = assetsPath + "normal.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "normal.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_ground.create(m_blockProgram);

  m_model_car.loadObj(assetsPath + "box.obj");
  m_model_car.setupVAO(m_carProgram);

  m_model_block.loadObj(assetsPath + "block.obj");
  m_model_block.setupVAO(m_blockProgram);

  glm::vec3 const eye{8.0f, 8.0f, 8.0f};
  glm::vec3 const at{0.0f, 0.0f, 0.0f};
  glm::vec3 const up{0.0f, 1.0f, 0.0f};
  m_viewMatrix = glm::lookAt(eye, at, up);

  setupCar(m_car);
  setupBlock(m_block);
}

void Window::setupCar(Car &m_car)
{
  glm::vec3 const initPos{0.0f, 0.3f, 3.5f};
  m_car.m_position = initPos;
  m_car.m_angle = 0.0f;
  m_car.m_rotationAxis = {0.0f, 1.0f, 0.0f};
}

void Window::setupBlock(Block &m_block)
{
  std::uniform_real_distribution<float> posXZ(-m_ground.m_maxLimit * 0.9f, +m_ground.m_maxLimit * 0.9f);
  glm::vec3 const initPos{posXZ(m_randomEngine), 0.3f, posXZ(m_randomEngine)};
  m_block.m_position = initPos;
  m_block.m_angle = 0.0f;
  m_block.m_rotationAxis = {0.0f , 1.0f, 0.0f};
}

void Window::randomizeBlock(Block &m_block){
  std::uniform_real_distribution<float> posXZ(-m_ground.m_maxLimit * 0.9f, +m_ground.m_maxLimit * 0.9f);
  glm::vec3 const initPos{posXZ(m_randomEngine), 0.3f, posXZ(m_randomEngine)};
  m_block.m_position = initPos;
}

void Window::onPaint()
{
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  /*==========//==========/ Renderiza carro com m_carProgram /==========//==========*/

  abcg::glUseProgram(m_carProgram);

  // Get location of uniform variables
  auto const car_viewMatrixLoc{abcg::glGetUniformLocation(m_carProgram, "viewMatrix")};
  auto const car_projMatrixLoc{abcg::glGetUniformLocation(m_carProgram, "projMatrix")};
  auto const car_modelMatrixLoc{abcg::glGetUniformLocation(m_carProgram, "modelMatrix")};
  //auto const normalMatrixLoc{abcg::glGetUniformLocation(m_carProgram, "normalMatrix")};
  auto const car_colorLoc{abcg::glGetUniformLocation(m_carProgram, "color")};

  // Set uniform variables that have the same value for every model
  abcg::glUniformMatrix4fv(car_viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(car_projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform4f(car_colorLoc, 0.3f, 0.3f, 0.3f, 1.0f); //Grey

  // Renders the car
  // Compute model matrix of the current m_car
  glm::mat4 car_modelMatrix{1.0f};
  car_modelMatrix = glm::translate(car_modelMatrix, m_car.m_position);
  car_modelMatrix = glm::scale(car_modelMatrix, glm::vec3(0.2f));
  car_modelMatrix = glm::rotate(car_modelMatrix, m_car.m_angle, m_car.m_rotationAxis);

  abcg::glUniformMatrix4fv(car_modelMatrixLoc, 1, GL_FALSE, &car_modelMatrix[0][0]);

  m_model_car.render();

  /*==========//==========/ Renderiza bloco com m_blockProgram /==========//==========*/

  abcg::glUseProgram(m_blockProgram);

  // Get location of uniform variables
  auto const block_viewMatrixLoc{abcg::glGetUniformLocation(m_blockProgram, "viewMatrix")};
  auto const block_projMatrixLoc{abcg::glGetUniformLocation(m_blockProgram, "projMatrix")};
  auto const block_modelMatrixLoc{abcg::glGetUniformLocation(m_blockProgram, "modelMatrix")};
  auto const block_normalMatrixLoc{abcg::glGetUniformLocation(m_blockProgram, "normalMatrix")};
  //auto const block_colorLoc{abcg::glGetUniformLocation(m_carProgram, "color")};

  // Set uniform variables that have the same value for every model
  abcg::glUniformMatrix4fv(block_viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(block_projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  //abcg::glUniform4f(car_colorLoc, 0.3f, 0.3f, 0.3f, 1.0f); //Grey

  // Renders the block
  // Compute model matrix of the current m_block
  glm::mat4 block_modelMatrix{1.0f};
  block_modelMatrix = glm::translate(block_modelMatrix, m_block.m_position);
  block_modelMatrix = glm::scale(block_modelMatrix, glm::vec3(0.2f));
  block_modelMatrix = glm::rotate(block_modelMatrix, m_block.m_angle, m_block.m_rotationAxis);

  auto const block_modelViewMatrix{glm::mat3(m_viewMatrix * block_modelMatrix)};
  auto const block_normalMatrix{glm::inverseTranspose(block_modelViewMatrix)};
  abcg::glUniformMatrix3fv(block_normalMatrixLoc, 1, GL_FALSE, &block_normalMatrix[0][0]);

  // Set uniform variable
  abcg::glUniformMatrix4fv(block_modelMatrixLoc, 1, GL_FALSE, &block_modelMatrix[0][0]);

  m_model_block.render();

  /*==========//==========/ Renderiza m_ground (ainda com o m_blockProgram) /==========//==========*/

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
  std::vector<std::string> const comboItems{"Perspective", "Orthographic", "Top View"};

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
    glm::vec3 const eye{10.0f, 10.0f, 10.0f};
    glm::vec3 const at{0.0f, 0.0f, 0.0f};
    glm::vec3 const up{0.0f, 1.0f, 0.0f};
    m_viewMatrix = glm::lookAt(eye, at, up);

    m_projMatrix =
        glm::perspective(glm::radians(m_FOV), aspect, 0.01f, 100.0f);

    ImGui::SliderFloat("FOV", &m_FOV, 1.0f, 179.0f, "%.0f degrees");
  } else if (currentIndex == 1)
  {
    glm::vec3 const eye{10.0f, 10.0f, 10.0f};
    glm::vec3 const at{0.0f, 0.0f, 0.0f};
    glm::vec3 const up{0.0f, 1.0f, 0.0f};
    m_viewMatrix = glm::lookAt(eye, at, up);
    m_projMatrix = glm::ortho(-5.0f * aspect, 5.0f * aspect, -8.0f,
                              8.0f, 1.0f, 95.0f);
  } else if (currentIndex == 2)
  {
    glm::vec3 const eye{0.0f, 18.0f, 0.0f};
    glm::vec3 const at{0.0f, 0.0f, 0.0f};
    glm::vec3 const up{0.0f, 0.0f, -1.0f};
    m_viewMatrix = glm::lookAt(eye, at, up);
    m_projMatrix = glm::perspective(glm::radians(30.0f), aspect, 0.01f, 100.0f);
  }
  ImGui::PopItemWidth();

  ImGui::End();
}

void Window::onResize(glm::ivec2 const &size) { m_viewportSize = size; }

void Window::onUpdate(){
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};
  if (m_speed < 0.0f)
  {
    m_speed += 0.01f;
    if (m_speed > 0.0f) m_speed = 0.0f;
  } else if (m_speed > 0.0f)
  {
    m_speed += -0.01f;
    if (m_speed < 0.0f) m_speed = 0.0f;
  }

  //Update m_car position
  m_car.m_angle += m_angleRot;
  m_car.m_position.z += cos(m_car.m_angle) * deltaTime * m_speed;
  m_car.m_position.x += sin(m_car.m_angle) * deltaTime * m_speed;


  if((abs(m_car.m_position.x - m_block.m_position.x) < 0.2f) && (abs(m_car.m_position.z - m_block.m_position.z) < 0.2f)){
    randomizeBlock(m_block);
  }

  if (m_car.m_position.z > m_ground.m_maxLimit)
  {
    m_car.m_position.z = m_ground.m_maxLimit;
    m_speed = m_speed * 0.2;
  } else if (m_car.m_position.z < -m_ground.m_maxLimit)
  {
    m_car.m_position.z = -m_ground.m_maxLimit;
    m_speed = m_speed * 0.2;
  }
  if (m_car.m_position.x > m_ground.m_maxLimit)
  {
    m_car.m_position.x = m_ground.m_maxLimit;
    m_speed = m_speed * 0.2;
  } else if (m_car.m_position.x < -m_ground.m_maxLimit)
  {
    m_car.m_position.x = -m_ground.m_maxLimit;
    m_speed = m_speed * 0.2;
  }
}

void Window::onDestroy()
{
  m_ground.destroy();
  m_model_car.destroy();
  m_model_block.destroy();
  abcg::glDeleteProgram(m_carProgram);
  abcg::glDeleteProgram(m_blockProgram);
}

#include "bubbles.hpp"

#include <glm/gtx/fast_trigonometry.hpp>

void Bubbles::create(GLuint program, int quantity){
  destroy();

  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  m_program = program;

  // Get location of uniforms in the program
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");


  // Create asteroids
  m_bubbles.clear();
  m_bubbles.resize(quantity);
    
  for (auto &bubble : m_bubbles) {
    bubble = makeBubbles();

    // Make sure the asteroid won't collide with the ship
    do {
      bubble.m_translation = {m_randomDist(m_randomEngine),
                                m_randomDist(m_randomEngine)};
    } while (glm::length(bubble.m_translation) < 0.5f);
  }

}

void Bubbles::paint() {
  abcg::glUseProgram(m_program);

  for (auto const &bubble : m_bubbles) {
    abcg::glBindVertexArray(bubble.m_VAO);

    abcg::glUniform4fv(m_colorLoc, 1, &bubble.m_color.r);
    abcg::glUniform1f(m_scaleLoc, bubble.m_scale);
    abcg::glUniform1f(m_rotationLoc, bubble.m_rotation);

    for (auto i : {-2, 0, 2}) {
      for (auto j : {-2, 0, 2}) {
        abcg::glUniform2f(m_translationLoc, bubble.m_translation.x + j,
                          bubble.m_translation.y + i);

        abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, bubble.m_polygonSides + 2);
      }
    }

    abcg::glBindVertexArray(0);
  }

  abcg::glUseProgram(0);
}

void Bubbles::destroy() {
  // Release previous resources, if any
  for (auto &bubble : m_bubbles){
    abcg::glDeleteBuffers(1, &bubble.m_VBOPositions);
    abcg::glDeleteBuffers(1, &bubble.m_VBOColors);
    abcg::glDeleteBuffers(1, &bubble.m_VBO);
    abcg::glDeleteVertexArrays(1, &bubble.m_VAO);
  }
}

void Bubbles::update(float deltaTime) {
  glm::vec2 const translation{0.05f,0.05f};
  for (auto &bubble : m_bubbles) {
    bubble.m_translation -= translation * deltaTime;
    bubble.m_rotation = 0; 

        // Wrap-around
    if (bubble.m_translation.x < -1.0f)
      bubble.m_translation.x += 2.0f;
    if (bubble.m_translation.x > +1.0f)
      bubble.m_translation.x -= 2.0f;
    if (bubble.m_translation.y < -1.0f)
      bubble.m_translation.y += 2.0f;
    if (bubble.m_translation.y > +1.0f)
      bubble.m_translation.y -= 2.0f;

  }
}


Bubbles::Bubble Bubbles::makeBubbles(glm::vec2 translation, float scale) {
  Bubble bubble;

  auto &re{m_randomEngine}; // Shortcut

  // Randomly pick the number of sides

  // Get a random color (actually, a grayscale)
  std::uniform_real_distribution randomIntensity(0.5f, 1.0f);
  bubble.m_color = glm::vec4(randomIntensity(re));

  bubble.m_color.a = 1.0f;
  bubble.m_rotation = 0.0f;
  bubble.m_scale = scale;
  bubble.m_translation = translation;

  // Get a random angular velocity
  bubble.m_angularVelocity = m_randomDist(re);

  // Get a random direction
  glm::vec2 const direction{m_randomDist(re), m_randomDist(re)};
  bubble.m_velocity = glm::normalize(direction) / 7.0f;

  // Create geometry data
  std::vector<glm::vec2> positions{{0, 0}};
  auto const step{M_PI * 2 / bubble.m_polygonSides};
  std::uniform_real_distribution randomRadius(0.8f, 1.0f);
  for (auto const angle : iter::range(0.0, M_PI * 2, step)) {
    auto const radius{randomRadius(re)};
    positions.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO
  abcg::glGenBuffers(1, &bubble.m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, bubble.m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &bubble.m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(bubble.m_VAO);

  abcg::glBindBuffer(GL_ARRAY_BUFFER, bubble.m_VBO);
  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);

  return bubble;
}

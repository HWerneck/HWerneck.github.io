### Participantes
Maurício Romano RA: 11013212  
Henrique Ramos Rissardo Werneck RA: 11007812

## Projeto 3 de Computação Gráfica

Neste projeto damos sequência ao projeto anterior, onde implementamos um modelo de carro controlado pelo usuário.
Como alterações adicionais, executamos:
- aplicação de iluminação;
- formas diferentes de coloração de pixel de forma simultânea;
- geração de objetos adicionais em posições aleatórias;
- coleta destes objetos através de um sistema de colisão;
- fechar mapa para movimentação apenas dentro dos seus limites;
- melhoria de contrabilidade do carro;

###  Análise do código

### main.cpp

Responsável pela execução do aplicativo.  
Nenhuma alteração significativa.

### model.hpp

Modificado para incluir os vetores normais no vértices.  
A estrutura dos vértices (struct Vertex) inclui um novo campo normal de tipo glm::vec3, como mostrado abaixo.  

``` 
struct Vertex
{
  glm::vec3 position{};
  glm::vec3 normal{};

  friend bool operator==(Vertex const &, Vertex const &) = default;
};
```

Também foi adicionada uma declaração para computar os vetores normais, caso o modelo não disponha dos vetores já calculados.

```
bool m_hasNormals{false};

void computeNormals();
```

### model.cpp

Modificado para refletir a inclusão dos vetores normais na estrutura dos vértices e operações com essa nova funcionalidade. Código hash adaptado:

```
template <> struct std::hash<Vertex>
{
  size_t operator()(Vertex const &vertex) const noexcept
  {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    auto const h2{std::hash<glm::vec3>()(vertex.normal)};
    return abcg::hashCombine(h1, h2);
  }
};
```

A função Model::loadObj() seta o booleano m_hasNormals como true caso não haja necessidade de calcular os vetores normais dos vértices do modelo:

```
glm::vec3 normal{};
if (index.normal_index >= 0) {
  m_hasNormals = true;
  auto const normalStartIndex{3 * index.normal_index};
  normal = {attrib.normals.at(normalStartIndex + 0),
            attrib.normals.at(normalStartIndex + 1),
            attrib.normals.at(normalStartIndex + 2)};
}
```  

Caso seja necessário, a função Model::computeNormals() é chamada mais adiante na função:

```
if (!m_hasNormals)
{
  computeNormals();
}
```  

A função Model::computeNormals() calcula os vértices da mesma forma que aplicado em atividade em sala.

### window.hpp

O window.hpp teve algumas alterações. Em primeiro lugar, foi adicionada uma estrutura para os blocos que são coletados no mapa, com modelos separados para o carro e os blocos:

```
Model m_model_car;

Model m_model_block;
```

e

```
struct Block
{
  glm::vec3 m_position{};
  float m_angle;
  glm::vec3 m_rotationAxis{};
};

Block m_block;
```

Além dessas, também foi adicionada duas declarações de funções relacionadas aos blocos, além do motor aleatório para posicionamento dos blocos:

```
std::default_random_engine m_randomEngine;
```

e

```
void setupBlock(Block &m_block);
void randomizeBlock(Block &m_block);
```

### window.cpp

O window.cpp teve mais alterações do que apenas as antecipadas pelo window.hpp. Logo no início da função foi implementadas novas versões das funções Window::onEvent() e Window::onUpdate(). A controlabilidade do carro foi melhorada, além de alterações no comportamento do carro em tela. Foi bloqueado ao carro atravessar o mapa e aparecer do outro lado; ele para na borda do mapa. Além disso, foi bloqueado movimento lateral quando não há velocidade para frente ou para trás:

```
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
```

e

```
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
```

Com relação aos shaders, foram utilizados shaders diferentes para o carro e o bloco, criados simultaneamente com programas diferentes do OpenGL na função Window::onCreate(). O chão fez uso do programa com iluminação utilizado no bloco:

```
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
```

Os modelos do carro e do bloco são configurados no mapa em seguida através da função Window::setupBlock(), analogamente à função Window::setupCar():

```
setupBlock(m_block);
```

As funções Window::setupCar, Window::setupBlock e Window::randomizeBlock:

```
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
```

Diferente do proposto no exemplo de sala, optamos por não variar o shader do modelo a partir de um comando na interface gráfica. Invés disso, os modelos fazem uso de shaders diferentes de forma simultânea, sem utilizar também a estrutura de vetor (vector) para armazená-los.

A função Window::onPaint() contém alterações referentes à renderização dos modelos utilizados. Cada modelo segue um padrão diferente, com o modelo do carro utilizando o modelo baseado em profundidade:

```
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
```

O bloco, por sua vez utiliza o modelo de coloração de acordo com a iluminação:

```
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
// Compute model matri x of the current m_block
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
```

### Referências

Sites e comandos úteis:
- espaço duplo (' &nbsp;'): cria uma nova linha
- stackedit (https://stackedit.io/app#): site pode ser usado para pré-visualizar readme.md antes de comitá-lo.

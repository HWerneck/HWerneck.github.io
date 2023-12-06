### Participantes
Maurício Romano RA: 11013212  
Henrique Ramos Rissardo Werneck RA: 11007812

## Projeto 3 de Computação Gráfica

Neste projeto damos sequência ao projeto anterior, onde implementamos um modelo de carro controlado pelo usuário.
Alterações referentes à parte final da disciplina incluem:  
- iluminação  
- textura  

Como alterações adicionais, executamos:
- geração de objetos em posição aleatória;
- coleta destes objetos através de um sistema de colisão;
- fechar mapa para movimentação apenas dentro dos seus limites;
- melhoria da contrabilidade do carro;

A partir deste ponto o readme é um exemplo de como podemos escrevê-lo.

###  Análise do código

### main.cpp

Responsável pela execução do aplicativo.  
Nenhuma alteração significativa.

### model.hpp

Modificado para incluir os vetores normais no vértices.  
A estrutura dos vértices (struct Vertex) inclui um novo campo glm::vec3, como mostrado abaixo.  

> struct Vertex
> {
>   glm::vec3 position{};
>   glm::vec3 normal{};
> 
>   friend bool operator==(Vertex const &, Vertex const &) = default;
> };  

Também foi adicionada uma declaração para computar as normais, caso o modelo não disponha dos elementos já calculados.

> bool m_hasNormals{false};
> 
> void computeNormals();

### model.cpp

Modificado para refletir a inclusão dos vetores normais na estrutura dos vértices.  
Destaque para inclusão da função Model::computeNormals(), utilizada para o caso de o modelo utilizado não contar com os vetores normais já calculados.    
Não está significativamente diferente com relação ao projeto visto em sala.

### window.hpp

O window.hpp teve algumas alterações. Em primeiro lugar, foi adicionada uma estrutura para os blocos que são coletados no mapa:

> Model m_model_block;

e

> struct Block
> {
>   glm::vec3 m_position{};
>   float m_angle;
>   glm::vec3 m_rotationAxis{};
> };
>
> Block m_block;

Além dessas, também foi adicionada duas declarações de funções relacionadas aos blocos:

> void setupBlock(Block &m_block);
> void randomizeBlock(Block &m_block);

### window.cpp

O window.cpp teve mais alterações do que as antecipadas pelo window.hpp. Com relação à adaptação do shader, somente foi alterado o nome do arquivo que é buscado dentro da função Window::onCreate(). O modelo da estrutura Block é carregado (com o mesmo shader utilizado para o modelo do carro):

> m_model_block.loadObj(assetsPath + "block.obj");
> m_model_block.setupVAO(m_program);

e depois configurado no mapa através da função Window::setupBlock(), analogamente à função Window::setupCar():

> setupBlock(m_block);

As funções Window::setupCar, Window::setupBlock e Window::randomizeBlock:

> void Window::setupCar(Car &m_car)
> {
>   glm::vec3 const initPos{0.0f, 0.3f, 3.5f};
>   m_car.m_position = initPos;
>   m_car.m_angle = 0.0f;
>   m_car.m_rotationAxis = {0.0f, 1.0f, 0.0f};
> }
> 
> void Window::setupBlock(Block &m_block)
> {
>   std::uniform_real_distribution<float> posXZ(-m_ground.m_maxLimit * 0.9f, +m_ground.m_maxLimit * 0.9f);
>   glm::vec3 const initPos{posXZ(m_randomEngine), 0.3f, posXZ(m_randomEngine)};
>   m_block.m_position = initPos;
>   m_block.m_angle = 0.0f;
>   m_block.m_rotationAxis = {0.0f , 1.0f, 0.0f};
> }
> 
> void Window::randomizeBlock(Block &m_block){
>   std::uniform_real_distribution<float> posXZ(-m_ground.m_maxLimit * 0.9f, +m_ground.m_maxLimit * 0.9f);
>   glm::vec3 const initPos{posXZ(m_randomEngine), 0.3f, posXZ(m_randomEngine)};
>   m_block.m_position = initPos;
> }

Diferente do proposto no exemplo de sala, optamos por não variar o shader do modelo a partir de um comando na interface gráfica. O modelo continua utilizando um único shader.

### Referências

Sites e comandos úteis:
- espaço duplo (' &nbsp;'): cria uma nova linha
- stackedit (https://stackedit.io/app#): site pode ser usado para pré-visualizar readme.md antes de comitá-lo.

Maurício Romano        RA: 11013212  
Henrique Ramos Rissardo Werneck    RA: 11007812

### Projeto 1 de Computação Gráfica
Neste projeto implementamos um protetor de tela com bolhas flutuando, cujos parâmetros de quantidade de objetos e velocidade podem ser contralados pelo usuário.
O ajuste de parâmetros é feito através de uma slide bar ajustada com o mouse, ou por input direto do teclado.
Após o ajuste, é necessário pressionar o botão "Restart" no canto superior esquerdo da tela para que a página reinicie a animação com os novos parâmetros definidos.

O código é descrito destacando três aspectos principais que juntos dão uma visão geral do projeto executado.
O aspecto inicial considerado no projeto foi sua organização. O código foi baseado nos exemplos apresentados em aula, mais especificamente no "Asteroids". Optamos por simplificar o código utilizando apenas uma classe: a classe bolhas. A classe bolhas contém o VBO_position, VBO_color, o VAO e outros atributos.

Outro aspecto importante para o desenvolvimento do projeto foram os shaders. O fragment shaders controla a escala e a transição das bolhas criadas, além das cores das bolhas. Fragment shader utilizado, portanto, é uma combinação do fragment shader do asteroid e do polígono regular. O vertex shader é similar ao do polígno regular, apenas inserindo a cor que foi determinada para o ponto.

Por fim, a interação com o usuário e seu efeito na aplicação completam o projeto. O cálculo da quantidade de objetos e suas velocidades é feito com base nos parâmetros definidos pelo usuário.
- Posição inicial: valor aleatório atribuído a cada bolha;
- Cor: 2 cores atribuídas aleatoriamente para cada bolha;
- Velocidade: velocidade base atribuida pelo usuário. Para cada bublle é somado um valor aleatório para ter uma diferença de velocidade entre as bolhas.

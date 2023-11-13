Maurício Romano RA: 11013212  
Henrique Ramos Rissardo Werneck RA: 11007812

### Projeto 2 de Computação Gráfica
Neste projeto implementamos um modelo de carro controlado pelo usuário. O controle do usuário se dá pelas setas direcionais do teclado, com as setas pra cima e pra baixo controlando a aceleração do carro. É necessário clicar na tela para dar foco à aplicação.

O código é baseado nas atividades de aula "Starfield Effects" e "Look At". O modelo de carro utiliza uma struct Car que é responsável por manter os dados de posição do modelo.
Conforme código em window.cpp, na função Window::onEvent() (linhas 7-36), o carro ganha velocidade a cada evento de tecla pressionada (no caso, setas direcionais ou w/s) até um limite. Quando atingido este limite, o carro continua rodando com velocidade máxima.
Também no arquivo window.cpp, na função Window::onUpdate() (linhas 175-183), foi implementado um sistema de desaceleração, segundo o qual, se o carro para de ser acelerado (acionamento do modelo pelas setas direcionais), o carro gradativamente tem sua velocidade reduzida eventualmente parando.
O carro também é transportado ao lado oposto da tela graças à lógica implementada após a atualização da posição do carro (linhas 185-196).

Foi adicionado uma perspectiva de topo, com relação às perspectivas já apresentadas. O efeito foi conseguido configurando novamente a m_viewMatrix depois do Window::onCreate(). A atualização da m_vewMatrix é feita dentro do Window::onPaintUI(), (linhas 138-163) segundo a lógica de cada uma das possíveis perspectivas escolhidas.
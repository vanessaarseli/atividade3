# Atividade 3 - Computação Gráfica UFABC

Uma sala 3D repleta de cubos com diversas texturas.

Ponto importante: Nessa versão o arquivo abcg_openglwindow.cpp da abcg foi modificada para aceitar Geometry Shader. Foi ffeita uma sobrecarga de dois métodos: createProgramFromString e createProgramFromFile

Ponto Importante 2: Por conta dos métodos: glFramebufferTexture e glDrawBuffers, eu não consegui compilar o WebAssembly. Se eu tivesse usado a model fornecida em aula, talvez eu não tivesse esse problema, mas quando comecei a atividade, essa a aula de Mapeamento de Ambiente ainda não estava disponível, então estudei a parte e desenvolvi essa parte na mão. Quando fui tentar adaptar para usar a model fornecida, o código quebrou inteiro e eu não tive tempo hábil para arrumar.

## Main.cpp

Foi utilizada a Main já fornecida em aula, apenas com a modificação de tamanho de tela para 1920x1080.

## Shadders

Foram utilizadas 2 shaders Shadow e Depth. A Depth além do .ver, .frag também possui um .gs que trata-se de um Geometry Shader, um shader intermediário entre vertex e fragment.

## OpenglWindow

A função handleEvent captura os eventos realizados no mouse e teclado e chama o objeto camera, que fica responsável por processar os movimentos em tela.

Comandos aceitos:

W -> Movimento para frente
D -> Movimento para a direita
A -> Movimento para a esquerda
S -> Movimento para trás

levar o mouse para a direita -> olhar da 1ª pessoa move-se para direita
levar o mouse para a esquerda -> olhar da 1ª pessoa move-se para esquerda
levar o mouse para cima -> olhar da 1ª pessoa move-se para cima
levar o mouse para baixo -> olhar da 1ª pessoa move-se para baixo

### initializeGL

No método initializeGL é setado o plano de vértices que serão utilizados no cubemap através da varíavel float planeVertices[]

As variáveis necessárias para inicialização são setadas com seus respectivos valores.

As 2 shaders são criadas usando o método adaptado da abcg para a Depth createProgramFromFile aceitando 3 parâmetros ao invés de 2.

Ocore também a geração dos Array Vertex e VBO

Nesse momento também é criado o cubemap e suas respectivas texturas na mão, pelo motivo já especificado no Ponto Importante 2

As demais texturas são carregadas usando o loadTextura da própria abcg

  boxTexture = abcg::opengl::loadTexture(getAssetsPath() + "textures/CrashBox.png");
  stoneTexture = abcg::opengl::loadTexture(getAssetsPath() + "textures/StoneWall.png");
  jumpBoxTexture = abcg::opengl::loadTexture(getAssetsPath() + "textures/JumpBox.png");
  bounceBoxTexture = abcg::opengl::loadTexture(getAssetsPath() + "textures/BounceBox.png");
  tntTexture = abcg::opengl::loadTexture(getAssetsPath() + "textures/TNT.png");
  portalTexture = abcg::opengl::loadTexture(getAssetsPath() + "textures/portal_tile.jpg");
  
Nesse momento também é setado o código da textura cubemap e diffusetexture na shader shadow


### paintGL

Primeiramente é setada a variável deltaTime que fica responsável por alimentar os comandos e velocidade de mouse e teclado. Foi criado um método chamado RenderScene() que ficará responsável por renderizar a cena e chamar o RenderCube para cada cubo que será criado na tela. Esse método RenderScene é chamado 2x, 1 para cada shader.

### paintGL

Como meu intuito era simular a tela de um jogo, deixei esse método vazio para que a tela ficasse limpa

### RenderScene

Recebe um GLuint programId responsável por identificar qual a shader que está sendo usada no momento. Para cada cubo ele define qual será a textura daquele cubo

ActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, boxTexture);


E cria a matriz referente ao respectivo cubo

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0f));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.75f));
 glUniformMatrix4fv(modelUn, 1, GL_FALSE, &model[0][0]);
 
 Ao final, ele chama a função renderCube(), responsável por plotar o cubo na tela com o tamanho pré-definido.
 
   renderCube();

### RenderCube

O método já inicia criando cada uma das faces do cubo, na variável float vertices[]. Além disso são criados o Array Vertex e cubeVBO. Foi utilizado um VBO e VAO somente para os cubos e outro somente para o cubemap.

## Camera

### GetViewMatrix()

Seta qual será a matriz de visualização a cada cena.

### ProcessKeyboard

Processa os movimentos realizados no teclado e que foram capturados pelo eventhandler na openwindow.cpp

Ele seta qual será a velocidade do movimento: float velocity = MovementSpeed * deltaTime;
E adiciona ou remove valores a variavel Position de acordo com o tipo de movimento realizado e velocidade estabelecida

  if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;

### ProcessMouseMovement

Processa os movimentos do mouse modificando as variáveis 
    Yaw += xoffset;
    Pitch += yoffset;
    
e chama o UpdateCameraVectors() que atualizará cada um dos critério utilizados na matriz de visualização.

###  UpdateCameraVectors

Atualiza as varíaveis

    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
    
 que são utilizadas no GetViewMatrix(), chamado a cada cena para setar a visualização da camera.
 
 ## itoa.h
 
 Biblioteca externa usada para converter inteiro em char*
 
Vídeo da compilação e execução no VSC: https://www.youtube.com/watch?v=CCF2DYq4Da0
 
#### Credits
    Source Code:
                https://learnopengl.com/
                https://github.com/hbatagelo/abcg

## Author

Vanessa Arseli
RA: 11117510

## License

MIT

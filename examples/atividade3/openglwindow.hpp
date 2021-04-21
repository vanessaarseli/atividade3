#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <string_view>

#include "abcg.hpp"
#include "camera.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
 
  Camera camera;
  float deltaTime; //mensura a diferença de tempo entre frames

  unsigned int planeVAO;
  unsigned int planeVBO;
  unsigned int cubeVAO;  
  unsigned int cubeVBO;
  unsigned int quadVAO;
  unsigned int quadVBO;

  bool shadows;

  //Shaders
  const std::string m_shadowName = "shadow";
  const std::string m_depthName = "depth";
  GLuint m_shadowProgram{};
  GLuint m_depthProgram{};

  //framebuffer
  unsigned int depthMapFBO;
  unsigned int depthCubeMap;
  const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

  //texturas
   unsigned int woodTexture, boxTexture, stoneTexture, jumpBoxTexture, bounceBoxTexture, tntTexture, portalTexture;
 

  glm::vec3 lightPos{0.0f, 0.0f, 0.0f};

  //Funcoes de renderizacao
  void renderScene(GLuint programId);
  void renderCube();

  //Tamanho da tela considerado
  const unsigned int SCR_WIDTH = 1920;
  const unsigned int SCR_HEIGHT = 1080;

  //controle do mouse
  float lastX;
  float lastY;
  bool firstMouse;

  
};

#endif
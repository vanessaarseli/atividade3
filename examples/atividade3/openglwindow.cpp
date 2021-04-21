#include "openglwindow.hpp"
#include <fmt/core.h>
#include <imgui.h>
#include "IMGUI/imgui_impl_opengl3.h"
#include <cppitertools/itertools.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "itoa.h"
#include "imfilebrowser.h"
#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/hash.hpp>
#include <unordered_map>

void OpenGLWindow::handleEvent(SDL_Event& event) {
   if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w)
      camera.ProcessKeyboard(FORWARD, deltaTime);
    if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s)
     camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a)
      camera.ProcessKeyboard(LEFT, deltaTime);
    if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d)
      camera.ProcessKeyboard(RIGHT, deltaTime);

 
  }

   if (event.type == SDL_MOUSEMOTION) {
      glm::ivec2 mousePosition;
      SDL_GetMouseState(&mousePosition.x, &mousePosition.y);


		if (firstMouse)
    	{
        lastX = mousePosition.x;
        lastY = mousePosition.y;
        firstMouse = false;
    	}

      	camera.ProcessMouseMovement(mousePosition.x - lastX, lastY - mousePosition.y);
		lastX = mousePosition.x;
        lastY = mousePosition.y;
   }

}

void OpenGLWindow::initializeGL() {
  glClearColor(0, 0, 0, 1);
  deltaTime = static_cast<float>(getDeltaTime());
  float planeVertices[] = {
		// posicoes            // normals         // texcoords
		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
		 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
	};

  shadows = true;
  firstMouse = true;
  planeVAO = 0;
  planeVBO = 0;
  cubeVAO = 0;
  cubeVBO = 0;
  quadVAO = 0;
  quadVBO = 0;
  
  lastX =  800.0f / 2.0;
  lastY =  600.0 / 2.0;

  glClearColor(0, 0, 0, 1);

  // Habilita depth buffering
  glEnable(GL_DEPTH_TEST);

  //cria as shaders Shadow e Depth
  auto path{getAssetsPath() + "shaders/" + m_shadowName};
   m_shadowProgram = createProgramFromFile(path + ".vert", path + ".frag");

  auto pathDepth{getAssetsPath() + "shaders/" + m_depthName};
   m_depthProgram = createProgramFromFile(pathDepth + ".vert", pathDepth + ".frag", pathDepth + ".gs");

  // Geração dos Array Vertex e VBO
  glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);

	glBindVertexArray(planeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	glBindVertexArray(0);

  //Framebuffer
  glGenFramebuffers(1, &depthMapFBO);

    glGenTextures(1,&depthCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);

	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  //Carregamento das texturas
  boxTexture = abcg::opengl::loadTexture(getAssetsPath() + "textures/CrashBox.png");
  stoneTexture = abcg::opengl::loadTexture(getAssetsPath() + "textures/StoneWall.png");
  jumpBoxTexture = abcg::opengl::loadTexture(getAssetsPath() + "textures/JumpBox.png");
  bounceBoxTexture = abcg::opengl::loadTexture(getAssetsPath() + "textures/BounceBox.png");
  tntTexture = abcg::opengl::loadTexture(getAssetsPath() + "textures/TNT.png");
  portalTexture = abcg::opengl::loadTexture(getAssetsPath() + "textures/portal_tile.jpg");

  glUseProgram(m_shadowProgram);

  // Pega a localizacao das uniform variables
  GLint diffuseTexture{glGetUniformLocation(m_shadowProgram, "diffuseTexture")};
  GLint shadowMap{glGetUniformLocation(m_shadowProgram, "shadowMap")};

  // Altera seus valores
  glUniform1i(diffuseTexture, 0);
  glUniform1i(shadowMap, 1);

    ImGui::StyleColorsDark();

  glUseProgram(0);

   //ImGui::CreateContext();



 
  
}


void OpenGLWindow::paintGL() {
  deltaTime = static_cast<float>(getDeltaTime());
  lightPos.z = sin(SDL_GetTicks() * 0.5) * 3.0;

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 0 - Cricao das matrizes de transformacao(depth cubemap)
		float near_plane = 0.1f, far_plane = 25.0f;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

    // 1 - Renderiza a cena (depth cubemap) 
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	    glClear(GL_DEPTH_BUFFER_BIT);

    	
    	ImGui_ImplOpenGL3_NewFrame();

  glUseProgram(m_depthProgram);

    for (unsigned int i = 0; i < 6; ++i){

    char num[10];
    itoa(i,num);
    
    char *str = new char[strlen("shadowMatrices[") + strlen(num) + strlen("]") + 1];

     GLint shadowMatrices{glGetUniformLocation(m_depthProgram, str)};
     glUniformMatrix4fv(shadowMatrices, 1, GL_FALSE, &shadowTransforms[i][0][0]);
     
    }


    GLint far_planeUn{glGetUniformLocation(m_depthProgram, "far_plane")};
    GLint lightPosUn{glGetUniformLocation(m_depthProgram, "lightPos")};
  
    glUniform1f(far_planeUn, far_plane);
    glUniform3f(lightPosUn, lightPos.x, lightPos.y, lightPos.z);

    renderScene(m_depthProgram);

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     
  glUseProgram(0);

  // 2 - Renderizar cena usando o map depth/shadow 
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
    
  glUseProgram(m_shadowProgram);

    GLint projectionUn{glGetUniformLocation(m_shadowProgram, "projection")};
    GLint viewUn{glGetUniformLocation(m_shadowProgram, "view")};
    GLint lightPosShadowUn{glGetUniformLocation(m_shadowProgram, "lightPos")};
    GLint shadowsUn{glGetUniformLocation(m_shadowProgram, "shadows")};
    GLint far_planeShadowUn{glGetUniformLocation(m_shadowProgram, "far_plane")};

    glUniformMatrix4fv(projectionUn, 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(viewUn, 1, GL_FALSE, &view[0][0]);

    glUniform3f(viewUn, camera.Position.x, camera.Position.y, camera.Position.z);
    glUniform3f(lightPosShadowUn, lightPos.x, lightPos.y, lightPos.z);

    glUniform1i(shadowsUn, shadows);
    glUniform1f(far_planeShadowUn, far_plane);

    	glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, stoneTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);

    renderScene(m_shadowProgram);

   glUseProgram(0);


}

void OpenGLWindow::paintUI() {
  
  
}

void OpenGLWindow::renderScene(GLuint programId)
{

  glm::mat4 model = glm::mat4(1.0f);
model = glm::scale(model, glm::vec3(5.0f));

  GLint modelUn{glGetUniformLocation(programId, "model")};
  glUniformMatrix4fv(modelUn, 1, GL_FALSE, &model[0][0]);
  glDisable(GL_CULL_FACE);   

  GLint reverse_normalsUn{glGetUniformLocation(programId, "reverse_normals")};
  glUniform1i(reverse_normalsUn, 1);

  renderCube();

glUniform1i(reverse_normalsUn, 0);
  glEnable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, boxTexture);

  //Cubos
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
  glUniformMatrix4fv(modelUn, 1, GL_FALSE, &model[0][0]);
  renderCube();

  glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, jumpBoxTexture);

  model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0f));
	model = glm::scale(model, glm::vec3(0.75f));
  glUniformMatrix4fv(modelUn, 1, GL_FALSE, &model[0][0]);
  renderCube();

  glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bounceBoxTexture);
  model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
  glUniformMatrix4fv(modelUn, 1, GL_FALSE, &model[0][0]);
  renderCube();

  glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tntTexture);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5f));
	model = glm::scale(model, glm::vec3(0.5f));
   glUniformMatrix4fv(modelUn, 1, GL_FALSE, &model[0][0]);
  renderCube();

  glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, boxTexture);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0f));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.75f));
  glUniformMatrix4fv(modelUn, 1, GL_FALSE, &model[0][0]);
  renderCube();

   
   glBindFramebuffer(GL_FRAMEBUFFER, 0);


}

void OpenGLWindow::renderCube()
{
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// face de trás
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // abaixo-esquerda
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // acima-direita
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // abaixo-direita         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // acima-direita
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // abaixo-esquerda
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // acima-esquerda
			// face da frente
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // abaixo-esquerda
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // abaixo-direita
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // acima-direita
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // acima-direita
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // acima-esquerda
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // abaixo-esquerda
			// face da esquerda
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // acima-direita
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // acima-esquerda
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // abaixo-esquerda
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // abaixo-esquerda
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // abaixo-direita
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // acima-direita
			// face da direita
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // acima-esquerda
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // abaixo-direita
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // acima-direita         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // abaixo-direita
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // acima-esquerda
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // abaixo-esquerda     
			// face de baixo
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // acima-direita
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // acima-esquerda
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // abaixo-esquerda
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // abaixo-esquerda
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // abaixo-direita
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // acima-direita
			// face de cima
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // acima-esquerda
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // abaixo-direita
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // acima-direita     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // abaixo-direita
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // acima-esquerda
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // abaixo-esquerda        
		};

		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);

		glBindVertexArray(cubeVAO);

		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		glBindVertexArray(0);
	}

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void OpenGLWindow::resizeGL(int width, int height) {
 

}

void OpenGLWindow::terminateGL() {


}



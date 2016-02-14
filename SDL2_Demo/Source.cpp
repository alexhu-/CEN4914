#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <string>

#include "Camera.h"
#include "Importer.h"
#include "Model.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "UIRectangle.h"

void matrixExamples()
{
	// create a default identity matrix
	glm::mat4 trans;
	// rotate multiplies identity matrix by rotation transformation 180 degrees around z-axis
	trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 0.1f));

	// test out above matrix
	glm::vec4 result = trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	printf("%f %f %f\n", result.x, result.y, result.z);
}

void setBoneTransforms(GLuint shader, GLuint index, glm::mat4 transform)
{
	std::string uniformName("gBones[" + std::to_string(index) + "]");
	GLuint gBones = glGetUniformLocation(shader, uniformName.c_str());
	glUniformMatrix4fv(gBones, 1, GL_FALSE, glm::value_ptr(transform));
}

int main(int argc, char *argv[])
{
	// change colors with time
	//auto t_start = std::chrono::high_resolution_clock::now();

	char *vertexShaderFile = "tutorialVertexShader.txt";
	char *fragmentShaderFile = "tutorialFragmentShader.txt";
	char *shapeVertexShaderFile = "UIVertexShader.txt";
	char *shapeFragmentShaderFile = "UIFragmentShader.txt";

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	SDL_Window *window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	glewInit();
	
	// stuff

	GLfloat vertices[] = {
		0.5f, 0.5f, 0.5f, // face 1
		0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, -0.5f, // face 2
		0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, 0.5f, -0.5f
	};

	GLuint elements[] = {
		1, 0, 2, // sq 1 front
		3, 0, 2,
		7, 3, 4, // sq 2 top
		0, 3, 4,
		7, 6, 4, // sq 3 back
		5, 6, 4,
		0, 4, 1, // sq 4 right
		5, 4, 1,
		5, 1, 6, // sq 5 bottom
		2, 1, 6,
		7, 3, 6, // sq 6 left
		2, 3, 6
	};

	// Shape shaders
	ShaderProgram shapeShaderProgram(shapeVertexShaderFile, shapeFragmentShaderFile);
	shapeShaderProgram.compileShaderProgram();
	GLuint shapeProgramId = shapeShaderProgram.getProgram();
	shapeShaderProgram.use();

	glBindFragDataLocation(shapeProgramId, 0, "outColor");

	GLint shapeUniColor = glGetUniformLocation(shapeProgramId, "inColor");

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindVertexArray(0);

	UIRectangle sampleRect(-5.0f, 5.0f, 10.0f, 5.0f, glm::vec4(0.5f, 0.0f, 0.5f, 1.0f));
	sampleRect.setZ(-5.0f);
	sampleRect.setup();


	// Model shaders

	ShaderProgram shaderProgram(vertexShaderFile, fragmentShaderFile);
	shaderProgram.compileShaderProgram();
	GLuint shaderProgramId = shaderProgram.getProgram();
	shaderProgram.use();

	// not necessary since there is only one output anyways
	glBindFragDataLocation(shaderProgramId, 0, "outColor");

	//GLint posAttrib = glGetAttribLocation(shaderProgramId, "position");
	//glEnableVertexAttribArray(posAttrib); // enable vertex attribute array
	//glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);

	// get location of uniform attribute
	GLint sampler = glGetUniformLocation(shaderProgramId, "texture_sampler");
	//glUniform3f(uniColor, 1.0f, 0.0f, 0.0f);


	// assimp stuff
	
	std::string nanopath("C:\\Users\\Alex Hu\\Documents\\Model\\nanosuit\\nanosuit.obj");
	std::string nanotexture("C:\\Users\\Alex Hu\\Documents\\Model\\nanosuit");
	Model nanoModel = Importer::loadModel(nanopath, nanotexture);
	nanoModel.setup();
	nanoModel.scale(0.1f, 0.1f, 0.1f);
	nanoModel.translate(-10.0f, 0.0f, 0.0f);

	std::string gpath("C:\\Users\\Alex Hu\\Documents\\Model\\Girl\\girl.obj");
	std::string gtexture("C:\\Users/Alex Hu/Documents/Model/Girl/Texture");
	Model girlModel = Importer::loadModel(gpath, gtexture);
	girlModel.setup();
	girlModel.scale(0.2f, 0.2f, 0.2f);
	girlModel.translate(5.0f, 0.0f, 0.0f);
	girlModel.rotateX(-90.0f);

	std::string path("C:\\Users/Alex Hu/Documents/Model/bob/bob_lamp_update_export.md5mesh");
	std::string texture("C:\\Users/Alex Hu/Documents/Model/bob/");
	//std::string path("C:\\Users/Alex Hu/Documents/Model/agent/Agent_FBX/AgentWalk.fbx");
	//std::string texture("C:\\Users/Alex Hu/Documents/Model/agent/Character");
	Model bobModel = Importer::loadModel(path, texture);
	bobModel.setup();
	bobModel.scale(0.2f, 0.2f, 0.2f);
	bobModel.rotateX(-90.0f);


// ----------------------------------------------------------------------------------------------
	// stuff for 3d stuff
	// 3d stuff now

	glm::mat4 model = glm::mat4();
	//model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	GLint uniModel = glGetUniformLocation(shaderProgramId, "model");
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

	// make our camera matrix
	glm::vec3 cameraLocation = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraLook = glm::vec3(0.0f, 0.0f, 2.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 goUp = glm::vec3(0.0f, 0.25f, 0.0f);
	glm::vec3 goRight = glm::vec3(0.25f, 0.0f, 0.0f);
	glm::vec3 goForward = glm::vec3(0.0f, 0.0f, -0.25f);
	glm::vec3 lookUp = glm::vec3(0.0f, 0.0f, 0.0f);
	float angleUp = glm::radians(180.0f);
	float angleRight = glm::radians(180.0f);
	float fullCircle = glm::radians(360.0f);
	float moveUp = glm::radians(5.0f);
	float ninety = glm::radians(90.0f);
	/*glm::mat4 view = glm::lookAt(
		cameraLocation, // camera location
		cameraLook, // where the camera is looking at
		cameraUp // which way is up, we define z-axis is up so xy plane is ground
		);*/
	Camera camera(cameraLocation, glm::vec3(0.0f, 0.0f, -1.0f), cameraUp);
	glm::mat4 view = camera.getViewMatrix();
	GLint uniView = glGetUniformLocation(shaderProgramId, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj = glm::perspective(
		45.0f, // vertical field of view
		800.0f / 600.0f,	 // aspect ratio of screen 
		0.1f,				 // near planes - clipping planes, anything outside is clipped
		100.0f				 // far planes - same as above
		);
	GLint uniProj = glGetUniformLocation(shaderProgramId, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
	

	shapeShaderProgram.use();

	GLint uniModelShape = glGetUniformLocation(shapeProgramId, "model");
	glUniformMatrix4fv(uniModelShape, 1, GL_FALSE, glm::value_ptr(model));
	GLint uniViewShape = glGetUniformLocation(shapeProgramId, "view");
	glUniformMatrix4fv(uniViewShape, 1, GL_FALSE, glm::value_ptr(view));
	GLint uniProjShape = glGetUniformLocation(shapeProgramId, "proj");
	glUniformMatrix4fv(uniProjShape, 1, GL_FALSE, glm::value_ptr(proj));

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
//--------------------------------------------------------------------------------------------------------

	Scene bobScene;
	bobScene.addModel(&bobModel);

	Scene girlScene;
	girlScene.addModel(&girlModel);
	girlScene.addModel(&nanoModel);

	bool displayBobScene = true;
	bool displayGirlScene = true;

	unsigned int prevTime = SDL_GetTicks();
	unsigned int fpsArray[] = {16, 17, 17};
	unsigned int fpsCount = 0;
	float fpsAverage = 0;
	unsigned int startTime = prevTime;

	unsigned int currentFrame = 0;
	bool playAnimation = false;

	unsigned int frameCount = 0;
	bool rectReverse = true;
	float rectWidth = 10.0f;

	SDL_Event windowEvent;
	bool isRunning = true;
	while (isRunning)
	{
		if (SDL_PollEvent(&windowEvent))
		{
			if ((windowEvent.type == SDL_QUIT) || (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE))
			{
				isRunning = false;
				break;
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_LEFT))
			{
				camera.translateRight(-0.25);
				//glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_RIGHT))
			{
				camera.translateRight(0.25);
				//glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_UP))
			{
				camera.translateUp(0.25);
				//glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_DOWN))
			{
				camera.translateUp(-0.25);
				//glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_z))
			{
				camera.translateForward(0.25);
				//glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_x))
			{
				camera.translateForward(-0.25);
				//glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_w))
			{
				camera.rotateUp(6.0f);
				//glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_s))
			{
				camera.rotateUp(-6.0f);
				//glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_a))
			{
				camera.rotateRight(3.0f);
				//glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_d))
			{
				camera.rotateRight(-3.0f);
				//glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_p))
			{
			
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_1))
			{
				displayBobScene = !displayBobScene;
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_2))
			{
				displayGirlScene = !displayGirlScene;
			}

			glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
		}

		// Clear the screen to black
		glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderProgram.use();

		unsigned int runningTime = SDL_GetTicks() - startTime;
		if (displayBobScene)
		{
			bobScene.render(shaderProgramId, sampler, uniModel, setBoneTransforms, runningTime);
		}
		if (displayGirlScene)
		{
			girlScene.render(shaderProgramId, sampler, uniModel, setBoneTransforms, runningTime);
		}

		shapeShaderProgram.use();
		glUniformMatrix4fv(uniViewShape, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
		/*glUniform4f(shapeUniColor, 1.0f, 0.0f, 0.0f, 1.0f);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);*/

		if (frameCount >= 15)
		{
			sampleRect.setWidth(rectWidth);
			if (rectReverse)
			{
				rectWidth -= 1.0f;
			}
			else
			{
				rectWidth += 1.0f;
			}
			if (rectWidth <= 0.0f)
			{
				rectReverse = false;
			}
			if (rectWidth >= 10.0f)
			{
				rectReverse = true;
			}
			frameCount = 0;
		}
		++frameCount;
		sampleRect.draw(shapeUniColor);

		shaderProgram.use();

		unsigned int currTime = SDL_GetTicks();
		unsigned int elapsed = currTime - prevTime;

		while (elapsed < fpsArray[(fpsCount%60) / 20])
		{
			currTime = SDL_GetTicks();
			elapsed = currTime - prevTime;
		}

		prevTime = currTime;

		fpsAverage = fpsAverage + (1000.0f / (float)elapsed);

		std::string fpsTemp = std::to_string(fpsAverage/(float)(fpsCount + 1));
		const char *fpsString = fpsTemp.c_str();
		SDL_SetWindowTitle(window, fpsString);

		++fpsCount;

		SDL_GL_SwapWindow(window);
	}

	shaderProgram.cleanup();
	shapeShaderProgram.cleanup();

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);

	sampleRect.cleanup();

	bobModel.clearGLBuffers();
	girlModel.clearGLBuffers();
	nanoModel.clearGLBuffers();

	SDL_GL_DeleteContext(context);

	SDL_Quit();
	return 0;
}
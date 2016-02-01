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
#include "Importer.h"
#include "Model.h"
#include "Camera.h"

void compileVertexShader(char *fileName, GLuint vertexShader)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		std::cout << "Cannot open file" << std::endl;
		return;
	}

	// get length of file
	file.seekg(0, file.end);
	std::size_t length = file.tellg();
	file.seekg(0, file.beg);

	// check if we need null terminator...
	char *shaderString = new char[length + 1];
	shaderString[length] = 0;
	file.read(shaderString, length);

	glShaderSource(vertexShader, 1, &shaderString, NULL);

	glCompileShader(vertexShader);

	GLint status;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
	{
		printf("Vertex shader compiled successfully\n");
	}
	else
	{
		char buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
		printf("Vertex shader failed to compile\n");
		std::cout << buffer << std::endl;
	}

	// clean up string after using it...
	delete[] shaderString;
}

void compileFragmentShader(char *fileName, GLuint fragmentShader)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		std::cout << "Cannot open file" << std::endl;
		return;
	}

	// get length of file
	file.seekg(0, file.end);
	std::size_t length = file.tellg();
	file.seekg(0, file.beg);

	// check if we need null terminator...
	char *shaderString = new char[length + 1];
	shaderString[length] = 0;
	file.read(shaderString, length);

	glShaderSource(fragmentShader, 1, &shaderString, NULL);
	glCompileShader(fragmentShader);

	GLint status;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status == GL_TRUE)
	{
		printf("Fragment shader compiled successfully\n");
	}
	else
	{
		char buffer[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
		printf("Fragment shader failed to compile\n");
		std::cout << buffer << std::endl;
	}

	// delete the new string
	delete[] shaderString;
}

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
	//matrixExamples();

	// change colors with time
	auto t_start = std::chrono::high_resolution_clock::now();

	/*char *vertexShaderString = "#version 150\n\nin vec2 position;\n\nvoid main()\n{\ngl_Position = vec4(position, 0.0, 1.0);\n}";
	char *fragmentShaderString = "#version 150\n\nout vec4 outColor;\n\nvoid main()\n{outColor = vec4(1.0, 1.0, 1.0, 1.0);\n}";*/
	char *vertexShaderFile = "tutorialVertexShader.txt";
	char *fragmentShaderFile = "tutorialFragmentShader.txt";

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	SDL_Window *window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	glewInit();
	
	// stuff

	/*GLfloat vertices[] = {
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
	glBindVertexArray(0);*/

	// shader stuff
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	compileVertexShader(vertexShaderFile, vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	compileFragmentShader(fragmentShaderFile, fragmentShader);


	// create shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// not necessary since there is only one output anyways
	glBindFragDataLocation(shaderProgram, 0, "outColor");

	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib); // enable vertex attribute array
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), 0);

	// get location of uniform attribute
	GLint sampler = glGetUniformLocation(shaderProgram, "texture_sampler");
	//glUniform3f(uniColor, 1.0f, 0.0f, 0.0f);



	// assimp stuff
	
	//std::string path("C:\\Users\\Alex Hu\\Documents\\Model\\nanosuit\\nanosuit.obj");
	//std::string texture("C:\\Users\\Alex Hu\\Documents\\Model\\nanosuit");
	std::string gpath("C:\\Users\\Alex Hu\\Documents\\Model\\Girl\\girl.obj");
	std::string gtexture("C:\\Users/Alex Hu/Documents/Model/Girl/Texture");
	Model girlModel = Importer::loadModel(gpath, gtexture);
	girlModel.setup();

	// need to fix all these texture loading and stuff...
	// TODO: figure out how to do skinning technique or w/e

	/*std::string path("C:\\Users/Alex Hu/Documents/Model/rifle/");
	std::string textureDirectory("C:\\Users/Alex Hu/Documents/Model/rifle/");
	std::vector<std::string> fileNames;

	//Model models[3];

	int size = 10;
	int size2 = 11;

	for (int i = 0; i < size; ++i)
	{
		std::string modelName = "cz805 With Animation_00000" + std::to_string(i) + ".obj";
		//models[i] = loadModel(path + modelName, textureDirectory);
		//std::cout << "Finished loading frame " << i << std::endl;
		fileNames.push_back(modelName);
	}

	for (int i = size; i < size2; ++i)
	{
		std::string modelName = "cz805 With Animation_0000" + std::to_string(i) + ".obj";
		fileNames.push_back(modelName);
	}


	Animation animation = loadAnimation(path, fileNames, textureDirectory);*/

	std::string path("C:\\Users/Alex Hu/Documents/Model/bob/bob_lamp_update_export.md5mesh");
	std::string texture("C:\\Users/Alex Hu/Documents/Model/bob/");
	//std::string path("C:\\Users/Alex Hu/Documents/Model/agent/Agent_FBX/AgentWalk.fbx");
	//std::string texture("C:\\Users/Alex Hu/Documents/Model/agent/Character");
	Model bobModel = Importer::loadModel(path, texture);
	bobModel.setup();


// ----------------------------------------------------------------------------------------------
	// stuff for 3d stuff
	// 3d stuff now

	glm::mat4 model = glm::mat4();
	//model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
	GLint uniModel = glGetUniformLocation(shaderProgram, "model");
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
	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj = glm::perspective(
		45.0f, // vertical field of view
		800.0f / 600.0f,	 // aspect ratio of screen 
		0.1f,				 // near planes - clipping planes, anything outside is clipped
		100.0f				 // far planes - same as above
		);
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	//glDepthFunc(GL_LESS);
//--------------------------------------------------------------------------------------------------------

	unsigned int prevTime = SDL_GetTicks();
	unsigned int fpsArray[] = {16, 17, 17};
	unsigned int fpsCount = 0;
	float fpsAverage = 0;
	unsigned int startTime = prevTime;

	unsigned int currentFrame = 0;
	bool playAnimation = false;

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
				/*cameraLocation -= goRight;
				cameraLook -= goRight;
				view = glm::lookAt(cameraLocation, cameraLook, cameraUp);\
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));*/
				camera.translateRight(-0.25);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_RIGHT))
			{
				/*cameraLocation += goRight;
				cameraLook += goRight;
				view = glm::lookAt(cameraLocation, cameraLook, cameraUp);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));*/
				camera.translateRight(0.25);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_UP))
			{
				/*cameraLocation += goUp;
				cameraLook += goUp;
				view = glm::lookAt(cameraLocation, cameraLook, cameraUp);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));*/
				camera.translateUp(0.25);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_DOWN))
			{
				/*cameraLocation -= goUp;
				cameraLook -= goUp;
				view = glm::lookAt(cameraLocation, cameraLook, cameraUp);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));*/
				camera.translateUp(-0.25);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_z))
			{
				/*cameraLocation += goForward;
				cameraLook += goForward;
				view = glm::lookAt(cameraLocation, cameraLook, cameraUp);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));*/
				camera.translateForward(0.25);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_x))
			{
				/*cameraLocation -= goForward;
				cameraLook -= goForward;
				view = glm::lookAt(cameraLocation, cameraLook, cameraUp);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));*/
				camera.translateForward(-0.25);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_w))
			{
				/*angleUp -= moveUp;
				angleUp = std::fmod(angleUp, fullCircle);
				if (angleUp < 0.0f)
				{
					angleUp += fullCircle;
				}
				glm::vec3 newCameraLook1(0.0f, glm::sin(angleUp), glm::cos(angleUp));
				//glm::vec3 newCameraLook2(glm::sin(angleRight), 0.0f, glm::cos(angleRight));
				cameraLook = newCameraLook1 /*+ newCameraLook2*/ /*+ cameraLocation;
				cameraUp = glm::vec3(0.0f, glm::sin(angleUp - ninety), glm::cos(angleUp - ninety));
				view = glm::lookAt(cameraLocation, cameraLook, cameraUp);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));*/
				camera.rotateUp(6.0f);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_s))
			{
				/*angleUp += moveUp;
				angleUp = std::fmod(angleUp, fullCircle);
				glm::vec3 newCameraLook1(0.0f, glm::sin(angleUp), glm::cos(angleUp));
				glm::vec3 newCameraLook2(glm::sin(angleRight), 0.0f, glm::cos(angleRight));
				cameraLook = newCameraLook1 + newCameraLook2 + cameraLocation;
				cameraUp = glm::vec3(0.0f, glm::sin(angleUp - ninety), glm::cos(angleUp - ninety));
				view = glm::lookAt(cameraLocation, cameraLook, cameraUp);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));*/
				camera.rotateUp(-6.0f);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_a))
			{
				/*angleRight += moveUp;
				angleRight = std::fmod(angleRight, fullCircle);
				glm::vec3 newCameraLook1(0.0f, glm::sin(angleUp), glm::cos(angleUp));
				glm::vec3 newCameraLook2(glm::sin(angleRight), 0.0f, glm::cos(angleRight));
				cameraLook = newCameraLook1 + newCameraLook2 + cameraLocation;
				cameraUp = glm::vec3(0.0f, glm::sin(angleUp - ninety), glm::cos(angleUp - ninety));
				view = glm::lookAt(cameraLocation, cameraLook, cameraUp);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));*/
				camera.rotateRight(3.0f);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_d))
			{
				/*angleRight -= moveUp;
				angleRight = std::fmod(angleRight, fullCircle);
				if (angleRight < 0.0f)
				{
					angleRight += fullCircle;
				}
				glm::vec3 newCameraLook1(0.0f, glm::sin(angleUp), glm::cos(angleUp));
				glm::vec3 newCameraLook2(glm::sin(angleRight), 0.0f, glm::cos(angleRight));
				cameraLook = newCameraLook1 + newCameraLook2 + cameraLocation;
				cameraUp = glm::vec3(0.0f, glm::sin(angleUp - ninety), glm::cos(angleUp - ninety));
				view = glm::lookAt(cameraLocation, cameraLook, cameraUp);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));*/
				camera.rotateRight(-3.0f);
				glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_p))
			{
				/*if (animation.isAnimationPlaying())
				{
					animation.resetAnimation();
				}

				animation.toggleAnimation();*/
			}
		}

		// Clear the screen to black
		glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*if (animation.currentItem())
		{
			animation.currentItem()->draw(sampler);
		}

		animation.nextItem();

		currentFrame = currentFrame % size;*/

		unsigned int runningTime = SDL_GetTicks() - startTime;
		std::vector<glm::mat4> boneTransforms = bobModel.getBoneTransforms((float)runningTime, 0);

		for (unsigned int i = 0; i < boneTransforms.size(); ++i)
		{
			if (i < 100)
			{
				setBoneTransforms(shaderProgram, i, boneTransforms[i]);
			}
		}
		bobModel.draw(sampler);

		for (unsigned int i = 0; i < 100; ++i)
		{
			setBoneTransforms(shaderProgram, i, glm::mat4(1.0f));
		}

		girlModel.draw(sampler);

		/*glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);*/

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

	glDeleteProgram(shaderProgram);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);

	/*glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);*/

	bobModel.clearGLBuffers();
	girlModel.clearGLBuffers();
	//animation.clearGLBuffers();
	

	SDL_GL_DeleteContext(context);

	SDL_Quit();
	return 0;
}
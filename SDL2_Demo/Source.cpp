#define GLEW_STATIC
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>
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
#include <ft2build.h>
#include FT_FREETYPE_H
#include <SDL_mixer.h>

#include "Camera.h"
#include "CharacterData.h"
#include "CharacterState.h"
#include "CharacterStateManager.h"
#include "Importer.h"
#include "Model.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "UIRectangle.h"

#include "MyAnimatedMeshClass.h"


/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;


void RenderText(ShaderProgram &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, GLuint vao, GLuint vbo);

void setBoneTransforms(GLuint shader, GLuint index, glm::mat4 transform)
{
	std::string uniformName("gBones[" + std::to_string(index) + "]");
	GLuint gBones = glGetUniformLocation(shader, uniformName.c_str());
	glUniformMatrix4fv(gBones, 1, GL_FALSE, glm::value_ptr(transform));
}

int main(int argc, char *argv[])
{
	// the width and height of the screen in pixels
	unsigned int WINDOW_WIDTH = 1024;
	unsigned int WINDOW_HEIGHT = 576;

	//The music that will be played
	Mix_Music *gMusic = NULL;

	char *vertexShaderFile = "tutorialVertexShader.txt";
	char *fragmentShaderFile = "tutorialFragmentShader.txt";
	char *shapeVertexShaderFile = "UIVertexShader.txt";
	char *shapeFragmentShaderFile = "UIFragmentShader.txt";
	char *textVertexShaderFile = "textVertexShader.txt";
	char *textFragmentShaderFile = "textFragmentShader.txt";

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	}
	//Load music
	gMusic = Mix_LoadMUS("breakout.mp3");
	if (gMusic == NULL)
	{
		printf("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
	}

	SDL_Joystick* gGameController = NULL;
	std::vector<SDL_Joystick*> joysticks;
	//Check for joysticks
	if (SDL_NumJoysticks() < 1)
	{
		printf("Warning: No joysticks connected!\n");
	}
	else
	{
		printf("Controllers are connected:\n");

		for (unsigned int i = 0; i < SDL_NumJoysticks(); i++)
		{
			joysticks.push_back(SDL_JoystickOpen(i));
			printf("    %s\n", SDL_JoystickName(joysticks[i]));
		}
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_Window *window = SDL_CreateWindow("OpenGL", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	glewInit();

	for (int i = 0; i < SDL_NumJoysticks(); ++i) {
		if (SDL_IsGameController(i)) {
			printf("Joystick %i is supported by the game controller interface!\n", i);
		}
	}

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

	// Compile and setup the text shader
	ShaderProgram textShaderProgram(textVertexShaderFile, textFragmentShaderFile);
	textShaderProgram.compileShaderProgram();
	GLuint textProgramId = textShaderProgram.getProgram();
	textShaderProgram.use();
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(WINDOW_WIDTH), 0.0f, static_cast<GLfloat>(WINDOW_HEIGHT));
	glUniformMatrix4fv(glGetUniformLocation(textShaderProgram.getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// FreeType
	FT_Library ft;
	// All functions return a value different than 0 whenever an error occurred
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	// Load font as face
	FT_Face face;
	if (FT_New_Face(ft, "docktrin.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, 48);

	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
			);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	GLuint vbo;
	GLuint vao;
	// Configure VAO/VBO for texture quads
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	UIRectangle sampleRect(-5.0f, 5.0f, 10.0f, 5.0f, glm::vec4(0.5f, 0.0f, 0.5f, 1.0f));
	sampleRect.setZ(-1.0f);
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


	//std::string nanopath("C:\\Users\\Alex Hu\\Desktop\\Downloads\\po7q2ieuk3r4-Body_Mesh_Rigged\\file2\\Body_Mesh_Rigged.fbx");
	std::string nanopath("Models\\Body_Mesh_Rigged.fbx");
	//std::string nanotexture("C:\\Users\\Alex Hu\\Desktop\\Downloads\\po7q2ieuk3r4-Body_Mesh_Rigged\\file2");
	std::string nanotexture("Models");
	Model nanoModel = Importer::loadModel(nanopath, nanotexture);
	nanoModel.setup();
	/*nanoModel.scale(0.005f, 0.005f, 0.005f);
	nanoModel.translate(-200.0f, 0.0f, 0.0f);
	nanoModel.rotateY(90.0f);*/
	nanoModel.scale(0.005f, 0.005f, 0.005f);
	nanoModel.rotateY(90.0f);
	nanoModel.translate(-50.0f, 0.0f, -200.0f);

	std::string fieldPath("C:\\Users\\Alex Hu\\Documents\\Model\\Field\\field.obj");
	std::string fieldTexture("C:\\Users\\Alex Hu\\Documents\\Model\\Field");
	Model fieldModel = Importer::loadModel(fieldPath, fieldTexture);
	fieldModel.setup();
	fieldModel.scale(2.5f, 2.5f, 2.5f);
	fieldModel.translate(0.0f, 0.0f, -1.0f);
	fieldModel.rotateY(180.0f);

	/*
	std::string gpath("C:\\Users\\Alex Hu\\Documents\\Model\\Girl\\girl.obj");
	std::string gtexture("C:\\Users/Alex Hu/Documents/Model/Girl/Texture");
	Model girlModel = Importer::loadModel(gpath, gtexture);
	girlModel.setup();
	girlModel.scale(0.2f, 0.2f, 0.2f);
	girlModel.translate(5.0f, 0.0f, 0.0f);
	girlModel.rotateX(-90.0f);

	std::string path("C:\\Users/Alex Hu/Documents/Model/bob/bob_lamp_update_export.md5mesh");
	std::string texture("C:\\Users/Alex Hu/Documents/Model/bob/");
	Model bobModel = Importer::loadModel(path, texture);
	bobModel.setup();
	bobModel.scale(0.2f, 0.2f, 0.2f);
	bobModel.rotateX(-90.0f);
	*/

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
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	Camera camera(cameraLocation, glm::vec3(0.0f, 0.0f, -1.0f), cameraUp);
	// translate upwards a little bit so that the scene is seen
	camera.translateUp(1.0f);
	camera.translateForward(-2.0f);

	glm::mat4 view = camera.getViewMatrix();
	GLint uniView = glGetUniformLocation(shaderProgramId, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj = glm::perspective(
		45.0f, // vertical field of view
		(float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,	 // aspect ratio of screen 
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
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//--------------------------------------------------------------------------------------------------------

	MyAnimatedMeshClass myModel;
	myModel.setAnimationIndex(MoveSet::IDLE);

	/*Scene bobScene;
	bobScene.addModel(&bobModel);
	//bobScene.addModel(&nanoModel);

	Scene girlScene;
	girlScene.addModel(&girlModel);
	//girlScene.addModel(&nanoModel);
	*/
	nanoModel.printAnimationData();

	bool displayBobScene = true;
	bool displayGirlScene = true;

	unsigned int prevTime = SDL_GetTicks();
	unsigned int fpsArray[] = { 16, 17, 17 };
	unsigned int fpsCount = 0;
	float fpsAverage = 0;
	unsigned int startTime = prevTime;

	// For animation test
	int stateDuration = 0;
	int actionDuration = 0;
	unsigned int controllerInputs = 0;

	//CharacterState fighterState = CharacterState();
	CharacterStateManager fighterStateManager((CharacterData*)(&myModel));

	unsigned int currentFrame = 0;
	float timeInMs = 0.0;

	int controllerDirection = 0;

	unsigned int frameCount = 0;
	bool rectReverse = true;
	float rectWidth = 10.0f;
	std::string inputCounter;
	inputCounter = "Input counts: ";
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
				inputCounter += ":) ";
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_RIGHT))
			{
				camera.translateRight(0.25);
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_UP))
			{
				camera.translateUp(0.25);
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_DOWN))
			{
				camera.translateUp(-0.25);
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_z))
			{
				camera.translateForward(0.25);
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_x))
			{
				camera.translateForward(-0.25);
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_w))
			{
				camera.rotateUp(6.0f);
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_s))
			{
				camera.rotateUp(-6.0f);
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_a))
			{
				camera.rotateRight(3.0f);
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_d))
			{
				camera.rotateRight(-3.0f);
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_1))
			{
				displayBobScene = !displayBobScene;
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_2))
			{
				displayGirlScene = !displayGirlScene;
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_3))
			{
				if (Mix_PlayingMusic() == 0)
				{
					//Play the music
					Mix_PlayMusic(gMusic, -1);
				}
				//If music is being played
				else
				{
					//If the music is paused
					if (Mix_PausedMusic() == 1)
					{
						//Resume the music
						Mix_ResumeMusic();
					}
					//If the music is playing
					else
					{
						//Pause the music
						Mix_PauseMusic();
					}
				}
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_u))
			{
				// jump
				controllerInputs |= GameInput::INPUT_UP;
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_k))
			{
				// walk forward
				controllerInputs |= GameInput::INPUT_RIGHT;
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_h))
			{
				controllerInputs |= GameInput::INPUT_LEFT;
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_j))
			{
				controllerInputs |= GameInput::INPUT_DOWN;
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_i))
			{
				// punch1
				controllerInputs |= GameInput::INPUT_PUNCH1;
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_o))
			{
				// punch2
				controllerInputs |= GameInput::INPUT_PUNCH2;
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_p))
			{
				// kick1
				//currentFrame = 2 + controllerDirection;
				//myModel.setAnimationIndex(currentFrame);
				//actionDuration = myModel.getTotalFrames(currentFrame);
				controllerInputs |= GameInput::INPUT_KICK1;
			}

			if ((windowEvent.type == SDL_KEYDOWN) && (windowEvent.key.keysym.sym == SDLK_l))
			{
				// kick2
				controllerInputs |= GameInput::INPUT_KICK2;
				/*
				CharacterState player1State = fighterStateManager.getState();
				Action player1Action = player1State.getAction();
				HorizontalDirection player1HD = player1State.getHorizontalDirection();
				VerticalDirection player1VD = player1State.getVerticalDirection();
				unsigned int player1Time = player1State.getStateTimer();

				std::cout << "Action: " << (int) player1Action << std::endl;
				std::cout << "HD: " << (int)player1HD << std::endl;
				std::cout << "VD: " << (int)player1VD << std::endl;
				std::cout << "Time: " << (int)player1Time << std::endl;
				*/
			}

			if ((windowEvent.type == SDL_KEYUP) && (windowEvent.key.keysym.sym == SDLK_u))
			{
				// jump
				controllerInputs &= ~GameInput::INPUT_UP;
			}

			if ((windowEvent.type == SDL_KEYUP) && (windowEvent.key.keysym.sym == SDLK_k))
			{
				// walk forward
				controllerInputs &= ~GameInput::INPUT_RIGHT;
			}

			if ((windowEvent.type == SDL_KEYUP) && (windowEvent.key.keysym.sym == SDLK_h))
			{
				controllerInputs &= ~GameInput::INPUT_LEFT;
			}

			if ((windowEvent.type == SDL_KEYUP) && (windowEvent.key.keysym.sym == SDLK_j))
			{
				controllerInputs &= ~GameInput::INPUT_DOWN;
			}

			if ((windowEvent.type == SDL_KEYUP) && (windowEvent.key.keysym.sym == SDLK_i))
			{
				// punch1
				controllerInputs &= ~GameInput::INPUT_PUNCH1;
			}

			if ((windowEvent.type == SDL_KEYUP) && (windowEvent.key.keysym.sym == SDLK_o))
			{
				// punch2
				controllerInputs &= ~GameInput::INPUT_PUNCH2;
			}

			if ((windowEvent.type == SDL_KEYUP) && (windowEvent.key.keysym.sym == SDLK_p))
			{
				// kick1
				controllerInputs &= ~GameInput::INPUT_KICK1;
			}

			if ((windowEvent.type == SDL_KEYUP) && (windowEvent.key.keysym.sym == SDLK_l))
			{
				// kick2
				controllerInputs &= ~GameInput::INPUT_KICK2;
			}

			if ((windowEvent.type == SDL_JOYDEVICEADDED))
			{
				gGameController = SDL_JoystickOpen(0);
				if (gGameController == NULL)
				{
					printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
				}
				printf("JOY DEVICE ADDED\n");
			}

			if ((windowEvent.type == SDL_JOYDEVICEREMOVED))
			{
				printf("JOY DEVICE REMOVED\n");
			}

			/*if ((windowEvent.type == SDL_JOYHATMOTION))
			{
			//printf("JOY HAT MOTION!!!!\n");
			if (windowEvent.jhat.value & SDL_HAT_UP)
			{
			currentFrame = 15;
			controllerDirection = 8;
			myModel.setAnimationIndex(currentFrame);
			}

			if (windowEvent.jhat.value & SDL_HAT_DOWN)
			{
			currentFrame = 16;
			controllerDirection = 4;
			myModel.setAnimationIndex(currentFrame);
			}

			if (windowEvent.jhat.value & SDL_HAT_RIGHT)
			{
			controllerDirection = 0;
			currentFrame = 13;
			myModel.setAnimationIndex(currentFrame);
			}

			if (windowEvent.jhat.value & SDL_HAT_LEFT)
			{
			controllerDirection = 0;
			currentFrame = 14;
			myModel.setAnimationIndex(currentFrame);
			}
			}*/

			/*if (windowEvent.type == SDL_JOYBUTTONDOWN)
			{
			switch (windowEvent.jbutton.button)
			{
			case SDL_CONTROLLER_BUTTON_A :
			currentFrame = 0 + controllerDirection;
			myModel.setAnimationIndex(currentFrame);
			break;
			case SDL_CONTROLLER_BUTTON_B:
			currentFrame = 1 + controllerDirection;
			myModel.setAnimationIndex(currentFrame);
			break;
			case SDL_CONTROLLER_BUTTON_X:
			currentFrame = 2 + controllerDirection;
			myModel.setAnimationIndex(currentFrame);
			break;
			case SDL_CONTROLLER_BUTTON_Y:
			currentFrame = 3 + controllerDirection;
			myModel.setAnimationIndex(currentFrame);
			break;
			default:
			break;
			}
			}*/

			glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
		}

		// update character moves
		fighterStateManager.setGameInputs(controllerInputs);
		fighterStateManager.update();
		CharacterState player1State = fighterStateManager.getState();
		Action player1Action = player1State.getAction();
		HorizontalDirection player1HD = player1State.getHorizontalDirection();
		VerticalDirection player1VD = player1State.getVerticalDirection();
		unsigned int player1Time = player1State.getStateTimer();

		if (player1State.getHorizontalDirection() == HorizontalDirection::HDIRECTION_FORWARD && player1State.getVerticalDirection() == VerticalDirection::VDIRECTION_STAND  && player1State.getAction() == Action::ACTION_NONE)
		{
			nanoModel.translate(0.0f, 0.0f, 2.0f);
		}
		if (player1State.getHorizontalDirection() == HorizontalDirection::HDIRECTION_BACKWARD && player1State.getVerticalDirection() == VerticalDirection::VDIRECTION_STAND && player1State.getAction() == Action::ACTION_NONE)
		{
			nanoModel.translate(0.0f, 0.0f, -2.0f);
		}
		if (player1State.getHorizontalDirection() == HorizontalDirection::HDIRECTION_FORWARD && player1State.getVerticalDirection() == VerticalDirection::VDIRECTION_JUMP)
		{
			unsigned int jumpTime = fighterStateManager.getJumpDuration();
			unsigned int jumpStartup = myModel.getTotalStartupFrames(MoveSet::JUMP);
			unsigned int jumpActive = myModel.getTotalActiveFrames(MoveSet::JUMP);
			if (jumpTime > jumpStartup && jumpTime <  jumpStartup + jumpActive)
			{
				float yinc = 2.0f - 2.0f * (float)(jumpTime-3) / 26.5f;
				nanoModel.translate(0.0f, 16.0f * yinc, 6.0f);
			}
		}
		if (player1State.getHorizontalDirection() == HorizontalDirection::HDIRECTION_BACKWARD && player1State.getVerticalDirection() == VerticalDirection::VDIRECTION_JUMP)
		{
			unsigned int jumpTime = fighterStateManager.getJumpDuration();
			unsigned int jumpStartup = myModel.getTotalStartupFrames(MoveSet::JUMP);
			unsigned int jumpActive = myModel.getTotalActiveFrames(MoveSet::JUMP);
			if (jumpTime > jumpStartup && jumpTime <  jumpStartup + jumpActive)
			{
				float yinc = 2.0f - 2.0f * (float)(jumpTime - 3) / 26.5f;
				nanoModel.translate(0.0f, 16.0f * yinc, -6.0f);
			}
		}
		if (player1State.getHorizontalDirection() == HorizontalDirection::HDIRECTION_NEUTRAL && player1State.getVerticalDirection() == VerticalDirection::VDIRECTION_JUMP)
		{
			unsigned int jumpTime = fighterStateManager.getJumpDuration();
			unsigned int jumpStartup = myModel.getTotalStartupFrames(MoveSet::JUMP);
			unsigned int jumpActive = myModel.getTotalActiveFrames(MoveSet::JUMP);
			float halfActive = (float)jumpActive / 2.0f;
			if (jumpTime > jumpStartup && jumpTime <  jumpStartup + jumpActive)
			{
				float yinc = 2.0f - 2.0f * (float)(jumpTime - jumpStartup) / halfActive;
				nanoModel.translate(0.0f, 16.0f * yinc, 0.0f);
			}
		}

		if (player1Time == 1)
		{
			controllerDirection = player1VD * 4;

			if (player1Action == Action::ACTION_PUNCH1 ||
				player1Action == Action::ACTION_PUNCH2 ||
				player1Action == Action::ACTION_KICK1 ||
				player1Action == Action::ACTION_KICK2)
			{
				currentFrame = player1Action - 4 + controllerDirection;
				myModel.setAnimationIndex(currentFrame);
			}
			if (player1HD == HorizontalDirection::HDIRECTION_FORWARD && player1VD == VerticalDirection::VDIRECTION_STAND && player1Action == Action::ACTION_NONE)
			{
				myModel.setAnimationIndex(MoveSet::WALK_FORWARD);
			}
			if (player1HD == HorizontalDirection::HDIRECTION_BACKWARD && player1VD == VerticalDirection::VDIRECTION_STAND && player1Action == Action::ACTION_NONE)
			{
				myModel.setAnimationIndex(MoveSet::WALK_BACKWARD);
			}
			if (player1VD == VerticalDirection::VDIRECTION_JUMP && player1Action == Action::ACTION_NONE)
			{
				myModel.setAnimationIndex(MoveSet::JUMP);
			}
			if (player1VD == VerticalDirection::VDIRECTION_CROUCH && player1Action == Action::ACTION_NONE)
			{
				myModel.setAnimationIndex(MoveSet::CROUCH);
			}
			if (player1VD == VerticalDirection::VDIRECTION_STAND && player1HD == HorizontalDirection::HDIRECTION_NEUTRAL && player1Action == Action::ACTION_NONE)
			{
				myModel.setAnimationIndex(MoveSet::IDLE);
			}
		}

		//controllerInputs = GameInput::INPUT_NONE;
		fighterStateManager.setGameInputs(GameInput::INPUT_NONE);

		// Clear the screen to black
		glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// render text
		RenderText(textShaderProgram, inputCounter, 300.0f, 340.0f, 0.5f, glm::vec3(0.5, 0.8f, 0.2f), vao, vbo);
		RenderText(textShaderProgram, "Player 2 Text", 600.0f, 25.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f), vao, vbo);
		RenderText(textShaderProgram, "Player 1 TextOH JUST TESTING IF LONG TEXT IS BAD IDK LOL", 25.0f, 25.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f), vao, vbo);

		shaderProgram.use();

		unsigned int runningTime = SDL_GetTicks() - startTime;
		timeInMs = 16.66667f;
		std::vector<glm::mat4> boneTransforms = nanoModel.getBoneTransforms(myModel.getAnimationTime((float)timeInMs), 0);

		for (unsigned int i = 0; i < boneTransforms.size(); ++i)
		{
			if (i < 100)
			{
				setBoneTransforms(shaderProgramId, i, boneTransforms[i]);
			}
		}

		nanoModel.draw(sampler, uniModel, glm::mat4(1.0f));

		boneTransforms = fieldModel.getBoneTransforms(0, 0);

		for (unsigned int i = 0; i < boneTransforms.size(); ++i)
		{
			if (i < 100)
			{
				setBoneTransforms(shaderProgramId, i, boneTransforms[i]);
			}
		}

		fieldModel.draw(sampler, uniModel, glm::mat4(1.0f));

		/*if (displayBobScene)
		{
		bobScene.render(shaderProgramId, sampler, uniModel, setBoneTransforms, runningTime);
		}
		if (displayGirlScene)
		{
		girlScene.render(shaderProgramId, sampler, uniModel, setBoneTransforms, runningTime);
		}*/

		shapeShaderProgram.use();
		glUniformMatrix4fv(uniViewShape, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
		/*glUniform4f(shapeUniColor, 1.0f, 0.0f, 0.0f, 1.0f);
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);*/

		//if (frameCount >= 15)
		//{
		//	sampleRect.setWidth(rectWidth);
		//	if (rectReverse)
		//	{
		//		rectWidth -= 1.0f;
		//	}
		//	else
		//	{
		//		rectWidth += 1.0f;
		//	}
		//	if (rectWidth <= 0.0f)
		//	{
		//		rectReverse = false;
		//	}
		//	if (rectWidth >= 10.0f)
		//	{
		//		rectReverse = true;
		//	}
		//	frameCount = 0;
		//}
		//++frameCount;
		//sampleRect.draw(shapeUniColor);

		shaderProgram.use();

		unsigned int currTime = SDL_GetTicks();
		unsigned int elapsed = currTime - prevTime;

		while (elapsed < fpsArray[(fpsCount % 60) / 20])
		{
			currTime = SDL_GetTicks();
			elapsed = currTime - prevTime;
		}

		prevTime = currTime;

		fpsAverage = fpsAverage + (1000.0f / (float)elapsed);

		std::string fpsTemp = std::to_string(fpsAverage / (float)(fpsCount + 1));
		const char *fpsString = fpsTemp.c_str();
		SDL_SetWindowTitle(window, fpsString);

		++fpsCount;

		SDL_GL_SwapWindow(window);
	}

	for (unsigned int i = 0; i < SDL_NumJoysticks(); i++)
	{
		SDL_JoystickClose(joysticks[i]);
	}

	shaderProgram.cleanup();
	shapeShaderProgram.cleanup();
	textShaderProgram.cleanup();

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	sampleRect.cleanup();

	//bobModel.clearGLBuffers();
	//girlModel.clearGLBuffers();
	nanoModel.clearGLBuffers();

	SDL_GL_DeleteContext(context);

	SDL_Quit();
	return 0;
}



void RenderText(ShaderProgram &shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, GLuint vao, GLuint vbo)
{
	// Activate corresponding render state	
	shader.use();
	glUniform3f(glGetUniformLocation(shader.getProgram(), "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
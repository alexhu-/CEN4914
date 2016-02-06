#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(std::string vertexShaderFile, std::string fragmentShaderFile) :
	mVertexShaderFile(vertexShaderFile),
	mFragmentShaderFile(fragmentShaderFile)
{

}

void ShaderProgram::compileShaderProgram()
{
	// create ids for vertex and fragment shaders and compile them
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	compileShader(mVertexShaderFile, "vertex", vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	compileShader(mFragmentShaderFile, "fragment", fragmentShader);

	// attach the shaders to a program
	mShaderProgram = glCreateProgram();
	glAttachShader(mShaderProgram, vertexShader);
	glAttachShader(mShaderProgram, fragmentShader);

	glLinkProgram(mShaderProgram);

	// clean up
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}

void ShaderProgram::compileShader(std::string fileName, std::string type, GLuint shader)
{
	// create temporary string to hold file data
	std::string fileString;

	// open the file
	std::ifstream file(fileName);

	// check if the file is open
	if (!file.is_open())
	{
		std::cout << "Cannot open file" << std::endl;
		return;
	}

	//  make sure ifstream can throw exception
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// read the file into a stringstream
		std::stringstream fileStream;
		fileStream << file.rdbuf();
		file.close();
		fileString = fileStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Error: failed to read " << type << " shader file: " << e.what() << std::endl;
	}

	// get the string as a c string so that openGL can receive it
	const GLchar* cstring = fileString.c_str();

	glShaderSource(shader, 1, &cstring, NULL);
	glCompileShader(shader);

	// Check if compilation was successful
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_TRUE)
	{
		std::cout << type << " shader compiled successfully" << std::endl;
	}
	else
	{
		char buffer[512];
		glGetShaderInfoLog(shader, 512, NULL, buffer);
		std::cout << type << " shader failed to compile" << std::endl;
		std::cout << "Compilation errors: " << buffer << std::endl;
	}
}

void ShaderProgram::use()
{
	glUseProgram(mShaderProgram);
}

GLuint ShaderProgram::getProgram()
{
	return mShaderProgram;
}

void ShaderProgram::cleanup()
{
	glDeleteProgram(mShaderProgram);
}
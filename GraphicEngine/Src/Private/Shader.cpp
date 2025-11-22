#include "Shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	//Create readers for the Vertex and fragment shaders paths, read content and create source string
	std::string vertexShaderSourceString;
	std::string fragmentShaderSourceString;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vShaderFile.open(vertexShaderPath);
		fShaderFile.open(fragmentShaderPath);

		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vertexShaderSourceString = vShaderStream.str();
		fragmentShaderSourceString = fShaderStream.str();

		vShaderFile.close();
		fShaderFile.close();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Failed to read shader file: " << e.what() << std::endl;
	}
	const char* vShaderSource = vertexShaderSourceString.c_str();
	const char* fShaderSource = fragmentShaderSourceString.c_str();


}

void Shader::Use()
{
	glUseProgram(shaderProgramID);
}

void Shader::SetFloat(const std::string& paramName, float value)
{
	glUniform1f(glGetUniformLocation(shaderProgramID, paramName.c_str()), value);
}

void Shader::SetInt(const std::string& paramName, int value)
{
	glUniform1i(glGetUniformLocation(shaderProgramID, paramName.c_str()), value);
}

void Shader::SetBool(const std::string& paramName, bool value)
{
	glUniform1i(glGetUniformLocation(shaderProgramID, paramName.c_str()), value);
}
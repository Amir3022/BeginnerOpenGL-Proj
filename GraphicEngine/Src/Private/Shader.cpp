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

	//variables to check for compile, link success
	int state;
	char infoLog[512];

	//Create Vertex Shader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderSource, nullptr);
	glCompileShader(vertexShader);
	//Check if vertex shader is successfully compiled
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &state);
	if (!state)
	{
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cout << "Vertex Shader failed to Compile: " << infoLog << std::endl;
		throw std::exception();
	}

	//Create Fragment Shader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fShaderSource, nullptr);
	glCompileShader(fragmentShader);
	//Check if fragment shader is successfully compiled
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &state);
	if (!state)
	{
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cout << "Fragment Shader failed to Compile: " << infoLog << std::endl;
		throw std::exception();
	}
	
	//Create Shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);
	//Check if Shader Program is successfully linked
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &state);
	if (!state)
	{
		glGetProgramInfoLog(shaderProgramID, 512, nullptr, infoLog);
		std::cout << "Shader Program failed to Link: " << infoLog << std::endl;
		throw std::exception();
	}

	//Destroy Vertex and Fragment Shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

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

void Shader::SetColor(const std::string& paramName, float red, float green, float blue, float alpha)
{
	glUniform4f(glGetUniformLocation(shaderProgramID, paramName.c_str()), red, green, blue, alpha);
}

void Shader::SetVec3(const std::string& paramName, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(shaderProgramID, paramName.c_str()), x, y, z);
}

void Shader::SetVec3(const std::string& paramName, const glm::vec3& vec)
{
	SetVec3(paramName, vec.x, vec.y, vec.z);
}

void Shader::SetVec4(const std::string& paramName, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(shaderProgramID, paramName.c_str()), x, y, z, w);
}

void Shader::SetVec4(const std::string& paramName, const glm::vec4 vec)
{
	SetVec4(paramName, vec.x, vec.y, vec.z, vec.w);
}

void Shader::SetMat44(const std::string& paramName, const glm::mat4& mat)
{
	glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, paramName.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetMat33(const std::string& paramName, const glm::mat3& mat)
{
	glUniformMatrix3fv(glGetUniformLocation(shaderProgramID, paramName.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
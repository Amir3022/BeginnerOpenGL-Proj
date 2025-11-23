#ifndef SHADER_H
#define SHADER_H

#include "glad\glad.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

class Shader
{
	unsigned int shaderProgramID;

public:
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);

	void Use();
	void SetFloat(const std::string& paramName, float value);
	void SetInt(const std::string& paramName, int value);
	void SetBool(const std::string& paramName, bool value);
	void SetColor(const std::string& paramName, float red, float green, float blue, float alpha);
};
#endif

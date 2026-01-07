#ifndef SHADER_H
#define SHADER_H

#include "glad\glad.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

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
	void SetVec3(const std::string& paramName, float x, float y, float z);
	void SetVec3(const std::string& paramName, const glm::vec3& vec);
	void SetVec4(const std::string& paramName, float x, float y, float z, float w);
	void SetVec4(const std::string& paramName, const glm::vec4 vec);
	void SetMat44(const std::string& paramName, const glm::mat4& mat);
	void SetMat33(const std::string& paramName, const glm::mat3& mat);
};
#endif

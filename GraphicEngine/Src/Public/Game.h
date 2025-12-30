#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>
#include "Shader.h"
#include "Camera.h"
#include "stb_image.h"

struct PosOrientPair
{
	glm::vec3 pos;
	glm::vec3 orient;
};

class Game
{
public:
	Game(int in_width, int in_height)
		:width(in_width), height(in_height)
	{
		//Initialize Class variables
		fragmentShaderPath = "Shaders/FragmentShader.glsl";
		vertexShaderPath = "Shaders/VertexShader.glsl";

		bDrawingInWireframe = false;
		texturesMixAlpha = 0.5f;

		objectXRotation = 0.0f;
		objectYRotation = 0.0f;

		lastCursorPos = glm::zero<glm::vec2>();
		bFirstCursorUpdate = true;

		deltaTime = 0.0f;
		currentFrameTime = 0.0f;
		lastFrameTime = 0.0f;

		camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(-90.0f, 0.0f, 0.0f));

		currentWindow = nullptr;
		VAO = -1;
		cubeTransforms.clear();
	}

	//Main Entry function, initializes the game and runs the Update Game and draw logic
	int Process();

	bool Init();

	void UpdateGame(float deltaTime);

	void DrawFrame();

public:
	float GetDeltaTime() { return deltaTime; }
	Camera* GetCamera() { return camera.get(); }
	Shader* GetShader() { return shader.get(); }
	bool IsCursorFirstUpdate() { return bFirstCursorUpdate; }
	void SetCursorFirstUpdate(bool bValue) { bFirstCursorUpdate = bValue; }
	glm::vec2 GetLastCursorPos() { return lastCursorPos; }
	void SetLastCursorPos(const glm::vec2& in_val) { lastCursorPos = in_val; }

//Internal Functions
public:
	static void framebuffer_resize_callback(GLFWwindow* targetWindow, int newWidth, int newHeight);
	static void mouseCursor_move_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouseScroll_change_callback(GLFWwindow* window, double xoffset, double yoffset);

private:
	void processInput(GLFWwindow* window);
	unsigned int LoadImageIntoTexture(const char* imagePath, GLenum textureUnit, GLenum dataFormat);
	void calculateDeltaTime();

private:
	//Window Size Variables
	GLFWwindow* currentWindow;
	int width = 800;
	int height = 600;

	//Shader Variables
	std::string fragmentShaderPath;
	std::string vertexShaderPath;
	
	//Rendering mode and texture variables
	bool bDrawingInWireframe;
	float texturesMixAlpha;

	//Cached Current cube rotation
	float objectXRotation;
	float objectYRotation;

	//Cursor movement variables
	glm::vec2 lastCursorPos;
	bool bFirstCursorUpdate;

	//Time variables
	float deltaTime;
	float currentFrameTime;
	float lastFrameTime;

	//Camera Instance
	std::unique_ptr<Camera> camera;

	//Shader Program ID Instance
	std::unique_ptr<Shader> shader;

	//Game Instance Specific variables
	std::vector<PosOrientPair> cubeTransforms;

	/// GL Array Variables ///
	unsigned int VAO;
};
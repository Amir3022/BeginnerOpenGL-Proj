#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>
#include "Components/Shader.h"
#include "Components/Camera.h"
#include "Utilities/stb_image.h"
#include "assimp/config.h"
#include "assimp/revision.h"

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
		fragmentShaderPath = "";
		vertexShaderPath = "";

		lastCursorPos = glm::zero<glm::vec2>();
		bFirstCursorUpdate = true;

		deltaTime = 0.0f;
		currentFrameTime = 0.0f;
		lastFrameTime = 0.0f;

		bDrawingInWireframe = false;

		camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(-90.0f, 0.0f, 0.0f));

		currentWindow = nullptr;
	}

	//Main Entry function, initializes the game and runs the Update Game and draw logic
	int Process();

	virtual bool Init();

	virtual void Terminate();

	virtual void UpdateGame(float deltaTime);

	virtual void DrawFrame();

public:
	float GetDeltaTime() { return deltaTime; }
	Camera* GetCamera() { return camera.get(); }
	Shader* GetShader() { return shader.get(); }
	bool IsCursorFirstUpdate() { return bFirstCursorUpdate; }
	void SetCursorFirstUpdate(bool bValue) { bFirstCursorUpdate = bValue; }
	glm::vec2 GetLastCursorPos() { return lastCursorPos; }
	void SetLastCursorPos(const glm::vec2& in_val) { lastCursorPos = in_val; }
	int GetWidth() { return width; }
	int GetHeight() { return height; }

//Internal Functions
public:
	static void framebuffer_resize_callback(GLFWwindow* targetWindow, int newWidth, int newHeight);
	static void mouseCursor_move_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouseScroll_change_callback(GLFWwindow* window, double xoffset, double yoffset);

protected:
	virtual void ProcessInput(GLFWwindow* window);
	unsigned int LoadImageIntoTexture(const char* imagePath);
	void calculateDeltaTime();

private:
	//Window Size Variables
	GLFWwindow* currentWindow;
	int width = 800;
	int height = 600;
	
	//Cursor movement variables
	glm::vec2 lastCursorPos;
	bool bFirstCursorUpdate;

	//Time variables
	float deltaTime;
	float currentFrameTime;
	float lastFrameTime;

	//Render Drawing Variables
	bool bDrawingInWireframe;

protected:
	//Camera Instance
	std::unique_ptr<Camera> camera;

	//Shader Program ID Instance
	std::unique_ptr<Shader> shader;

	//Shader Variables
	std::string fragmentShaderPath;
	std::string vertexShaderPath;
};
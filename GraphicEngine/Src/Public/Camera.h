#pragma once
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

class Camera
{
public:
	//Class initialization
	Camera()
	{
		position = glm::vec3(0.0f);
		rotation = glm::vec3(0.0f);
		upDir = glm::vec3(0.0, 1.0f, 0.0f);
		FOV = 45.f;
		FOVChangeRate = 5.0f;
		camMoveSpeed = 2.5;
		camSensitivity = 0.1f;
		bUseInternalLookAt = false;
	}

	Camera(glm::vec3 startPos, glm::vec3 startRot)
		:position(startPos), rotation(startRot)
	{
		upDir = glm::vec3(0.0, 1.0f, 0.0f);
		FOV = 45.f;
		FOVChangeRate = 5.0f;
		camMoveSpeed = 2.5;
		camSensitivity = 0.1f;
		bUseInternalLookAt = false;
	}

	Camera(glm::vec3 startPos, glm::vec3 upDir, glm::vec3 startRot, float startFOV, float FOVChangeRate, float camSpeed, float camSensitivity, bool bUseInternalLookAt)
		:
		position(startPos), upDir(upDir), rotation(startRot), 
		FOV(startFOV), FOVChangeRate(FOVChangeRate),
		camMoveSpeed(camSpeed), camSensitivity(camSensitivity),
		bUseInternalLookAt(bUseInternalLookAt)
	{

	}

	//Getter functions
	glm::vec3 GetCameraLocation() { return position; }
	glm::vec3 GetCameraRotation() { return rotation; }
	float GetCameraFOV() { return FOV; }
	glm::vec3 GetCameraForwardDir();
	glm::vec3 GetCameraRightDir();
	glm::vec3 GetCameraUpDir();
	glm::mat4 GetLookAtMat(glm::vec3 targetPos);

	//Input Handling
	void ProcessKeyboardInput(glm::vec2 inputVec, float deltaTime);
	void ProcessKeyboardHeightInput(float inputVal, float deltaTime);
	void ProcessMouseMovementInput(glm::vec2 deltaMove);
	void ProcessMouseScrollInput(float yValue);

private:
	glm::mat4 internal_lookAt(glm::vec3 targetPos);
private:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 upDir;
	float FOV;
	float FOVChangeRate;
	float camMoveSpeed;
	float camSensitivity;
	bool bUseInternalLookAt;
};

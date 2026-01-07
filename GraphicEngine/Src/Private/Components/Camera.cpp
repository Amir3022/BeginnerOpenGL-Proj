#include "Components/Camera.h"

glm::vec3 Camera::GetCameraForwardDir()
{
	return glm::normalize(glm::vec3(glm::cos(glm::radians(rotation.x)) * glm::cos(glm::radians(rotation.y)), glm::sin(glm::radians(rotation.y)), glm::sin(glm::radians(rotation.x)) * glm::cos(glm::radians(rotation.y))));
}

glm::vec3 Camera::GetCameraRightDir()
{
	return glm::cross(GetCameraForwardDir(), upDir);
}

glm::vec3 Camera::GetCameraUpDir()
{
	return glm::cross(GetCameraRightDir(), GetCameraForwardDir());
}

glm::mat4 Camera::GetLookAtMat(glm::vec3 targetPos)
{
	if (bUseInternalLookAt)
	{
		return internal_lookAt(targetPos);
	}
	return glm::lookAt(position, position + GetCameraForwardDir(), upDir);
}

void Camera::ProcessKeyboardInput(glm::vec2 inputVec, float deltaTime)
{
    glm::vec3 moveForwardDir = GetCameraForwardDir();
    glm::vec3 moveRightDir = GetCameraRightDir();

    glm::vec3 moveDir = glm::normalize(moveForwardDir) * inputVec.y + glm::normalize(moveRightDir) * inputVec.x;

    position += moveDir * camMoveSpeed * deltaTime;
}

void Camera::ProcessKeyboardHeightInput(float inputVal, float deltaTime)
{
    glm::vec3 localUpVector = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 moveDir = localUpVector * inputVal;

    position += moveDir * camMoveSpeed * deltaTime;
}

void Camera::ProcessMouseMovementInput(glm::vec2 deltaMove)
{
    deltaMove.y = -1 * deltaMove.y;
    deltaMove *= camSensitivity; //Add mouse sensitivity to mouse location difference
    rotation.x += deltaMove.x;
    rotation.y += deltaMove.y;
    if (rotation.y > 89.9f)
        rotation.y = 89.9f;
    else if (rotation.y < -89.9)
        rotation.y = -89.9f;
}

void Camera::ProcessMouseScrollInput(float yValue)
{
    FOV += yValue * FOVChangeRate;
    if (FOV > 120.0f)
        FOV = 120.0f;
    else if (FOV < 20.0f)
        FOV = 20.0f;
}

glm::mat4 Camera::internal_lookAt(glm::vec3 targetPos)
{
    glm::vec3 forwardDir = glm::normalize(position - targetPos);
    glm::vec3 rightDir = glm::normalize(glm::cross(upDir, forwardDir));
    glm::vec3 upActualDir = glm::normalize(glm::cross(forwardDir, rightDir));
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation[0][0] = rightDir.x;
    rotation[1][0] = rightDir.y;
    rotation[2][0] = rightDir.z;
    rotation[0][1] = upActualDir.x;
    rotation[1][1] = upActualDir.y;
    rotation[2][1] = upActualDir.z;
    rotation[0][2] = forwardDir.x;
    rotation[1][2] = forwardDir.y;
    rotation[2][2] = forwardDir.z;

    glm::mat4 translation = glm::mat4(1.0f);
    translation[3][0] = -position.x;
    translation[3][1] = -position.y;
    translation[3][2] = -position.z;

    return rotation * translation;
}
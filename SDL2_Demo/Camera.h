#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f));
	~Camera();
	void rotateUp(float degrees);
	void rotateRight(float degrees);
	void translateUp(float distance);
	void translateRight(float distance);
	void translateForward(float distance);
	glm::mat4 getViewMatrix();
	glm::vec3 getPosition();

private:
	// The position of the camera
	glm::vec3 mPosition;
	// The unit vector direction the camera is facing
	glm::vec3 mDirection;
	// The upwards direction of the camera
	glm::vec3 mUp;
	// The right direction of the camera
	glm::vec3 mRight;
};
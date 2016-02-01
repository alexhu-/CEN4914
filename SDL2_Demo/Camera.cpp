#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 direction, glm::vec3 up) :
	mPosition(position),
	mDirection(direction),
	mUp(up),
	mRight(glm::cross(direction, up))
{

}

Camera::~Camera()
{

}

void Camera::rotateUp(float degrees)
{
	//mDirection = glm::rotateX(mDirection, glm::radians(degrees));
	mUp = glm::rotateX(mUp, glm::radians(degrees));
	mDirection = glm::cross(mUp, mRight);
}

void Camera::rotateRight(float degrees)
{
	mRight = glm::rotateY(mRight, glm::radians(degrees));
	mDirection = glm::cross(mUp, mRight);
}

void Camera::translateUp(float distance)
{
	mPosition = mPosition + (distance * (mUp));
}

void Camera::translateForward(float distance)
{
	mPosition = mPosition + (distance * (mDirection));
}

void Camera::translateRight(float distance)
{
	mPosition = mPosition + (distance * (mRight));
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(mPosition, mPosition + mDirection, mUp);
}
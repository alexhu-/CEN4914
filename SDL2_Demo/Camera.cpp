#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>

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

// try absolute space instead of world space
void Camera::rotateUp(float degrees)
{
	mUp = glm::rotate(mUp, glm::radians(degrees), mRight);
	mDirection = glm::cross(mUp, mRight);
}

void Camera::rotateRight(float degrees)
{
	mRight = glm::rotate(mRight, glm::radians(degrees), mUp);
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

glm::vec3 Camera::getPosition()
{
	return mPosition;
}
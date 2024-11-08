#include "camera.h"
#include <math.h>

glm::mat4x4 Camera::getProjectionMatrix()
{
	if (std::isinf(this->aspectRatio) || std::isnan(this->aspectRatio) || this->aspectRatio == 0)
	{
		return glm::mat4x4(1.f);
	}

	auto mat = glm::perspective(this->fovRadians, this->aspectRatio, this->closePlane,
		this->farPlane);

	return mat;
}


glm::mat4x4 Camera::getViewMatrix()
{
	return  glm::lookAt(position, position + viewDirection, up);
}


//todo better rotate function
void Camera::rotateCamera(const glm::vec2 delta)
{
//	glm::vec3 rotateYaxe = glm::cross(viewDirection, up);
//
//	viewDirection = glm::mat3(glm::rotate(delta.x, up)) * viewDirection;
//
//	if (delta.y < 0)
//	{	//down
//		if (viewDirection.y < -0.99999)
//			goto noMove;
//	}
//	else
//	{	//up
//		if (viewDirection.y > 0.99999)
//			goto noMove;
//	}
//
//	viewDirection = glm::mat3(glm::rotate(delta.y, rotateYaxe)) * viewDirection;
//noMove:
//
//	viewDirection = glm::normalize(viewDirection);

	constexpr float PI = 3.1415926;

	yaw += delta.x;
	pitch += delta.y;

	if (yaw >= 2 * PI)
	{
		yaw -= 2 * PI;
	}
	else if (yaw < 0)
	{
		yaw = 2 * PI - yaw;
	}

	if (pitch > PI/2.f - 0.01) { pitch = PI / 2.f - 0.01; }
	if (pitch < -PI / 2.f + 0.01) { pitch = -PI / 2.f + 0.01; }

	viewDirection = glm::vec3(0,0,-1);

	viewDirection = glm::mat3(glm::rotate(yaw, up)) * viewDirection;

	glm::vec3 rotatePitchAxe = glm::cross(viewDirection, up);
	viewDirection = glm::mat3(glm::rotate(pitch, rotatePitchAxe)) * viewDirection;

}

void Camera::moveFPS(glm::vec3 direction)
{
	viewDirection = glm::normalize(viewDirection);

	//forward
	float forward = -direction.z;
	float leftRight = direction.x;
	float upDown = direction.y;

	glm::vec3 move = {};

	move += up * upDown;
	move += glm::normalize(glm::cross(viewDirection, up)) * leftRight;
	move += viewDirection * forward;

	this->position += move;
}

void Camera::rotateFPS(glm::ivec2 mousePos, float speed, bool shouldMove)
{
	if (shouldMove)
	{
		glm::ivec2 currentMousePos = mousePos;


		glm::vec2 delta = lastMousePos - currentMousePos;
		delta *= speed;

		rotateCamera(delta);

		lastMousePos = currentMousePos;
	}
	else
	{
		lastMousePos = mousePos;
	}
}

int getViewDirectionRotation(glm::vec3 vec)
{
	// If the vector is zero or facing straight up/down, return 0 as default
	if (vec.x == 0.0f && vec.z == 0.0f)
	{
		return 0;
	}

	// Normalize the vector in the xz-plane
	glm::vec2 xzVec(vec.x, vec.z);
	if (glm::length(xzVec) != 0)
	{
		xzVec = glm::normalize(xzVec);
	}
	else
	{
		return 0;
	}

	// Check direction by comparing x and z components
	if (xzVec.y >= 0.0f && std::abs(xzVec.y) > std::abs(xzVec.x))
	{
		return 2;  // Facing +Z
	}
	else if (xzVec.x <= 0.0f && std::abs(xzVec.x) > std::abs(xzVec.y))
	{
		return 1;  // Facing -X
	}
	else if (xzVec.y <= 0.0f && std::abs(xzVec.y) > std::abs(xzVec.x))
	{
		return 0;  // Facing -Z
	}
	else
	{
		return 3;  // Facing +X
	}
}

glm::mat4 lookAtSafe(glm::vec3 const &eye, glm::vec3 const &center, glm::vec3 const &upVec)
{
	glm::vec3 up = glm::normalize(upVec);

	glm::vec3 f;
	glm::vec3 s;
	glm::vec3 u;

	f = (normalize(center - eye));
	if (f == up || f == -up)
	{
		s = glm::vec3(up.z, up.x, up.y);
		u = (cross(s, f));

	}
	else
	{
		s = (normalize(cross(f, up)));
		u = (cross(s, f));
	}

	glm::mat4 Result(1);
	Result[0][0] = s.x;
	Result[1][0] = s.y;
	Result[2][0] = s.z;
	Result[0][1] = u.x;
	Result[1][1] = u.y;
	Result[2][1] = u.z;
	Result[0][2] = -f.x;
	Result[1][2] = -f.y;
	Result[2][2] = -f.z;
	Result[3][0] = -dot(s, eye);
	Result[3][1] = -dot(u, eye);
	Result[3][2] = dot(f, eye);
	return Result;
}








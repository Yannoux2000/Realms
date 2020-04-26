#include "Camera.h"

#include "AspectRatio.h"

using namespace rlms;

std::shared_ptr<Camera> Camera::MainCamera;

Camera::Camera (glm::vec3 pos, float t_yaw, float t_pitch, float zm)
	: position (pos), forward (), yaw (t_yaw), pitch (t_pitch), zoom (zm) {
	update ();
}

Camera::~Camera () {}

glm::mat4 Camera::getMainView () {
	return MainCamera->getView ();
}

glm::mat4 Camera::getMainPerspectivePtr () {
	return MainCamera->getPerspective ();
}

void Camera::CreateMainCamera (glm::vec3 pos, float t_yaw, float t_pitch, float zm) {
	MainCamera = std::make_shared<Camera> (pos, t_yaw, t_pitch, zm);
}

void Camera::setMainCamera () {
	MainCamera = std::shared_ptr<Camera> (this);
}

glm::mat4 Camera::getView () {
	return glm::lookAt (
		position,
		position + forward,
		glm::vec3 (0.0f, 0.0f, 1.0f)
	);
}

glm::mat4 Camera::getPerspective () {
	//std::cout << "Ratio : " << AspectRatio::Ratio () << "( " << AspectRatio::Width () << ", " << AspectRatio::Height () << ")" << std::endl;
	return glm::perspective (glm::radians (zoom), AspectRatio::Ratio (), 0.5f, 2500.0f);
}

uint8_t const Camera::getBFCullingflags () {
	return VoxelMath::BackfacesCulling (forward);
}

void Camera::reset () {
	position = DEF_POSITION;
	yaw = DEF_YAW;
	pitch = DEF_PITCH;
	zoom = DEF_ZOOM;
	update ();
}

void Camera::look (glm::ivec2 delta) {
	float xoffset = delta.x * SENSITIVITY;
	float yoffset = delta.y * SENSITIVITY;

	yaw -= xoffset;
	pitch -= yoffset;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	update ();
}

void Camera::update () {
	glm::vec3 fwd;
	fwd.x = cos (glm::radians (yaw)) * cos (glm::radians (pitch));
	fwd.y = sin (glm::radians (yaw)) * cos (glm::radians (pitch));
	fwd.z = sin (glm::radians (pitch));

	forward = glm::normalize (fwd);
	right = glm::normalize (glm::cross (forward, glm::vec3 (0.0f, 0.0f, 1.0f)));
	up = glm::normalize (glm::cross (right, forward));
}

void Camera::rotate (float rot) {
	position = glm::rotate (glm::mat4 (1), glm::radians (rot), glm::vec3 (0.0f, 0.0f, 1.0f)) * glm::vec4 (position, 1.0);
}

void Camera::rotation (float rot) {
	reset ();
	position = glm::rotate (glm::mat4 (1), glm::radians (rot), glm::vec3 (0.0f, 0.0f, 1.0f)) * glm::vec4 (position, 1.0);
}

void Camera::move (glm::vec3 pos) { //!\\ COORDS GLOBAL
	position = position + pos;
}

void Camera::postion (glm::vec3 pos) {
	reset ();
	position = pos;
}

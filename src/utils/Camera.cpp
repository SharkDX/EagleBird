#include "Camera.h"

EagleBird::Utils::Camera::Camera(GLFWwindow* window)
{
	this->_window = window;
	glfwGetWindowSize(window, &_screen_width, &_screen_height);
    init();
}

EagleBird::Utils::Camera::Camera(const glm::vec3 &position)
{
    init();
    setPosition(position);
}

void EagleBird::Utils::Camera::init()
{
    this->Projection = glm::perspective(60.0f, _screen_width / (float)_screen_height, 0.1f, 20000.0f);
    this->position = glm::vec3(200.0f, 150.0f, 200.0f);
    this->direction = glm::vec3(0.0f, 0.0f, 1.0f);
    this->up = glm::vec3(0.0f, 1.0f, 0.0f);
    horizontalAngle = 0;
    verticalAngle = 0;
}

void EagleBird::Utils::Camera::Update(float elapsed)
{
	double xpos, ypos;
	glfwGetCursorPos(_window, &xpos, &ypos);
	
	int wX, wY;
    glfwGetWindowSize(_window, &wX, &wY);

	glfwSetCursorPos(_window, wX / 2, wY / 2);

	horizontalAngle += (float(wX / 2) - xpos) * 0.002f; 
	verticalAngle += (float(wY / 2) - ypos) * 0.002f;

	if (verticalAngle > 3.14f / 2.0f)
		verticalAngle = 3.14f / 2.0f;
	if (verticalAngle < -3.14f / 2.0f)
		verticalAngle = -3.14f / 2.0f;

    float speed = 0.1f;
    if(glfwGetKey(_window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        speed *= 200;
	if (glfwGetKey(_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
		printf("Position: %g, %g, %g\n", position.x, position.y, position.z);
	if(glfwGetKey(_window, 'W') == GLFW_PRESS)
        moveAhead(speed);
	if(glfwGetKey(_window, 'A') == GLFW_PRESS)
		moveLeft(speed);
	if(glfwGetKey(_window, 'D') == GLFW_PRESS)
		moveRight(speed);
	if(glfwGetKey(_window, 'S') == GLFW_PRESS)
		moveBack(speed);

	calculateVectors();
	this->View = getView();
}

glm::mat4 EagleBird::Utils::Camera::getView()
{
    return glm::lookAt(position, position + direction, up);
}

glm::mat4 EagleBird::Utils::Camera::getMVP()
{
	return this->Projection * this->View;
}

void EagleBird::Utils::Camera::moveAhead(float speed)
{
    position += direction * speed;
}

void EagleBird::Utils::Camera::moveBack(float speed)
{
    position -= direction * speed;
}

void EagleBird::Utils::Camera::moveLeft(float speed)
{
    position += glm::cross(up, direction) * speed;
}

void EagleBird::Utils::Camera::moveRight(float speed)
{
    position -= glm::cross(up, direction) * speed;
}

void EagleBird::Utils::Camera::calculateVectors()
{
    direction.x = glm::cos(verticalAngle) * glm::sin(horizontalAngle);
    direction.y = glm::sin(verticalAngle);
    direction.z = glm::cos(verticalAngle) * glm::cos(horizontalAngle);
	
    right = glm::vec3(glm::sin(horizontalAngle - 3.14f/2.0f), 0, glm::cos(horizontalAngle - 3.14f/2.0f));
    up = glm::cross(right, direction);
}

glm::vec3 EagleBird::Utils::Camera::getPosition()
{
    return position;
}

void EagleBird::Utils::Camera::setPosition(const glm::vec3 &position)
{
    this->position = position;
}

float EagleBird::Utils::Camera::getHorizontalAngle()
{
    return horizontalAngle;
}

float EagleBird::Utils::Camera::getVerticalAngle()
{
    return verticalAngle;    
}

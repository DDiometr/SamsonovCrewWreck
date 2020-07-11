#pragma once
#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include<stb_image.h>

#include<Camera.h>
#include<Shader.h>
#include<Mesh.h>
#include<Model.h>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const * path);

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
float lastX = 400.0f;
float lastY = 300.0f;
bool firstMouse = true;
float fov = 45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float currentFrame;

float surfVert[]{
		 200.0f,   0.0f, 200.0f,     200.0f, 200.0f,
		 200.0f,  0.0f,-200.0f,     200.0f, 0.0f,
		 -200.0f,  0.0f,-200.0f,      0.0f, 0.0f,
		-200.0f,   0.0f,200.0f,      0.0f, 200.0f
};
int indices[]{
		0, 1, 3, 
		1, 2, 3  
};

glm::vec3 surfColor = glm::vec3(0.13f, 0.54f, 0.13f);

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tu-22", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	unsigned int surfTex = loadTexture("D:/openGLProjects/tu22/grasstex.jpg");

	unsigned int surfVAO, surfEBO, surfVBO;
	glGenBuffers(1, &surfEBO);
	glGenBuffers(1, &surfVBO);
	glGenVertexArrays(1, &surfVAO);

	glBindVertexArray(surfVAO);

	glBindBuffer(GL_ARRAY_BUFFER, surfVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(surfVert), &surfVert, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surfEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	

	//stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);

	Shader planeShaderProgram("D:/openGLProjects/tu22/Shaders/planeVertexShader.glsl", "D:/openGLProjects/tu22/Shaders/planeFragmentShader.glsl");
	Shader surfaceShaderProgram("D:/openGLProjects/tu22/Shaders/surfaceVertexShader.glsl", "D:/openGLProjects/tu22/Shaders/surfaceFragmentShader.glsl");

	Model planeModel("D:/openGLProjects/tu22/tupolev-tu22-blinder/source/untitled.obj");
	
		float radius = 100.0;
		float angle = 0.0;
		glm::vec3 selfRotAxis;
	while (!glfwWindowShouldClose(window)) {

		processInput(window);
		
		glClearColor(0.52f, 0.8f, 0.91f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		planeShaderProgram.use();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		planeShaderProgram.setMat4("projection", projection);
		planeShaderProgram.setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(500.0f, 5.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

		glm::vec3 trans = glm::vec3(radius*cos(glm::radians(angle)),20.0f, radius*sin(glm::radians(angle)));
		model = glm::translate(model, trans);
		selfRotAxis = glm::vec3(0.0f, 1.0f, 0.0f);
		model = glm::rotate(model, glm::radians(90.0f-angle), selfRotAxis);
		angle += 0.1f;
		planeShaderProgram.setMat4("model", model);
		
		planeModel.Draw(planeShaderProgram);

		
		glBindVertexArray(surfVAO);
		surfaceShaderProgram.use();

		surfaceShaderProgram.setMat4("projection", projection);
		surfaceShaderProgram.setMat4("view", view);

		model = glm::mat4(1.0f);

		surfaceShaderProgram.setMat4("model", model);

		surfaceShaderProgram.setVec3("color", surfColor);

		glBindTexture(GL_TEXTURE_2D, surfTex);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);



		glfwSwapBuffers(window);
		glfwPollEvents();
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	}

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov = camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
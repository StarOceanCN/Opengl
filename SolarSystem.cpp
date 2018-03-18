#include<vector>
#include<string>
#include<algorithm>
using namespace std;

#define GLEW_STATIC
#include<GL\glew.h>

#include<GLFW\glfw3.h>

#include"Camera.h"
#include"Shader.h"
#include"Model.h"
#include<SOIL.h>

#include<glm\glm.hpp>
#include<glm\gtc\type_ptr.hpp>
#include<glm\gtc\matrix_transform.hpp>

GLuint WIDTH = 1000, HEIGHT = 700;
bool isFirst = true;
double LastxPos = (double)WIDTH / 2.0f;
double LastyPos = (double)HEIGHT / 2.0f;
GLfloat lastTime = 0.0f;
Camera SolarSystemCamera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat deltaTime;
glm::vec3 LightPos(1.2f, 1.0f, 2.0f);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {//参数顺序不可打乱
	if (key == GLFW_KEY_ESCAPE&&action == GLFW_PRESS)glfwSetWindowShouldClose(window, GL_TRUE);
	if (action == GLFW_PRESS) { keys[key] = true; }
	else if (action == GLFW_RELEASE)keys[key] = false;
}
void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	if (isFirst) {
		LastxPos = xPos;
		LastyPos = yPos;
		isFirst = false;
	}
	GLfloat xoffset = xPos - LastxPos;
	GLfloat yoffset = LastyPos - yPos;
	LastxPos = xPos;
	LastyPos = yPos;
	SolarSystemCamera.ProcessMouseMovement(xoffset, yoffset);
}
void cameraMove() {
	if (keys[GLFW_KEY_W])SolarSystemCamera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])SolarSystemCamera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])SolarSystemCamera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])SolarSystemCamera.ProcessKeyboard(RIGHT, deltaTime);
}
GLuint loadTex(GLchar* path) {
	GLuint TexID;
	glGenTextures(1, &TexID);
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);

	glBindTexture(GL_TEXTURE_2D, TexID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D,0);
	SOIL_free_image_data(image);
	return TexID;
}
GLuint loadBox(vector<const char*> faces) {
	GLuint TexID;
	glGenTextures(1, &TexID);
	int width, height;
	unsigned char* image;
	glBindTexture(GL_TEXTURE_CUBE_MAP, TexID);
	for (GLuint i = 0; i < faces.size(); i++) {
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return TexID;
}
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "SolarSystem", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initizlize GLEW" << std::endl;
		return -1;
	}
	//int width, height;
	//glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLfloat UniverseBox[] = {
		// Positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	GLuint UbVAO, UbVBO;
	glGenVertexArrays(1, &UbVAO);
	glGenBuffers(1, &UbVBO);
	glBindVertexArray(UbVAO);
	glBindBuffer(GL_ARRAY_BUFFER, UbVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UniverseBox), UniverseBox, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glBindVertexArray(0);
	
	vector<const GLchar*> faces;
	faces.push_back("UniverseBox/U_rt.jpg");
	faces.push_back("UniverseBox/U_lf.jpg");
	faces.push_back("UniverseBox/U_up.jpg");
	faces.push_back("UniverseBox/U_dn.jpg");
	faces.push_back("UniverseBox/U_bk.jpg");
	faces.push_back("UniverseBox/U_ft.jpg");
	GLuint UniverseBoxTexture = loadBox(faces);

	Shader UniverseBoxShader;
	UniverseBoxShader.fileShader("UniverseBox/U_Sader.vt","UniverseBox/U_Sader.fg");
	
	Model Earth("SolarAndPlanet/Earth/Earth.obj");
	Shader EarthShader;
	EarthShader.fileShader("SolarAndPlanet/Earth/Earth.vt", "SolarAndPlanet/Earth/Earth.fg");

	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastTime;
		lastTime = currentFrame;

		glfwPollEvents();
		cameraMove();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glm::mat4 Boxview, projection;
		Boxview = SolarSystemCamera.GetViewMatrix();
		projection = glm::perspective(SolarSystemCamera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		glDepthFunc(GL_LEQUAL);
		UniverseBoxShader.Use();
		Boxview = glm::mat4(glm::mat3(SolarSystemCamera.GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(UniverseBoxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(Boxview));
		glUniformMatrix4fv(glGetUniformLocation(UniverseBoxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(UbVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(UniverseBoxShader.Program, "UniverseBoxTexture"),0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, UniverseBoxTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		LightPos.x = glm::sin(glfwGetTime()*0.3f);
		LightPos.y = glm::cos(glfwGetTime()*0.6f);

		EarthShader.Use();  
		glUniform3f(glGetUniformLocation(EarthShader.Program, "light.position"), LightPos.x, LightPos.y, LightPos.z);
		glUniform3f(glGetUniformLocation(EarthShader.Program, "light.ambient"), 0.2f, 0.2f, 0.2f);
		glUniform3f(glGetUniformLocation(EarthShader.Program, "light.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(EarthShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(EarthShader.Program, "material.shininess"), 32.0f);
		glUniform3f(glGetUniformLocation(EarthShader.Program, "viewPos"), SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z);

		glm::mat4 view = SolarSystemCamera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(EarthShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(EarthShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));


		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	
		glUniformMatrix4fv(glGetUniformLocation(EarthShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		Earth.ModelRender(EarthShader);

		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}
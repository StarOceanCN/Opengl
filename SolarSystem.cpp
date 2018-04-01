#include<vector>
#include<string>
#include<algorithm>
using namespace std;

#define GLEW_STATIC
#include<GL\glew.h>

#include<GLFW\glfw3.h>

#include"Camera.h"
#include"Shader.h"
#include<SOIL.h>

#include<glm\glm.hpp>
#include<glm\gtc\type_ptr.hpp>
#include<glm\gtc\matrix_transform.hpp>

#include"Planet.h"

GLuint WIDTH = 1000, HEIGHT = 700;
bool isFirst = true;
double LastxPos = (double)WIDTH / 2.0f;
double LastyPos = (double)HEIGHT / 2.0f;
GLfloat lastTime = 0.0f;
Camera SolarSystemCamera(glm::vec3(0.0f, 0.0f, 30.0f));
bool keys[1024];
GLfloat deltaTime;
glm::vec3 LightPos(50.0f, 30.0f, 40.0f);
//键盘回调函数，参数顺序不可打乱
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE&&action == GLFW_PRESS)glfwSetWindowShouldClose(window, GL_TRUE);
	if (action == GLFW_PRESS) { keys[key] = true; }
	else if (action == GLFW_RELEASE)keys[key] = false;
}
//鼠标回调函数，用于摄像机镜头的旋转
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
//纹理加载函数
/*
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
*/
//天空盒加载函数
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
	glViewport(0, 0, WIDTH, HEIGHT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_PROGRAM_POINT_SIZE);


	GLfloat UniverseBox[] = {
		// Positions          
		-50.0f,  50.0f, -50.0f,
		-50.0f, -50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,
		50.0f,  50.0f, -50.0f,
		-50.0f,  50.0f, -50.0f,

		-50.0f, -50.0f,  50.0f,
		-50.0f, -50.0f, -50.0f,
		-50.0f,  50.0f, -50.0f,
		-50.0f,  50.0f, -50.0f,
		-50.0f,  50.0f,  50.0f,
		-50.0f, -50.0f,  50.0f,

		50.0f, -50.0f, -50.0f,
		50.0f, -50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f,  50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,

		-50.0f, -50.0f,  50.0f,
		-50.0f,  50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f, -50.0f,  50.0f,
		-50.0f, -50.0f,  50.0f,

		-50.0f,  50.0f, -50.0f,
		50.0f,  50.0f, -50.0f,
		50.0f,  50.0f,  50.0f,
		50.0f,  50.0f,  50.0f,
		-50.0f,  50.0f,  50.0f,
		-50.0f,  50.0f, -50.0f,

		-50.0f, -50.0f, -50.0f,
		-50.0f, -50.0f,  50.0f,
		50.0f, -50.0f, -50.0f,
		50.0f, -50.0f, -50.0f,
		-50.0f, -50.0f,  50.0f,
		50.0f, -50.0f,  50.0f
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
	faces.clear();

	Shader UniverseBoxShader;
	UniverseBoxShader.fileShader("UniverseBox/U_Sader.vt","UniverseBox/U_Sader.fg");
//行星模型初始化
	Sun sun(13.0f);
	sun.ReadShaderAndModel("SolarAndPlanet/Shader/SunShader.vt","SolarAndPlanet/Shader/SunShader.fg","SolarAndPlanet/Sun/Sun.obj");
	
	Planet Mercury(1.0f, 70.0f,0.5f);
	Mercury.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
		"SolarAndPlanet/Shader/PlanetBeanShader.fg",
		"SolarAndPlanet/Mercury/Mercury.obj");

/*	Planet Venus(1.0f, 50.0f,0.5f);
	Venus.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
		"SolarAndPlanet/Shader/PlanetShader.fg",
		"SolarAndPlanet/Venus/Venus.obj");

	Planet Earth(1.0f, 0.0f);
	Earth.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
		"SolarAndPlanet/Shader/PlanetShader.fg",
		"SolarAndPlanet/Earth/Earth.obj");

	Planet Moon(1.0f, 0.0f);
	Moon.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
		"SolarAndPlanet/Shader/PlanetShader.fg",
		"SolarAndPlanet/Moon/Moon.obj");

	Planet Mars(1.0f, 0.0f);
	Mars.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
		"SolarAndPlanet/Shader/PlanetShader.fg",
		"SolarAndPlanet/Mars/Mars.obj");

	Planet Jupiter(1.0f, 0.0f);
	Jupiter.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
		"SolarAndPlanet/Shader/PlanetShader.fg",
		"SolarAndPlanet/Jupiter/Jupiter.obj");

	Planet Neptune(1.0f, 0.0f);
	Neptune.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
		"SolarAndPlanet/Shader/PlanetShader.fg",
		"SolarAndPlanet/Neptune/Neptune.obj");

	Planet Uranus(1.0f, 0.0f);
	Uranus.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
		"SolarAndPlanet/Shader/PlanetShader.fg",
		"SolarAndPlanet/Uranus/Uranus.obj");

	Planet Saturn(1.0f, 0.0f);
	Saturn.ReadShaderAndModel("SolarAndPlanet/Shader/PlanetShader.vt",
		"SolarAndPlanet/Shader/PlanetShader.fg",
		"SolarAndPlanet/Saturn/Saturn.obj");
*/

	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastTime;
		lastTime = currentFrame;

		glfwPollEvents();
		cameraMove();
		GLfloat angle = 0.0f;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glm::mat4 BoxView, projection;
		BoxView = SolarSystemCamera.GetViewMatrix();
		projection = glm::perspective(SolarSystemCamera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 1000.0f);

		glDepthFunc(GL_LEQUAL);
		UniverseBoxShader.Use();
		BoxView = glm::mat4(glm::mat3(SolarSystemCamera.GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(UniverseBoxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(BoxView));
		glUniformMatrix4fv(glGetUniformLocation(UniverseBoxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(UbVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(UniverseBoxShader.Program, "UniverseBoxTexture"),0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, UniverseBoxTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		glm::mat4 model;
		model = glm::scale(model, glm::vec3(13.0f));

		sun.GetMatix(SolarSystemCamera.GetViewMatrix(), projection,glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		sun.RenderPlanet();
		//Sun.RenderOrbit();

		Mercury.GetMatix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Mercury.RenderPlanet();

/*
		Venus.GetMatix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Venus.RenderPlanet();

		Earth.GetMatix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Earth.RenderPlanet();

		Moon.GetMatix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Moon.RenderPlanet();

		Mars.GetMatix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Mars.RenderPlanet();

		Jupiter.GetMatix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Jupiter.RenderPlanet();

		Neptune.GetMatix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Neptune.RenderPlanet();

		Uranus.GetMatix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Uranus.RenderPlanet();

		Saturn.GetMatix(SolarSystemCamera.GetViewMatrix(), projection, glm::vec3(SolarSystemCamera.Position.x, SolarSystemCamera.Position.y, SolarSystemCamera.Position.z));
		Saturn.RenderPlanet();
*/

		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}
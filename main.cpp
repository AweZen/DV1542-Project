//ALLA DEFINE HÄR!!
#define GLM_ENABLE_EXPERIMENTAL
#define WIDTH 1920.0f
#define HEIGHT 1080.0f

//CLASS INCLUDES
#include "ObjectData.h"
#include "Window.h"
#include "Glow.h"

float mouseSpeed = 1.f;
float horizontalAngle = 3.14f;
float verticalAngle = 0.0f;

float deltaTime;
float timeSinceJump;
float gFloat     = 0.0f;
float gIncrement = 0.0f;
float gIncrement2 = 0.0f;
int counter = 0;

float rotation   = 0.0f;
float lastTime   = 0;
float initialFoV = 45.0f;
float speed		 = 3.0f;
bool show_demo_window	 = true;
bool show_another_window = false;
bool EnableMouse		 = true;
bool deBugCulling		 = false;
bool glowEnabled		 = false;
bool normalTexture		 = false;
bool finalTexture		 = true;
bool positionTexture	 = false;
bool colorTexture		 = false;
bool glowTexture		 = true;
bool UVTexture			 = false;
bool lightOne = false;
bool lightTwo = false;

bool frustumEnabled = false;
bool normalEnabled = false;
bool heightEnabled = false;

matrix pvw;
float farPlaneSize = (2 * tan((3.14f * 0.45f) / 2) * 500);
float nearPlaneSize = (2 * tan((3.14f * 0.45f) / 2) * 0.1f);
glm::vec3 position  = glm::vec3(0, 3, 3);
glm::vec3 direction = glm::vec3(0, 1, 0);
glm::vec3 up        = glm::vec3(0, 1, 0);
glm::vec3 paralell  = glm::vec3(sin(horizontalAngle - 3.14f / 2.0f),0,cos(horizontalAngle - 3.14f / 2.0f));

PlanePoints farPlane;
PlanePoints nearPlane;
ObjectDataHandler *handlerHeight[1];
ObjectDataHandler *handlerNormal[2];
ObjectDataHandler *handlerFrustum[1];
ObjectDataHandler *handlerGlow[1];

Shader  *quadTreeShader, *FSQShader;
Window *testW;
Deferred *deferred;
Light *lights[1000];
Glow *glow;
FullScreenQuad *FSquad;

void calcPlanes() 
{
	glm::vec3 farCenter, nearCenter;
	farCenter = position + direction * 500.f;
	nearCenter = position + direction * 0.01f;

	farPlane.topRight = farCenter + up * farPlaneSize	/2+ paralell * (farPlaneSize * (float)WIDTH / (float)HEIGHT) /2;
	farPlane.topLeft = farCenter + up* farPlaneSize		/2- paralell* (farPlaneSize * (float)WIDTH / (float)HEIGHT)  /2;
	farPlane.botRight = farCenter - up * farPlaneSize	/2+ paralell * (farPlaneSize * (float)WIDTH / (float)HEIGHT) /2;
	farPlane.botLeft = farCenter - up * farPlaneSize	/2- paralell * (farPlaneSize * (float)WIDTH / (float)HEIGHT) /2;

	nearPlane.topRight = nearCenter + up * nearPlaneSize	/2+ paralell * (nearPlaneSize * (float)WIDTH / (float)HEIGHT)	/2;
	nearPlane.topLeft = nearCenter + up * nearPlaneSize		/2- paralell * (nearPlaneSize * (float)WIDTH / (float)HEIGHT)	/2;
	nearPlane.botRight = nearCenter - up * nearPlaneSize	/2+ paralell * (nearPlaneSize * (float)WIDTH / (float)HEIGHT)	/2;
	nearPlane.botLeft = nearCenter - up * nearPlaneSize		/2- paralell * (nearPlaneSize * (float)WIDTH / (float)HEIGHT)	/2;
															
	farPlane.leftNormal = cross(farPlane.topLeft - nearPlane.topLeft, farPlane.topLeft - farPlane.botLeft);
	farPlane.rightNormal = cross(farPlane.topRight - farPlane.botRight, farPlane.topRight - nearPlane.topRight);

	nearPlane.leftNormal = farPlane.leftNormal;
	nearPlane.rightNormal = farPlane.rightNormal;
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (EnableMouse) {
		glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);

		horizontalAngle += mouseSpeed * deltaTime * float(WIDTH  / 2 - xpos);
		verticalAngle   += mouseSpeed * deltaTime * float(HEIGHT / 2 - ypos);

		direction = glm::vec3(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
		);

		paralell = glm::vec3(
			sin(horizontalAngle - 3.14f / 2.0f),
			0,
			cos(horizontalAngle - 3.14f / 2.0f)
		);

		up = glm::cross(paralell, direction);

		pvw.View = glm::lookAt(
			position,				// Camera is at (4,3,3), in World Space
			position + direction,	// and looks at the origin
			up);					// Head is up (set to 0,-1,0 to look upside-down)
	
		deferred->GeoShader->useShader();
		deferred->GeoShader->setUniform("pos", position);
		deferred->LightShader->useShader();
		deferred->LightShader->setUniform("pos", position);
		if (!deBugCulling) {
			calcPlanes();
		}

	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	int state = glfwGetKey(window, GLFW_KEY_TAB);
	if (key == GLFW_KEY_TAB && state == GLFW_RELEASE)
	{
		if (EnableMouse) {
			EnableMouse = false;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else {
			glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			EnableMouse = true;
		}

	}
}

void createMatrix() {
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	pvw.World = scaleMatrix;

	pvw.Projection = glm::perspective(3.14f * 0.45f, (float)WIDTH / (float)HEIGHT, 0.1f, 500.0f);
	cursor_position_callback(testW->_window, WIDTH / 2, HEIGHT / 2);
	EnableMouse = false;

	calcPlanes();
}

void createObject()
{
	//OBJECTHANDLERS
	handlerGlow[0] = new ObjectDataHandler(2502,false);
	handlerFrustum[0] = new ObjectDataHandler(2502);
	handlerHeight[0] = new ObjectDataHandler(2, false);
	handlerNormal[0] = new ObjectDataHandler(2502, false, true);
	handlerNormal[1] = new ObjectDataHandler(2502, false);

	//ADD OBJECTS:
	//TO GLOW HANDELER
	handlerGlow[0]->addObject(Cube, 3, pvw.World, glm::vec3(7, 3, 0), "res/lava_flow.png", glm::vec2(0, 0.0), glm::vec2(1, 0.0625), true);
	handlerGlow[0]->addObject(pvw.World, glm::vec3(2, 3, 1), "res/planet.obj");

	//TO NORMAL MAPPING HANDELER
	handlerNormal[0]->addObject(Cube, 2, pvw.World, glm::vec3(7, 2, 0), "res/green.png", glm::vec2(0, 0), glm::vec2(1, 1), false, "res/normal4.jpg");
	handlerNormal[0]->addObject(Cube, 2, pvw.World, glm::vec3(1, 2, 0), "res/yellow.jpg", glm::vec2(0, 0), glm::vec2(1, 1), false, "res/normal2.jpg");
	handlerNormal[0]->addObject(Cube, 2, pvw.World, glm::vec3(16, 2, 0), "res/brown.jpg", glm::vec2(0, 0), glm::vec2(1, 1), false, "res/normal3.jpg");
	handlerNormal[1]->addObject(Cube, 2, pvw.World, glm::vec3(10, 2, 0), "res/green.png");
	handlerNormal[1]->addObject(Cube, 2, pvw.World, glm::vec3( 4, 2, 0), "res/yellow.jpg");
	handlerNormal[1]->addObject(Cube, 2, pvw.World, glm::vec3(13, 2, 0), "res/brown.jpg");
	handlerNormal[1]->addObject(Plane, 100, pvw.World, glm::vec3(0, 0, 0), "res/Stone.jpg");

	//TO HEIGHTMAP HANDELER
	handlerHeight[0]->addObject(Terrain, pvw.World, "res/heightmap.bmp", "res/green.png", 10, { 0,-1,0 });

	int i;
	//TO FRUSTUM HANDELER
	for (i = 0; i < 25; i++) {
		for (int j = 0; j < 25; j++) {
				handlerFrustum[0]->addObject(Cube, 2, pvw.World, glm::vec3(3 * i - 38, 2, 3 * j - 38), "res/brown.jpg", glm::vec2(0, 0.0), glm::vec2(1, 1), false);

		}
	}

	//LIGHTS
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			lights[5 + i + j * 10] = new Light(glm::vec3(rand() % 2, rand() % 2, rand() % 2), 0.1, 1, glm::vec3(rand() % 80 - 38, 5, rand() % 80 - 38), Attenuation(0.1f, 0.005f, 0.005f));
		}
	}


	lights[4] = new Light(glm::vec3(0, 1, 1), 0.1, 1, glm::vec3(0, 3, 10), Attenuation(1.f, 0.05f, 0.05f));
	lights[5] = new Light(glm::vec3(1, 0, 1), 0.1, 1, glm::vec3(0, 3, -10), Attenuation(1.f, 0.05f, 0.05f));
	lights[2] = new Light(glm::vec3(1, 1, 0), 0.1, 1, glm::vec3(-10, 3, 5), Attenuation(1.f, 0.05f, 0.05f));
	lights[3] = new Light(glm::vec3(1, 0, 1), 0.1, 1, glm::vec3(28, 3, 12), Attenuation(1.f, 0.05f, 0.05f));
	lights[0] = new Light({ 1,1,1 }, 0.1, 1, { glm::vec3(1.f, 2, 2) }, WIDTH, HEIGHT, FSQShader);
	lights[1] = new Light({ 1,1,1 }, 0.1, 1, { glm::vec3(0.f, 1, 2) }, WIDTH, HEIGHT, FSQShader);
}

void createShader()
{
	quadTreeShader = new Shader("LineVS.glsl", "LineFS.glsl");
	FSQShader = new Shader("FSQvs.glsl", "FSQfs.glsl");
}

void createWindow()
{
	testW = new Window(WIDTH, HEIGHT, "DV1542 3D Programming Project");
	createShader();
	deferred = new Deferred(WIDTH, HEIGHT, FSQShader);
	glow = new Glow(WIDTH, HEIGHT, FSQShader);
	FSquad = new FullScreenQuad(FSQShader);
	createMatrix();
	createObject();
}

void SetupImGui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(testW->_window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
}

void Render()
{
	glfwPollEvents();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
	{
		static float f = 0.0f;
		ImGui::SetWindowSize(ImVec2(100, 200));
		ImGui::Begin("Press tab to activateMouse");
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::SliderFloat("Rotation", &gIncrement, 0.0f, 1.0f);

		ImGui::Checkbox("Want Glow scene?", &glowEnabled);
		if (glowEnabled) {
			heightEnabled = false;
			normalEnabled = false;
			frustumEnabled = false;
			glowTexture = true;
		}

		ImGui::Checkbox("Want Heightmap scene?", &heightEnabled);
		if (heightEnabled)
		{
			glowEnabled = false;
			normalEnabled = false;
			frustumEnabled = false;
			glowTexture = false;
		}

		ImGui::Checkbox("Want Normal scene?", &normalEnabled);
		if (normalEnabled)
		{
			heightEnabled = false;
			glowEnabled = false;
			frustumEnabled = false;
			glowTexture = false;
		}

		ImGui::Checkbox("Want Frustum scene?", &frustumEnabled);
		if (frustumEnabled)
		{
			heightEnabled = false;
			normalEnabled = false;
			glowEnabled = false;
			glowTexture = false;
		}

		if (!glowEnabled && !heightEnabled && !normalEnabled && !frustumEnabled) {
			frustumEnabled = true;
			glowTexture = false;
		}

		ImGui::Checkbox("Final texture", &finalTexture);
		if (finalTexture)
		{
			colorTexture = false;
			normalTexture = false;
			positionTexture = false;
			glowTexture = false;
			UVTexture = false;
			lightOne = false;
			lightTwo = false;
		}

		ImGui::Checkbox("Normal texture", &normalTexture);
		if (normalTexture)
		{
			finalTexture = false;
			colorTexture = false;
			positionTexture = false;
			glowTexture = false;
			UVTexture = false;
			lightOne = false;
			lightTwo = false;
		}

		ImGui::Checkbox("Color texture", &colorTexture);
		if (colorTexture)
		{
			finalTexture = false;
			normalTexture = false;
			positionTexture = false;
			glowTexture = false;
			UVTexture = false;
			lightOne = false;
			lightTwo = false;
		}

		ImGui::Checkbox("Position texture", &positionTexture);
		if (positionTexture)
		{
			finalTexture = false;
			normalTexture = false;
			colorTexture = false;
			glowTexture = false;
			UVTexture = false;
			lightOne = false;
			lightTwo = false;
		}
		ImGui::Checkbox("UV texture", &UVTexture);
		if (UVTexture)
		{
			finalTexture = false;
			normalTexture = false;
			colorTexture = false;
			glowTexture = false;
			positionTexture = false;
			lightOne = false;
			lightTwo = false;


		}
		ImGui::Checkbox("ShadowMap 1", &lightOne);
		if (lightOne)
		{
			finalTexture = false;
			normalTexture = false;
			colorTexture = false;
			glowTexture = false;
			positionTexture = false;
			lightTwo = false;

		}
		ImGui::Checkbox("ShadowMap 2", &lightTwo);
		if (lightTwo)
		{
			finalTexture = false;
			normalTexture = false;
			colorTexture = false;
			glowTexture = false;
			positionTexture = false;
			lightOne = false;

		}
		if (glowEnabled) {
			ImGui::Checkbox("Glow texture", &glowTexture);
			if (glowTexture)
			{
				finalTexture = false;
				normalTexture = false;
				colorTexture = false;
				positionTexture = false;
				UVTexture = false;
				lightOne = false;
				lightTwo = false;
			}
		}

		if (!finalTexture && !normalTexture && !colorTexture && !positionTexture && !glowTexture && !UVTexture && !lightOne && !lightTwo) {
			finalTexture = true;
		}

		ImGui::Checkbox("Stop updating culling?", &deBugCulling);
		ImGui::Text("Objects rendered using frustumCulling = %d", counter);
		ImGui::End();
	}


	if (show_another_window)
	{
		ImGui::Begin("Useless meme", &show_another_window);
		ImGui::Text("I wanna be tracer!!!");
		if (ImGui::Button("Press me daddy"))
		{
			cout << "im already tracer!!" << endl;
			show_another_window = false;
		}
		ImGui::End();
	}

	ImGui::Render();

	counter = 0;

	glViewport(0, 0, WIDTH, HEIGHT);

	deferred->LightShader->setUniform("screenSize", glm::vec2(WIDTH, HEIGHT));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (glowEnabled) {
		deferred->Render(handlerGlow, farPlane, nearPlane, deltaTime, gIncrement, 1, lights, 2);
		glow->renderGlow(deferred->GetFinalTexture(), deferred->GetColorTexture(), handlerGlow, farPlane, nearPlane, deltaTime, gIncrement, 3);

		if (glowTexture)
			FSquad->Texture(glow->getFinalTexture());
		if (normalTexture)
			FSquad->Texture(deferred->GetNormalTexture());
		if (finalTexture)
			FSquad->Texture(deferred->GetFinalTexture());
		if (colorTexture)
			FSquad->Texture(deferred->GetColorTexture());
		if (positionTexture)
			FSquad->Texture(deferred->GetPositionTexture());
		if (UVTexture)
			FSquad->Texture(deferred->GetUVTexture());
		if (lightOne)
			FSquad->Texture(lights[0]->GetShadowMap());
		if (lightTwo)
			FSquad->Texture(lights[1]->GetShadowMap());
	}
	else if (heightEnabled)
	{
		deferred->Render(handlerHeight, farPlane, nearPlane, deltaTime, gIncrement, 1, lights, 1);
		if (normalTexture)
			FSquad->Texture(deferred->GetNormalTexture());
		if (finalTexture)
			FSquad->Texture(deferred->GetFinalTexture());
		if (colorTexture)
			FSquad->Texture(deferred->GetColorTexture());
		if (positionTexture)
			FSquad->Texture(deferred->GetPositionTexture());
		if (UVTexture)
			FSquad->Texture(deferred->GetUVTexture());
		if (lightOne)
			FSquad->Texture(lights[0]->GetShadowMap());
		if (lightTwo)
			FSquad->Texture(lights[1]->GetShadowMap());
	}
	else if (normalEnabled)
	{
		deferred->Render(handlerNormal, farPlane, nearPlane, deltaTime, gIncrement, 2, lights, 1);
		if (normalTexture)
			FSquad->Texture(deferred->GetNormalTexture());
		if (finalTexture)
			FSquad->Texture(deferred->GetFinalTexture());
		if (colorTexture)
			FSquad->Texture(deferred->GetColorTexture());
		if (positionTexture)
			FSquad->Texture(deferred->GetPositionTexture());
		if (UVTexture)
			FSquad->Texture(deferred->GetUVTexture());
		if (lightOne)
			FSquad->Texture(lights[0]->GetShadowMap());
		if (lightTwo)
			FSquad->Texture(lights[1]->GetShadowMap());
	}
	else if (frustumEnabled)
	{
		deferred->Render(handlerFrustum, farPlane, nearPlane, deltaTime, gIncrement, 1, lights, 104);

		if (normalTexture)
			FSquad->Texture(deferred->GetNormalTexture());
		if (finalTexture)
			FSquad->Texture(deferred->GetFinalTexture());
		if (colorTexture)
			FSquad->Texture(deferred->GetColorTexture());
		if (positionTexture)
			FSquad->Texture(deferred->GetPositionTexture());
		if (UVTexture)
			FSquad->Texture(deferred->GetUVTexture());
		if (lightOne)
			FSquad->Texture(lights[0]->GetShadowMap());
		if (lightTwo)
			FSquad->Texture(lights[1]->GetShadowMap());

	}






	//Render Imgui
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	//Uppdatera till window
	glfwMakeContextCurrent(testW->_window);
	glfwSwapBuffers(testW->_window);
}

void CloseProgram() {

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(testW->_window);
	glfwTerminate();
}

void Keys() {
	bool pressed = false;
	if (glfwGetKey(testW->_window, GLFW_KEY_W) == GLFW_PRESS) {
		position += direction * deltaTime * speed;
		pressed = true;
	}
	if (glfwGetKey(testW->_window, GLFW_KEY_S) == GLFW_PRESS) {
		position -= direction * deltaTime * speed;
		pressed = true;
	}
	if (glfwGetKey(testW->_window, GLFW_KEY_D) == GLFW_PRESS) {
		position += paralell * deltaTime * speed;
		pressed = true;
	}
	if (glfwGetKey(testW->_window, GLFW_KEY_A) == GLFW_PRESS) {
		position -= paralell * deltaTime * speed;
		pressed = true;
	}
	
	if (pressed) {
		if (heightEnabled) {
			position.y = handlerHeight[0]->getHeight(position.x, position.z, position.x + direction.x, position.z + direction.z);

		}
		pvw.View = glm::lookAt(position, position + direction, up);
		if (!deBugCulling) {
			calcPlanes();
		}
	}

	deferred->GeoShader->useShader();
	deferred->GeoShader->setUniform("pos", position);
	deferred->LightShader->useShader();
	deferred->LightShader->setUniform("pos", position);
}

int main(){
	createWindow();
	SetupImGui();

	glfwSetCursorPos(testW->_window, WIDTH / 2, HEIGHT / 2);
	glfwSetCursorPosCallback(testW->_window, cursor_position_callback);
	glfwSetKeyCallback(testW->_window, key_callback);

	while (glfwGetKey(testW->_window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(testW->_window) == 0) 
	{
		double currentTime = glfwGetTime();
		deltaTime = float(currentTime - lastTime);
		lastTime = currentTime;
		Keys();
		Render();
		glfwPollEvents();
	}
	CloseProgram();
	return 0;
}


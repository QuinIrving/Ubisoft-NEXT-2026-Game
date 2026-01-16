///////////////////////////////////////////////////////////////////////////////
// Filename: GameTest.cpp
// Provides a demo of how to use the API
///////////////////////////////////////////////////////////////////////////////

#if BUILD_PLATFORM_WINDOWS
//------------------------------------------------------------------------
#include <windows.h> 
#endif

#include <iostream>
//------------------------------------------------------------------------
#include <math.h>  
//------------------------------------------------------------------------
#include "../ContestAPI/app.h"
//------------------------------------------------------------------------

#include "Math/MathConstants.h"
#include "Math/MatVecQuat_ops.h"
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"
#include "Math/Quaternion.h"
#include "Math/Mat4.h"
#include "Graphics/Pipeline.h"
#include "Objects/Quad.h"
#include <main.h>
#include "Graphics/Pipeline.h"
#include "Tessellation/TessellatedPipeline.h"
#include "Loaders/ObjectLoader.h"
#include "Tessellation/TriangleNode.h"


//Pipeline& p = Pipeline::GetInstance();
TessellatedPipeline& p = TessellatedPipeline::GetInstance();

//------------------------------------------------------------------------
// Example data....
//------------------------------------------------------------------------
CSimpleSprite* testSprite;
enum
{
	ANIM_FORWARDS,
	ANIM_BACKWARDS,
	ANIM_LEFT,
	ANIM_RIGHT,
};

// This is where our ECS should be
/*Vec2<float> t2;
Vec3<float> t3;
Vec4<float> t4;
Quaternion q;
Mat4<float> m;*/

//m.Print();
//q.Print();
bool moveForward;

// Don't forget we have a negative z camera view.
Vertex v1 = Vertex(-1, -1, -1);
Vertex v2 = Vertex(1, -1, -10);
Vertex v3 = Vertex(-1, 1, -900);

//Vertex v4 = Vertex(1, -1, -1);
Vertex v4 = Vertex(1.f, 1, 0);
//Vertex v6 = Vertex(-1, 1, -1);

std::vector<Vertex> randV = {};
std::vector<uint32_t> indices = {};

Quad q = Quad(1, 1, 2, Vec4<float>(1.f, 0.f, 0.f, 1.f));
//Quad q = Quad(8, 2, 2, Vec4<float>(1.f, 0.f, 0.f, 1.f));
std::vector<Mesh> quadMeshes;
ModelEdge quadEdges;
Model bunny;
Model fox;
//------------------------------------------------------------------------

float mX;
float mY;
Vec2<float> mousePos;
Vec2<float> diff;

Vec2<int> windowSize;

std::vector<Vertex> foxVerts;
std::vector<uint32_t> foxIndices;
ModelAttributes ma;

int frames = 0;

Quad q2 = Quad(1, 1, 2, Vec4<float>(1.f, 0.f, 0.f, 1.f));
std::vector<Mesh> q2Meshes;
ModelEdge q2Edges;

//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{
	/*alignas(16) float o[4] = {1.f, 2.f, 3.f, 4.f};
	__m128 veca = _mm_load_ps(o);*/ // SIMD for SSE x86, need to worry about mac os as well.
	windowSize = { WINDOW_WIDTH, WINDOW_HEIGHT };
	App::GetMousePos(mX, mY);
	mousePos = { mX, mY };

	moveForward = true;
	q.Translate(0, 0, -2);
	q2.Translate(-2, 0, 0);
	q2.Rotate(0, 90, 0);
	//quadMeshes.push_back(q.GetMesh());
	quadEdges = q.GetAdjacencyTable();

	/*for (int i = 0; i < 1966; ++i) {
		indices.push_back(i);
		randV.push_back(Vertex(1, 2, 3));
	}*/

	//bunny = ObjectLoader::Load("./data/Models/Bunny/stanford-bunny.obj");
	//bunny.Translate(0, 0, -2);
	fox = ObjectLoader::Load("./data/Models/Fox/low-poly-fox.obj");
	fox.Translate(0, 0.25, -1);
	fox.Rotate(180, 90, 0);
	fox.Scale(0.025);

	//TriangleNode::MAX_DEPTH = 15;

	std::string t = "./data/Textures/Wall/brickwall.tga";
	//std::string t = "./data/Textures/Rock/RockColour.tga";
	//std::string t = "./data/Textures/Rock/StoneBricksColour.tga";
	//std::string t = "./data/Textures/Rock/PavingStonesColor.tga";
	//std::string t = "./data/Textures/Rock/TilesPatternColor.tga";
	//std::string t = "./data/Textures/Rock/TilesCheckeredColor.tga";
	TextureLoader::textureMap["brickwall"] = TextureLoader::ProcessTGA(t);
	quadMeshes.push_back(q.GetMesh(TextureLoader::GenerateTextureTopology(t)));
	q2Meshes.push_back(q2.GetMesh());
	q2Meshes[0].material.map_Kd = std::make_shared<Texture>(TextureLoader::textureMap["brickwall"]);

	//TextureLoader::textureMap["brickwall"] = TextureLoader::GenerateTextureTopology(t);
	
	quadMeshes[0].material.map_Kd = std::make_shared<Texture>(TextureLoader::textureMap["brickwall"]);

	/*foxVerts = fox.GetMeshes()[0].geometry->processedMesh;
	foxIndices.reserve(foxVerts.size());
	for (unsigned int i = 0; i < foxVerts.size(); ++i) {
		foxIndices.push_back(i);
	}

	ma.modelMatrix = fox.GetModelMatrix();
	ma.material = std::make_shared<Material>(fox.GetMeshes()[0].material);*/

	/*bunnyVerts = bunny.GetMeshes()[0].geometry->processedMesh;
	bunnyIndices.reserve(bunnyVerts.size());
	for (unsigned int i = 0; i < bunnyVerts.size(); ++i) {
		bunnyIndices.push_back(i);
	}

	ma.modelMatrix = bunny.GetModelMatrix();*/
	//------------------------------------------------------------------------
	// Example Sprite Code....
	testSprite = App::CreateSprite("./data/TestData/Test.bmp", 8, 4);
	testSprite->SetPosition(400.0f, 400.0f);
	const float speed = 1.0f / 15.0f;
	testSprite->CreateAnimation(ANIM_BACKWARDS, speed, { 0,1,2,3,4,5,6,7 });
	testSprite->CreateAnimation(ANIM_LEFT, speed, { 8,9,10,11,12,13,14,15 });
	testSprite->CreateAnimation(ANIM_RIGHT, speed, { 16,17,18,19,20,21,22,23 });
	testSprite->CreateAnimation(ANIM_FORWARDS, speed, { 24,25,26,27,28,29,30,31 });
	testSprite->SetScale(1.0f);
	int a = 1;
	//------------------------------------------------------------------------
}

//------------------------------------------------------------------------
// Update your simulation here. deltaTime is the elapsed time since the last update in ms.
// This will be called at no greater frequency than the value of APP_MAX_FRAME_RATE
//------------------------------------------------------------------------
void Update(const float deltaTime)
{
	frames++;
	//------------------------------------------------------------------------
	// Example Sprite Code....
	testSprite->Update(deltaTime);
	if (App::GetController().GetLeftThumbStickX() > 0.5f)
	{
		testSprite->SetAnimation(ANIM_RIGHT);
		float x, y;
		testSprite->GetPosition(x, y);
		x += 1.0f;
		testSprite->SetPosition(x, y);
	}
	if (App::GetController().GetLeftThumbStickX() < -0.5f)
	{
		testSprite->SetAnimation(ANIM_LEFT);
		float x, y;
		testSprite->GetPosition(x, y);
		x -= 1.0f;
		testSprite->SetPosition(x, y);
	}
	if (App::GetController().GetLeftThumbStickY() > 0.5f)
	{
		testSprite->SetAnimation(ANIM_FORWARDS);
		float x, y;
		testSprite->GetPosition(x, y);
		y += 1.0f;
		testSprite->SetPosition(x, y);
	}
	if (App::GetController().GetLeftThumbStickY() < -0.5f)
	{
		testSprite->SetAnimation(ANIM_BACKWARDS);
		float x, y;
		testSprite->GetPosition(x, y);
		y -= 1.0f;
		testSprite->SetPosition(x, y);
	}

	if (App::GetController().CheckButton(App::BTN_DPAD_UP, false))
	{
		testSprite->SetScale(testSprite->GetScale() + 0.1f);
		if (frames > 16) {
			NodeDepth::MAX_DEPTH = std::min<int>(NodeDepth::MAX_DEPTH + 1, 20);
			frames = 0;
		}
	}
	if (App::GetController().CheckButton(App::BTN_DPAD_DOWN, false))
	{
		testSprite->SetScale(testSprite->GetScale() - 0.1f);
		if (frames > 16) {
			NodeDepth::MAX_DEPTH = std::max<int>(NodeDepth::MAX_DEPTH - 1, 0);
			frames = 0;
		}
	}
	if (App::GetController().CheckButton(App::BTN_DPAD_LEFT, false))
	{
		testSprite->SetAngle(testSprite->GetAngle() + 0.1f);
	}
	if (App::GetController().CheckButton(App::BTN_DPAD_RIGHT, false))
	{
		testSprite->SetAngle(testSprite->GetAngle() - 0.1f);
	}
	if (App::GetController().CheckButton(App::BTN_A, true))
	{
		testSprite->SetAnimation(-1);
	}
	//------------------------------------------------------------------------
	// Sample Sound.
	//------------------------------------------------------------------------
	if (App::GetController().CheckButton(App::BTN_B, true))
	{
		App::PlayAudio("./Data/TestData/Test.wav", true);
	}
	if (App::GetController().CheckButton(App::BTN_X, true))
	{
		App::StopAudio("./Data/TestData/Test.wav");
	}

	//q.Translate(0, 0.002 * deltaTime, (-0.001) * deltaTime);
	//q.Rotate(0.021 * deltaTime, 0.021 * deltaTime, 0);

	/*float t = 0.45 / deltaTime;
	t = (moveForward) ? t : (-t);

	if (q.GetTranslation().z < -5) {
		t *= 2.5;
	}

	if (q.GetTranslation().z > -0.55 && t > 0) {
		moveForward = false;
	}
	else if (q.GetTranslation().z < -10 && t < 0) {
		moveForward = true;
	}*/

	// This whole issue is due to the type of pipeline naming, need to share the same camera between pipelines....

	// Mouse rotation
	/*Need to make all of this compatible with a controller as well! FOr movements and looking and jumping etc, should have a switch statement to handle either scenario*/
	App::GetMousePos(mX, mY);
	diff = Vec2<float>(mX, mY) - mousePos;
	mousePos = { mX, mY };
	float degX = (diff.y / static_cast<float>(APP_VIRTUAL_HEIGHT)) * p.camera.GetMouseSensitivty(); // Around x axis, we care about mouse Y
	float degY = (-diff.x / static_cast<float>(APP_VIRTUAL_WIDTH)) * p.camera.GetMouseSensitivty(); // Around y axis we care about mouse X
	p.camera.RotateXY(degX, degY);

	float camX = 0, camU = 0;
	float moveSpeed = 0.007;

	// Translation -> controller and keyboard same
	if (App::GetController().GetLeftThumbStickX() > 0.5f)
	{
		camX += moveSpeed;
	}
	if (App::GetController().GetLeftThumbStickX() < -0.5f)
	{
		camX -= moveSpeed;
	}
	if (App::GetController().GetLeftThumbStickY() > 0.5f)
	{
		camU += moveSpeed;
	}
	if (App::GetController().GetLeftThumbStickY() < -0.5f)
	{
		camU -= moveSpeed;
	}

	float camRotY = 0, camRotX = 0; // around the axis
	float div = 35;
	// Rotation (right thumb stick, same as mouse)
	if (App::GetController().GetRightThumbStickX() > 0.5f)
	{
		camRotY -= p.camera.GetMouseSensitivty() / div;
	}
	if (App::GetController().GetRightThumbStickX() < -0.5f)
	{
		camRotY += p.camera.GetMouseSensitivty() / div;
	}
	if (App::GetController().GetRightThumbStickY() > 0.5f)
	{
		camRotX -= p.camera.GetMouseSensitivty() / div;
	}
	if (App::GetController().GetRightThumbStickY() < -0.5f)
	{
		camRotX += p.camera.GetMouseSensitivty() / div;
	}

	p.camera.RotateXY(camRotX, camRotY);
	p.camera.Translate(camX, 0, camU);

	if (windowSize != Vec2<int>(WINDOW_WIDTH, WINDOW_HEIGHT)) {
		p.ResizeWindowProjection(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT));
		windowSize = Vec2<int>(WINDOW_WIDTH, WINDOW_HEIGHT);
	}

	//q.Translate(0, 0, t);
	//p.camera.RotateXY(0, 10 / deltaTime);
	//p.camera.
	//q.Rotate(0.1, 0.1, 0);
}

//------------------------------------------------------------------------
// Add your display calls here (DrawLine,Print, DrawSprite.) 
// See App.h 
//------------------------------------------------------------------------
void Render()
{
	//v1.SetColour(0, 0, 0, 255);
	//p.Render(std::vector<Vertex>({v1, v2, v3, v4}), std::vector<uint32_t>({0, 1, 2, 1, 3, 2}), ModelAttributes());
	//p.Render(q.GetVertices(), q.GetVertexIds(), q.GetModelAttributes());


	/* MAIN PIPELINE */
	//p.Render(bunnyVerts, bunnyIndices, ma);
	//p.Render(foxVerts, foxIndices, ma);
	/* TESSELLATED PIPELINE */ // Don't forget I need to share cameras.
	//p.Render(fox.GetMeshes(), fox.GetModelMatrix(), fox.GetAdjacencyTable());
	p.Render(quadMeshes, q.GetModelMatrix(), quadEdges);
	p.Render(q2Meshes, q2.GetModelMatrix(), q2Edges);
	// DONE!!!
	


	//App::DrawTriangle(600.0f, 300.0f, -0.5f, 1, 650.0f, 400.0f, -0.5f, 1, 700.0f, 300.0f, -0.5f, 1, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	//p.Render(randV, indices, ModelAttributes());
	//------------------------------------------------------------------------
	// Example Sprite Code....
	//testSprite->Draw();

	float x, y;
	App::GetMousePos(x, y);
	char textBuffer[64];
	snprintf(textBuffer, sizeof(textBuffer), "Mouse: (%f, %f)", x, y);
	App::Print(10, APP_VIRTUAL_HEIGHT - 20, textBuffer, 0.2f, 1.0f, 0.2f, GLUT_BITMAP_HELVETICA_10);
	float controllerY = App::GetController().GetRightThumbStickY();
	float controllerX = App::GetController().GetRightThumbStickX();

	snprintf(textBuffer, sizeof(textBuffer), "Controller: (%f, %f)", controllerX, controllerY);
	App::Print(10, APP_VIRTUAL_HEIGHT - 40, textBuffer, 0.2f, 1.0f, 0.2f, GLUT_BITMAP_HELVETICA_10);

	snprintf(textBuffer, sizeof(textBuffer), "Window: (%d, %d)", WINDOW_WIDTH, WINDOW_HEIGHT); // USE THIS VALUE FOR OUR AUTO-CHANGING PROJECTION MATRIX!
	App::Print(10, APP_VIRTUAL_HEIGHT - 80, textBuffer, 0.2f, 0.55f, 0.98f, GLUT_BITMAP_HELVETICA_10);

	// Camera will use a fixed sensitivity (that in settings can be changed), while mouse will use raw input given + sensitivity for our camera input.

	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	// Example Text.
	//------------------------------------------------------------------------
	//App::Print(100, 100, "Sample Text");

	//------------------------------------------------------------------------
	// Example Line Drawing.
	//------------------------------------------------------------------------
	static float a = 0.0f;
	const float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;
	a += 0.1f;
	for (int i = 0; i < 20; i++)
	{

		const float sx = 200 + sinf(a + i * 0.1f) * 60.0f;
		const float sy = 200 + cosf(a + i * 0.1f) * 60.0f;
		const float ex = 700 - sinf(a + i * 0.1f) * 60.0f;
		const float ey = 700 - cosf(a + i * 0.1f) * 60.0f;
		g = (float)i / 20.0f;
		b = (float)i / 20.0f;
		//App::DrawLine(sx, sy, ex, ey, r, g, b);
	}

	//------------------------------------------------------------------------
	// Example Triangle Drawing.
	//------------------------------------------------------------------------
	//App::DrawTriangle(600.0f, 300.0f, -1, 1, 650.0f, 400.0f, 0, 1, 700.0f, 300.0f, 0, 1, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	//App::DrawTriangle(500.0f, 300.0f, 0, 1, 550.0f, 450.0f, 0.5, 1, 700.0f, 340.0f, -0.5, 1, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	//App::DrawTriangle(800.0f, 300.0f, 0, 1, 850.0f, 400.0f, 0, 1, 900.0f, 300.0f, 0, 1, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, true);
	//App::DrawTriangle(800.0f, 300.0f, 0, 1, 850.0f, 400.0f, 0, 1, 900.0f, 300.0f, 0, 1, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, true);
	//App::DrawTriangle(320, 576, -0.5f, 1, 512, 384, -0.5f, 1, 704, 192, -0.5f, 1, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0.f, 1.f, 0.f, false);
}
//------------------------------------------------------------------------
// Add your shutdown code here. Called when the APP_QUIT_KEY is pressed.
// Just before the app exits.
//------------------------------------------------------------------------
void Shutdown()
{
	//------------------------------------------------------------------------
	// Example Sprite Code....
	delete testSprite;
	//------------------------------------------------------------------------
}
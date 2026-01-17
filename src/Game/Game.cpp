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
#include "Loaders/ObjectLoader.h"
#include "Tessellation/TriangleNode.h"
#include "Graphics/RenderPipeline.h"
#include "Entities/Player/Player.h"


RenderPipeline& p = RenderPipeline::GetInstance();
Player player;
//Pipeline& p = Pipeline::GetInstance();
//TessellatedPipeline& p = TessellatedPipeline::GetInstance();

// This is where our ECS should be
/*Vec2<float> t2;
Vec3<float> t3;
Vec4<float> t4;
Quaternion q;
Mat4<float> m;*/

//m.Print();
//q.Print();
/*
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
*/
float mX;
float mY;
static Vec2<float> mousePos;
Vec2<float> diff;

Vec2<int> windowSize;
int frames = 0;

/*
std::vector<Vertex> foxVerts;
std::vector<uint32_t> foxIndices;
ModelAttributes ma;



Quad q2 = Quad(1, 1, 2, Vec4<float>(1.f, 0.f, 0.f, 1.f));
std::vector<Mesh> q2Meshes;
ModelEdge q2Edges;*/
Quad q1 = Quad(1, 1, 100, Vec4<float>(1.f, 0.f, 0.f, 1.f));
Quad q2 = q1;
Quad q3 = q1;
Quad q4 = q1;
//std::vector<Mesh> q2Meshes;
bool isEscapeDown = false; // need a whole handler for this of keeping track of already down keys.
bool showCursor = false;

bool ignoreNextMouse = false;
bool wasCursorVisibleLastFrame = false;
int ignoreMouseFrames = 0;

bool fullScreen = false;
int prevWindowWidth = WINDOW_WIDTH;
int prevWindowHeight = WINDOW_HEIGHT;

char textBufferA[64];
char textBufferB[64];

int windowChange = 0;

//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{
	/*alignas(16) float o[4] = {1.f, 2.f, 3.f, 4.f};
	__m128 veca = _mm_load_ps(o);*/ // SIMD for SSE x86, need to worry about mac os as well.
	windowSize = { WINDOW_WIDTH, WINDOW_HEIGHT };
	glutWarpPointer(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
	App::GetMousePos(mX, mY);
	mousePos = { WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f };
	glutSetCursor(GLUT_CURSOR_NONE);
	player = Player();

	/*
	moveForward = true;
	q.Translate(0, 0, -2);
	q2.Translate(-2, 0, 0);
	q2.Rotate(0, 90, 0);
	//quadMeshes.push_back(q.GetMesh());
	quadEdges = q.GetAdjacencyTable();
	*/
	/*for (int i = 0; i < 1966; ++i) {
		indices.push_back(i);
		randV.push_back(Vertex(1, 2, 3));
	}*/

	//bunny = ObjectLoader::Load("./data/Models/Bunny/stanford-bunny.obj");
	//bunny.Translate(0, 0, -2);
	/*
	fox = ObjectLoader::Load("./data/Models/Fox/low-poly-fox.obj");
	fox.Translate(0, 0.25, -1);
	fox.Rotate(180, 90, 0);
	fox.Scale(0.025);*/

	//TriangleNode::MAX_DEPTH = 15;

	std::string t = "./data/Textures/Wall/brickwall.tga";
	//std::string t = "./data/Textures/Rock/RockColour.tga";
	//std::string t = "./data/Textures/Rock/StoneBricksColour.tga";
	//std::string t = "./data/Textures/Rock/PavingStonesColor.tga";
	//std::string t = "./data/Textures/Rock/TilesPatternColor.tga";
	//std::string t = "./data/Textures/Rock/TilesCheckeredColor.tga";
	TextureLoader::textureMap["brickwall"] = TextureLoader::ProcessTGA(t);
	/*quadMeshes.push_back(q.GetMesh(TextureLoader::GenerateTextureTopology(t)));
	*/
	

	q1.Translate(0, 0.f, 0);
	q2.Translate(0, 0, -5.f);
	q3.Translate(5.f, 0, -5.f);
	q4.Translate(5.f, 0, 0);
	//q1.Scale(50, 50, 0);
	q1.Rotate(90, 0, 0);
	q2.Rotate(90, 0, 0);
	q3.Rotate(90, 0, 0);
	q4.Rotate(90, 0, 0);
	//q2Meshes.push_back(q2.GetMesh());
	//q2Meshes[0].material.map_Kd = std::make_shared<Texture>(TextureLoader::textureMap["brickwall"]);

	//TextureLoader::textureMap["brickwall"] = TextureLoader::GenerateTextureTopology(t);

	//quadMeshes[0].material.map_Kd = std::make_shared<Texture>(TextureLoader::textureMap["brickwall"]);*/

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
}

//------------------------------------------------------------------------
// Update your simulation here. deltaTime is the elapsed time since the last update in ms.
// This will be called at no greater frequency than the value of APP_MAX_FRAME_RATE
//------------------------------------------------------------------------
void Update(const float deltaTime)
{
	if (windowSize != Vec2<int>(WINDOW_WIDTH, WINDOW_HEIGHT)) {
		p.ResizeWindowProjection(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT));
		windowSize = Vec2<int>(WINDOW_WIDTH, WINDOW_HEIGHT);
		glutWarpPointer(WINDOW_WIDTH / 2, (int)std::ceil(WINDOW_HEIGHT / 2));
		App::GetMousePos(mX, mY);
		windowChange = 1; // denotes that we just changed so skip a frame before going again
	}

	float deltaSeconds = deltaTime / 1000.f;
	frames++;
	
	//------------------------------------------------------------------------
	// Example Sprite Code....
	//testSprite->Update(deltaTime);
	/*if (App::GetController().GetLeftThumbStickX() > 0.5f)
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
	}*/
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

	if (App::IsKeyPressed(App::KEY_ESC) && !isEscapeDown) {
		showCursor = !showCursor;
		
		if (!showCursor) {
			glutSetCursor(GLUT_CURSOR_NONE);
		}
		else {
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}

		isEscapeDown = true;
	}

	if (!App::IsKeyPressed(App::KEY_ESC) && isEscapeDown) {
		isEscapeDown = false;
	}

	// This whole issue is due to the type of pipeline naming, need to share the same camera between pipelines....

	// Mouse rotation
	/*Need to make all of this compatible with a controller as well! FOr movements and looking and jumping etc, should have a switch statement to handle either scenario*/
	if (!showCursor) {
		App::GetMousePos(mX, mY);
		diff = Vec2<float>(mX - mousePos.x, mY - mousePos.y);
		
		if (windowChange == 1) { // Our window was changed, but we have dirty mousePos, wait until next frame
			windowChange = 2;
		}
		else if (windowChange == 2) {
			mousePos = Vec2<float>(mX, mY);
			windowChange = 0;
		}
		snprintf(textBufferA, sizeof(textBufferA), "New Mouse: (%f, %f)", mX, mY);
		
		float degX = (diff.y / static_cast<float>(APP_INIT_WINDOW_HEIGHT)) * player.GetMouseSensitivty(); // Around x axis, we care about mouse Y
		float degY = (-diff.x / static_cast<float>(APP_INIT_WINDOW_WIDTH)) * player.GetMouseSensitivty(); // Around y axis we care about mouse X
		glutWarpPointer(WINDOW_WIDTH / 2, (int)std::ceil(WINDOW_HEIGHT / 2));
		snprintf(textBufferB, sizeof(textBufferB), "Warped Mouse: (%f, %f)", mousePos.x, mousePos.y);

		player.RotateXY(degX, degY);
	}

	int moveRight = 0;
	int moveForward = 0;

	// Translation -> controller and keyboard same
	if (App::GetController().GetLeftThumbStickX() > 0.5f || App::IsKeyPressed(App::KEY_D))
	{
		moveRight += 1;
	}
	if (App::GetController().GetLeftThumbStickX() < -0.5f || App::IsKeyPressed(App::KEY_A))
	{
		moveRight -= 1;
	}
	if (App::GetController().GetLeftThumbStickY() > 0.5f || App::IsKeyPressed(App::KEY_W))
	{
		moveForward += 1;
	}
	if (App::GetController().GetLeftThumbStickY() < -0.5f || App::IsKeyPressed(App::KEY_S))
	{
		moveForward -= 1;
	}

	if (App::GetController().CheckButton(App::BTN_X, false) && player.GetMoveState() != MovementState::AIR) {
		// add an impulse to the y velocity, and set player as in air.
		player.UpdateVelocity(Vec3<float>(0.f, -16.f, 0.f));
		player.TransitionMoveState(MovementState::AIR);
	}
	else if (App::IsKeyPressed(App::KEY_SPACE) && player.GetMoveState() != MovementState::AIR) { // same for keyboard
		player.UpdateVelocity(Vec3<float>(0.f, -16.f, 0.f));
		player.TransitionMoveState(MovementState::AIR);
	}

	float camRotY = 0, camRotX = 0; // around the axis
	camRotY -= App::GetController().GetRightThumbStickX() * player.GetControllerSensitivty() * (deltaSeconds);
	camRotX -= App::GetController().GetRightThumbStickY() * player.GetControllerSensitivty() * (deltaSeconds);
	player.RotateXY(camRotX, camRotY);

	MovementSystem::HandlePlayerMovement(player, Vec3<float>(moveRight, 0, moveForward), deltaSeconds);

	if (player.GetPosition().y >= 0.f) {
		player.SetYPosition(0.f);
		player.ResetOffGroundTimer();
		player.TransitionMoveState(MovementState::GROUND);
	}
}

//------------------------------------------------------------------------
// Add your display calls here (DrawLine,Print, DrawSprite.) 
// See App.h 
//------------------------------------------------------------------------
void Render()
{
	p.Render(q1.GetVertices(), q1.GetModelMatrix(), player.GetViewMatrix(), Colour(3 / 255.f, 219 / 255.f, 252 / 255.f, 1.f));
	p.Render(q2.GetVertices(), q2.GetModelMatrix(), player.GetViewMatrix(), Colour(161 / 255.f, 57 / 255.f, 230 / 255.f, 1.f));
	p.Render(q3.GetVertices(), q3.GetModelMatrix(), player.GetViewMatrix(), Colour(3 / 255.f, 219 / 255.f, 252 / 255.f, 1.f));
	p.Render(q4.GetVertices(), q4.GetModelMatrix(), player.GetViewMatrix(), Colour(161 / 255.f, 57 / 255.f, 230 / 255.f, 1.f));
	/* MAIN PIPELINE */
	//p.Render(bunnyVerts, bunnyIndices, ma);
	//p.Render(foxVerts, foxIndices, ma);
	/* TESSELLATED PIPELINE */ // Don't forget I need to share cameras.
	//p.Render(fox.GetMeshes(), fox.GetModelMatrix(), fox.GetAdjacencyTable());
	//p.Render(quadMeshes, q.GetModelMatrix(), quadEdges);
	//p.Render(q2Meshes, q2.GetModelMatrix(), q2Edges);
	// DONE!!!
	


	//App::DrawTriangle(600.0f, 300.0f, -0.5f, 1, 650.0f, 400.0f, -0.5f, 1, 700.0f, 300.0f, -0.5f, 1, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	//p.Render(randV, indices, ModelAttributes());
	//------------------------------------------------------------------------
	// Example Sprite Code....
	//testSprite->Draw();

	/*float x, y;
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
	*/

	char textBuffer[64];
	snprintf(textBuffer, sizeof(textBuffer), "Pos: (%f, %f, %f)", player.GetPosition().x, player.GetPosition().y, player.GetPosition().z);
	App::Print(10, APP_VIRTUAL_HEIGHT - 20, textBuffer, 0.2f, 1.0f, 0.2f, GLUT_BITMAP_HELVETICA_10);

	snprintf(textBuffer, sizeof(textBuffer), "Vel: (%f, %f, %f)", player.GetVelocity().x, player.GetVelocity().y, player.GetVelocity().z);
	App::Print(10, APP_VIRTUAL_HEIGHT - 40, textBuffer, 0.2f, 1.0f, 0.2f, GLUT_BITMAP_HELVETICA_10);

	App::Print(10, APP_VIRTUAL_HEIGHT - 120, textBufferA, 0.2f, 1.0f, 0.2f, GLUT_BITMAP_HELVETICA_10);
	App::Print(10, APP_VIRTUAL_HEIGHT - 140, textBufferB, 0.2f, 1.0f, 0.2f, GLUT_BITMAP_HELVETICA_10);

	std::string stateText = "GROUND";
	if (player.GetMoveState() == MovementState::AIR) {
		stateText = "AIR";
	} else if (player.GetMoveState() == MovementState::GRAPPLEHOOK) {
		stateText = "GRAPPLEHOOK";
	}

	snprintf(textBuffer, sizeof(textBuffer), "State: %s", stateText);
	App::Print(10, APP_VIRTUAL_HEIGHT - 60, textBuffer, 0.2f, 1.0f, 0.2f, GLUT_BITMAP_HELVETICA_10);

	// Camera will use a fixed sensitivity (that in settings can be changed), while mouse will use raw input given + sensitivity for our camera input.

	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	// Example Text.
	//------------------------------------------------------------------------
	//App::Print(100, 100, "Sample Text");

	//------------------------------------------------------------------------
	// Example Line Drawing.
	//------------------------------------------------------------------------
	/*static float a = 0.0f;
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
	}*/

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
	//delete testSprite;
	//------------------------------------------------------------------------
}
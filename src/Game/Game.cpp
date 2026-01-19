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
#include "Entities/World.h"
#include <Physics/Collisions/Collision.h>


RenderPipeline& p = RenderPipeline::GetInstance();
World world;
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

constexpr float SLOW_TIME = 1.f;

/*
std::vector<Vertex> foxVerts;
std::vector<uint32_t> foxIndices;
ModelAttributes ma;



Quad q2 = Quad(1, 1, 2, Vec4<float>(1.f, 0.f, 0.f, 1.f));
std::vector<Mesh> q2Meshes;
ModelEdge q2Edges;*/

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
	world.player = Player();
	/*
	world.quads.insert(world.quads.end(), { Quad(1, 1, 100, Colour(FRAND_RANGE(0, 255) / 255.f, FRAND_RANGE(0, 255) / 255.f, FRAND_RANGE(0, 255) / 255.f, 1.f)), Quad(1, 1, 100, Colour(FRAND_RANGE(0, 255) / 255.f, FRAND_RANGE(0, 255) / 255.f, FRAND_RANGE(0, 255) / 255.f, 1.f)),
		Quad(1, 1, 100, Colour(FRAND_RANGE(0, 255) / 255.f, FRAND_RANGE(0, 255) / 255.f, FRAND_RANGE(0, 255) / 255.f, 1.f)), Quad(1, 1, 100, Colour(FRAND_RANGE(0, 255) / 255.f, FRAND_RANGE(0, 255) / 255.f, FRAND_RANGE(0, 255) / 255.f, 1.f)) });

	world.quads.push_back(Quad(1, 1, 200, Colour(FRAND_RANGE(0, 255) / 255.f, FRAND_RANGE(0, 255) / 255.f, FRAND_RANGE(0, 255) / 255.f, 1.f)));
	world.quads.push_back(Quad(1, 10, 10, Colour(FRAND_RANGE(0, 255) / 255.f, FRAND_RANGE(0, 255) / 255.f, FRAND_RANGE(0, 255) / 255.f, 1.f)));
	world.boids.push_back(Boid());
	*/
	/*for (int i = 0; i < 200; ++i) {
		world.boids.push_back(Boid());
	}*/
	world.boidSwarm.push_back(Swarm(30));
	world.boidSwarm.push_back(Swarm(35));

	world.cube = LivingCube(200, 200, 200);

	world.bridges.push_back(Bridge({0, 20, 20}, { 0, 20, 40 }, {0, 40, 25}, BridgeClass::TriangularPrism, {1, 0, 0}, 15, 5.5)); // not going to be able to add ramps with this technique but oh well, maybe just spawn them in.
	world.bridges.push_back(Bridge({ 20, 40, 0 }, { 40, 40, 0 }, { 20, 60, 0 }, BridgeClass::Box, { 0, 0, 1 }, 4, 10.5));

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
	
	/*
	world.quads[0].Translate(0, 0.f, 0);
	world.quads[1].Translate(120.f, 0, 0.f);
	world.quads[2].Translate(50.f, 0, -50.f);
	world.quads[3].Translate(50.f, 40, 0);
	world.quads[4].Translate(70.f, 0, 80.f);
	world.quads[5].Translate(170.f, 25, 0.f);

	//q1.Scale(50, 50, 0);
	//world.quads[0].Rotate(30, 0, 0);
	world.quads[0].Rotate(90, 0, 0);
	world.quads[1].Rotate(30, 0, 0);
	world.quads[2].Rotate(30, 0, 0);
	world.quads[3].Rotate(90, 0, 0);

	world.quads[4].Rotate(-30, 0, 0);
	world.quads[5].Rotate(0, -90, 0);
	world.quads[5].Rotate(-240, 0, 0);
	*/
	/*
	for (Boid& b : world.boids) {
		b.Translate(FRAND_RANGE(0.f, 200.f), FRAND_RANGE(0.f, 40.f), FRAND_RANGE(-50.f, 80.f));
	}

	world.boids[0].Translate(0, 5.f, 0);*/

	world.player.SetPosition({ 10.f, 2.5f, 0 });
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

	float deltaSeconds = (deltaTime / 1000.f) * SLOW_TIME;
	
	//world.quads[5].Rotate(10.f * deltaSeconds, 0, 0);
	frames++;
	
	//------------------------------------------------------------------------
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
		
		float degX = (-diff.y / static_cast<float>(APP_INIT_WINDOW_HEIGHT)) * world.player.GetMouseSensitivty(); // Around x axis, we care about mouse Y
		float degY = (-diff.x / static_cast<float>(APP_INIT_WINDOW_WIDTH)) * world.player.GetMouseSensitivty(); // Around y axis we care about mouse X
		glutWarpPointer(WINDOW_WIDTH / 2, (int)std::ceil(WINDOW_HEIGHT / 2));
		snprintf(textBufferB, sizeof(textBufferB), "Warped Mouse: (%f, %f)", mousePos.x, mousePos.y);

		world.player.RotateXY(degX, degY);
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

	if (App::GetController().CheckButton(App::BTN_X, false) && world.player.GetMoveState() != MovementState::AIR) {
		// add an impulse to the y velocity, and set player as in air.
		world.player.UpdateVelocity(Vec3<float>(0.f, MovementSystem::JUMP_IMPULSE, 0.f));
		world.player.TransitionMoveState(MovementState::AIR);
	}
	else if (App::IsKeyPressed(App::KEY_SPACE) && world.player.GetMoveState() != MovementState::AIR) { // same for keyboard
		world.player.UpdateVelocity(Vec3<float>(0.f, MovementSystem::JUMP_IMPULSE, 0.f));
		world.player.TransitionMoveState(MovementState::AIR);
	}

	float camRotY = 0, camRotX = 0; // around the axis
	camRotY -= App::GetController().GetRightThumbStickX() * world.player.GetControllerSensitivty() * (deltaSeconds);
	camRotX += App::GetController().GetRightThumbStickY() * world.player.GetControllerSensitivty() * (deltaSeconds);
	world.player.RotateXY(camRotX, camRotY);

	MovementSystem::HandlePlayerMovement(world.player, Vec3<float>(moveRight, 0, moveForward), deltaSeconds);

	/*for (Boid& b : world.boids) {
		b.Update(world.boids, world.player.GetPosition(), deltaSeconds);
		Vec3<float> t = b.GetPosition();
		
		if (t.x > 50.f) {
			t.x = 1.f;
		}
		else if (t.x < 0.f) {
			t.x = 49.f;
		} if (t.y > 50.f) {
			t.y = 1.f;
		} else if (t.y < 0.f) {
			t.y = 49.f;
		} if (t.z > 50.f || t.z < -50.f) {
			t.z = -t.z;
			//b.SetPosition(FRAND_RANGE(0.f, 200.f), FRAND_RANGE(0.f, 40.f), FRAND_RANGE(-50.f, 80.f));
		}

		b.SetPosition(t.x, t.y, t.z);
	}*/

	for (Bridge& b : world.bridges) {
		b.Update(deltaSeconds, (Vec3<float>(world.cube.GetWidth(), world.cube.GetHeight(), world.cube.GetDepth()) * b.GetTranslateAxis()).GetMagnitude());
	}

	for (Swarm& s : world.boidSwarm) {
		s.Update(world.player.GetPosition(), deltaSeconds);
	}

	// collision detection section
	Collision::ResolvePlayerCollision(world, deltaSeconds);


	auto newVel = world.player.GetVelocity();
	if (world.player.GetMoveState() == MovementState::GROUND && newVel.y < 0 && moveRight == 0 && moveForward == 0) {
		// hacky solution but necessary for ramps to stand on If this doesn't work move on, we don't necessarily need ramps.
		// lets also cut off the decimals here.

		if (std::fabsf(newVel.x) < 0.3f) {
			newVel.x = 0;//std::roundf(newVel.x);
			newVel.z = 0;
		}
		newVel.y = 0.f;
		world.player.SetVelocity(newVel);
		// lets also cut off the decimals here.
	}

	// update position.
	world.player.UpdatePosition(world.player.GetVelocity() * deltaSeconds);

	
	if (world.player.GetPosition().y < -50.5f) {
		world.player.SetVelocity({ 0, 0, 0 });
		world.player.SetPosition({ 10, 10, 10 });
	}

}

//------------------------------------------------------------------------
// Add your display calls here (DrawLine,Print, DrawSprite.) 
// See App.h 
//------------------------------------------------------------------------
void Render()
{
	for (Wall& w : world.cube.GetWalls()) {
		p.Render(w.baseVerts, {}, world.player.GetViewMatrix(), w.col);
	}
	//world.render();
	for (Quad& q : world.quads) {
		p.Render(q.GetVertices(), q.GetModelMatrix(), world.player.GetViewMatrix(), q.GetColour());
	}

	for (Bridge& b : world.bridges) {
		if (b.state == BridgeState::Inactive) {
			continue;
		}

		p.Render(b.GetVertices(), {}, world.player.GetViewMatrix(), { 1.f, 1.f, 1.f, 1.f });
	}
	/*
	p.Render(world.quads[0].GetVertices(), world.quads[0].GetModelMatrix(), world.player.GetViewMatrix(), Colour(3 / 255.f, 219 / 255.f, 252 / 255.f, 1.f));
	p.Render(world.quads[1].GetVertices(), world.quads[1].GetModelMatrix(), world.player.GetViewMatrix(), Colour(161 / 255.f, 57 / 255.f, 230 / 255.f, 1.f));
	p.Render(world.quads[2].GetVertices(), world.quads[2].GetModelMatrix(), world.player.GetViewMatrix(), Colour(3 / 255.f, 219 / 255.f, 252 / 255.f, 1.f));
	p.Render(world.quads[3].GetVertices(), world.quads[3].GetModelMatrix(), world.player.GetViewMatrix(), Colour(161 / 255.f, 57 / 255.f, 230 / 255.f, 1.f));
	p.Render(world.quads[4].GetVertices(), world.quads[4].GetModelMatrix(), world.player.GetViewMatrix(), Colour(1.f, 57 / 255.f, 18 / 255.f, 1.f));
	p.Render(world.quads[5].GetVertices(), world.quads[5].GetModelMatrix(), world.player.GetViewMatrix(), Colour(1.f, 1.f, 1.f, 1.f));*/
	/*
	for (Boid& b : world.boids) {
		p.Render(b.GetVertices(), b.GetModelMatrix(), world.player.GetViewMatrix(), b.GetColour());
	}*/

	for (Swarm& s : world.boidSwarm) {
		for (Boid& b : s.GetBoids()) {
			p.Render(b.GetVertices(), b.GetModelMatrix(), world.player.GetViewMatrix(), b.GetColour());
		}
	}

	//p.Render(world.boids[0].GetVertices(), world.boids[0].GetModelMatrix(), world.player.GetViewMatrix(), world.boids[0].GetColour());
	
	/* MAIN PIPELINE */
	//p.Render(bunnyVerts, bunnyIndices, ma);
	//p.Render(foxVerts, foxIndices, ma);
	/* TESSELLATED PIPELINE */ // Don't forget I need to share cameras.
	//p.Render(fox.GetMeshes(), fox.GetModelMatrix(), fox.GetAdjacencyTable());
	//p.Render(quadMeshes, q.GetModelMatrix(), quadEdges);
	//p.Render(q2Meshes, q2.GetModelMatrix(), q2Edges);
	// DONE!!!
	
	char textBuffer[64];
	snprintf(textBuffer, sizeof(textBuffer), "Pos: (%f, %f, %f)", world.player.GetPosition().x, world.player.GetPosition().y, world.player.GetPosition().z);
	App::Print(10, APP_VIRTUAL_HEIGHT - 20, textBuffer, 0.2f, 1.0f, 0.2f, GLUT_BITMAP_HELVETICA_10);

	snprintf(textBuffer, sizeof(textBuffer), "Vel: (%f, %f, %f)", world.player.GetVelocity().x, world.player.GetVelocity().y, world.player.GetVelocity().z);
	App::Print(10, APP_VIRTUAL_HEIGHT - 40, textBuffer, 0.2f, 1.0f, 0.2f, GLUT_BITMAP_HELVETICA_10);

	App::Print(10, APP_VIRTUAL_HEIGHT - 120, textBufferA, 0.2f, 1.0f, 0.2f, GLUT_BITMAP_HELVETICA_10);
	App::Print(10, APP_VIRTUAL_HEIGHT - 140, textBufferB, 0.2f, 1.0f, 0.2f, GLUT_BITMAP_HELVETICA_10);

	std::string stateText = "GROUND";
	if (world.player.GetMoveState() == MovementState::AIR) {
		stateText = "AIR";
	} else if (world.player.GetMoveState() == MovementState::GRAPPLEHOOK) {
		stateText = "GRAPPLEHOOK";
	}

	snprintf(textBuffer, sizeof(textBuffer), "State: %s", stateText.c_str());
	App::Print(10, APP_VIRTUAL_HEIGHT - 60, textBuffer, 0.2f, 1.0f, 0.2f, GLUT_BITMAP_HELVETICA_10);

	// Camera will use a fixed sensitivity (that in settings can be changed), while mouse will use raw input given + sensitivity for our camera input.
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
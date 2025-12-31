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


Pipeline& p = Pipeline::GetInstance();

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
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{
	/*alignas(16) float o[4] = {1.f, 2.f, 3.f, 4.f};
	__m128 veca = _mm_load_ps(o);*/ // SIMD for SSE x86, need to worry about mac os as well.

	moveForward = true;

	q.Translate(0, 0, -2);

	for (int i = 0; i < 1966; ++i) {
		indices.push_back(i);
		randV.push_back(Vertex(1, 2, 3));
	}


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
	}
	if (App::GetController().CheckButton(App::BTN_DPAD_DOWN, false))
	{
		testSprite->SetScale(testSprite->GetScale() - 0.1f);
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

	float t = 0.45 / deltaTime;
	t = (moveForward) ? t : (-t);

	if (q.GetTranslation().z < -5) {
		t *= 2.5;
	}

	if (q.GetTranslation().z > -0.55 && t > 0) {
		moveForward = false;
	}
	else if (q.GetTranslation().z < -10 && t < 0) {
		moveForward = true;
	}

	q.Translate(0, 0, t);

}

//------------------------------------------------------------------------
// Add your display calls here (DrawLine,Print, DrawSprite.) 
// See App.h 
//------------------------------------------------------------------------
void Render()
{
	//v1.SetColour(0, 0, 0, 255);
	//p.Render(std::vector<Vertex>({v1, v2, v3, v4}), std::vector<uint32_t>({0, 1, 2, 1, 3, 2}), ModelAttributes());
	p.Render(q.GetVertices(), q.GetVertexIds(), q.GetModelAttributes());
	//App::DrawTriangle(600.0f, 300.0f, -0.5f, 1, 650.0f, 400.0f, -0.5f, 1, 700.0f, 300.0f, -0.5f, 1, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	//p.Render(randV, indices, ModelAttributes());
	//------------------------------------------------------------------------
	// Example Sprite Code....
	//testSprite->Draw();
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
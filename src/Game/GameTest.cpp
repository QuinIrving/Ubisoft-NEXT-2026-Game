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

//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Called before first update. Do any initial setup here.
//------------------------------------------------------------------------
void Init()
{
	//Mat4<float> m;
	//m.Print();
	float arr[] = { 1,   2,  3,  4,
					5,   6,  7,  8,
					9,  10, 11, 12,
					13, 14, 15, 16 };
	Mat4<float> m1 = Mat4<float>(arr);
	//m1.Print();

	std::array<float, 16> arr1 = { 5,   11,  7,  3,
					-4,   9,  2,  2,
					-8,  16, -9, 1,
					6, 2, 0, 1 };

	Mat4<float> m2 = Mat4<float>(arr1);
	//OutputDebugString("\n\n");
	//m2.Print();

	std::array<Vec4<float>, 4> arr2 = {Vec4<float>(1, 2, 3, 4), Vec4<float>(5, 6, 7, 8), Vec4<float>(9, 10, 11, 12), Vec4<float>(13, 14, 15, 16)};


	Mat4<float> m3 = Mat4<float>(arr2);
	/*m3.Print();
	OutputDebugString("\n\n");
	(m3 * m2).Print();
	OutputDebugString("\n\n");
	m2 *= m3;
	m2.Print();*/

	std::array<Vec4<float>, 4> arrNEW = { Vec4<float>(0.7071, -0.3062, 1.2795, 0), Vec4<float>(1.2247, 0.1768, -0.7392, 0), Vec4<float>(-1.4142, 0.1768, 0.9186, 0), Vec4<float>(0, 0, 0, 1) };
	//auto mNew = Mat4<float>(arrNEW);
	//auto ma = mNew.GetNormalMatrix();
	//mNew.Print();
	//OutputDebugString("\n");
	//ma.Print();

	auto v = Vec4<float>(-276, -42, 127, 1);
	//(v * Mat4<float>::GetIdentity()).Print();
	//OutputDebugString("\n");
	std::array<Vec4<float>, 4> arrTranslate = { Vec4<float>(1, 2, 3, 4), Vec4<float>(-5, 6, 7, 8), Vec4<float>(9, -10, 11, 12), Vec4<float>(13, 14, -15, 16) };
	//(v * Mat4<float>(arrTranslate)).Print();
	//OutputDebugString("\n");
	//v *= Mat4<float>(arrTranslate);
	//v.Print();

	auto mp = Mat4<float>::Scale(v);
	mp.Print();
	Vec4<float> v1 = Vec4<float>(1, 1, 1, 1);
	OutputDebugString("\n");
	//auto f = v1 * v; // interesting conversion to I presume a quaternion, as there isn't a vec4 * vec4 operator overload. -> went with a delete option to prevent it from happening
	(v1 * mp).Print();
	float t = 1;

	/*
	Quaternion q;
	//q.Print();
	Quaternion q1 = Quaternion(0.5f, 1.f, 2.f, 0.97f);
	//q1.Print();
	
	// Quaternion Vec3 axis and angle (axis angle)
	Quaternion q2 = Quaternion(Vec3<float>(1, 0.71, 1.98), PI/3); // SEEMS GOOD!
	 
	// Quaternion 3 rot values (euler angles)
	Quaternion q3 = Quaternion(25, 36, -47);

	// Quaternion from Mat4 rotation matrix
	//Quaternion q4 = Quaternion(m3); // NOT COMPLETE -> implement?
	//q3.Normalize();
	auto v = Vec3<float>(2.f, 3.f, 1.7f);
	q1.Print();
	v *= q1;
	q1.Normalize();
	q1.Print();
	//v.Print();
	OutputDebugString("\n");
	
	v.Print();*/
	//auto p = v * q1;
	//p.Print();

	//q1.Print();
	//q2.Print();
	//q3.Print();
	//q4.Print();

	//auto z = q1.GetDelta();
	//auto w = q1.GetRotationMatrix();
	//w.Print();
	//q1.Print();
	//q2.Print();

	//OutputDebugString("\nResults:\n");
	//(q1 * q2).Print();
	//q2 *= q1;
	//q2.Print();
	//(~q1).Print();
	//q2.Conjugate();
	//q2.Print();


	float a = 1.f;
	/*t2.Print();
	t3.Print();
	t4.Print();
	q.Print();
	m.Print();*/
	// TESTING WORK!
	// Vec2
	/*OutputDebugString("\nVEC3:\n");
	Vec3<float> v3 = Vec3<float>(-2.5f, 9.f, -1.1f);
	v3.Print();
	Vec3<float> v3a = Vec3<float>(5, 5, 5);
	v3a.Print();
	OutputDebugString("\nResults:\n");
	/*auto d = v3.CrossProduct(v3a);
	auto dS = Vec3<float>::CrossProduct(v3, v3a);
	auto b = v3a.CrossProduct(v3);
	auto bS = Vec3<float>::CrossProduct(v3a, v3);* /
	

	v3.CrossProduct(v3a).Print();
	Vec3<float>::CrossProduct(v3, v3a).Print();
	v3a.CrossProduct(v3).Print();
	Vec3<float>::CrossProduct(v3a, v3).Print();


	OutputDebugString("\nVEC4:\n");
	Vec4<float> v4 = Vec4<float>(-2.5f, 9.f, -1.1f, 3.f);
	v4.Print();
	Vec4<float> v4a = Vec4<float>(5, 5, 5, 5);
	OutputDebugString("\nResults:\n");*/


	

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
}

//------------------------------------------------------------------------
// Add your display calls here (DrawLine,Print, DrawSprite.) 
// See App.h 
//------------------------------------------------------------------------
void Render()
{
	//------------------------------------------------------------------------
	// Example Sprite Code....
	testSprite->Draw();
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	// Example Text.
	//------------------------------------------------------------------------
	App::Print(100, 100, "Sample Text");

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
		App::DrawLine(sx, sy, ex, ey, r, g, b);
	}

	//------------------------------------------------------------------------
	// Example Triangle Drawing.
	//------------------------------------------------------------------------
	App::DrawTriangle(600.0f, 300.0f, -1, 1, 650.0f, 400.0f, 0, 1, 700.0f, 300.0f, 0, 1, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	App::DrawTriangle(500.0f, 300.0f, 0, 1, 550.0f, 450.0f, 0.5, 1, 700.0f, 340.0f, -0.5, 1, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	//App::DrawTriangle(800.0f, 300.0f, 0, 1, 850.0f, 400.0f, 0, 1, 900.0f, 300.0f, 0, 1, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, true);
	App::DrawTriangle(800.0f, 300.0f, 0, 1, 850.0f, 400.0f, 0, 1, 900.0f, 300.0f, 0, 1, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, true);
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
//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: main.cpp
// 
// Author: William Cheung
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "d3dUtility.h"
#include "psystem.h"
#include "camera.h"
#include <cstdlib>
#include <ctime>

#include "cube.h"
#include "snowman.h"
#include "terrain.h"

#include <cstdio>

//
// Globals
//

IDirect3DDevice9*     Device = 0; 

const int Width  = 800;
const int Height = 600;

psys::PSystem* Sno = 0;

Camera TheCamera(Camera::AIRCRAFT);

bool   IsOrbiting = false;  // is the camera orbiting

HWND   HWnd       = NULL;
bool   IsMousing  = false;  // are we using mouse to control

bool DisplayBasicScene(IDirect3DDevice9* device);

//
// Framework Functions
//
bool Setup()
{
	// seed random number generator
	srand((unsigned int)time(0));

	//
	// Create Snow System.
	//

	d3d::BoundingBox boundingBox;
	boundingBox._min = D3DXVECTOR3(-50.0f, -20.0f, -50.0f);
	boundingBox._max = D3DXVECTOR3( 50.0f,  50.0f,  50.0f);
	Sno = new psys::Snow(&boundingBox, 6000);
	Sno->init(Device, "snowflake.dds");

	//
	// Create basic scene.
	//
	DisplayBasicScene(Device);

	//
	// Set projection matrix.
	//
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
			&proj,
			D3DX_PI / 4.0f, // 45 - degree
			(float)Width / (float)Height,
			1.0f,
			5000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);

	return true;
}

void HandleKeyboardInput(float timeDelta) {

	if (!IsOrbiting) {
		if (::GetAsyncKeyState('W') & 0x8000f)
			TheCamera.walk(4.0f * timeDelta);

		if (::GetAsyncKeyState('S') & 0x8000f)
			TheCamera.walk(-4.0f * timeDelta);

		if (::GetAsyncKeyState('A') & 0x8000f)
			TheCamera.strafe(-4.0f * timeDelta);

		if (::GetAsyncKeyState('D') & 0x8000f)
			TheCamera.strafe(4.0f * timeDelta);

		if (::GetAsyncKeyState('R') & 0x8000f)
			TheCamera.fly(4.0f * timeDelta);

		if (::GetAsyncKeyState('F') & 0x8000f)
			TheCamera.fly(-4.0f * timeDelta);
	}

	if (::GetAsyncKeyState('X') & 0x8000f) // free the camera
		IsOrbiting = false;

	if (::GetAsyncKeyState(VK_UP) & 0x8000f)
		TheCamera.pitch(1.0f * timeDelta);

	if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
		TheCamera.pitch(-1.0f * timeDelta);

	if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
		TheCamera.yaw(-1.0f * timeDelta);

	if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
		TheCamera.yaw(1.0f * timeDelta);
}

void HandleMouseInput() {

	static POINT lastMousePosition;

	POINT currMousePosition;

	GetCursorPos(&currMousePosition);
	ScreenToClient(HWnd, &currMousePosition);

	if (IsMousing)
	{
		int nXDiff = (currMousePosition.x - lastMousePosition.x);
		int nYDiff = (currMousePosition.y - lastMousePosition.y);

		D3DXVECTOR3 vRight, vLook, vUp;
		TheCamera.getRight(&vRight), TheCamera.getLook(&vLook), TheCamera.getUp(&vUp);

		D3DXMATRIX matRotation;

		if (nYDiff != 0)
		{
			D3DXMatrixRotationAxis(&matRotation, &vRight, D3DXToRadian((float)nYDiff / 3.0f));
			D3DXVec3TransformCoord(&vLook, &vLook, &matRotation);
			D3DXVec3TransformCoord(&vUp, &vUp, &matRotation);
		}

		if (nXDiff != 0)
		{
			D3DXMatrixRotationAxis(&matRotation, &D3DXVECTOR3(0, 1, 0), D3DXToRadian((float)nXDiff / 3.0f));
			D3DXVec3TransformCoord(&vLook, &vLook, &matRotation);
			D3DXVec3TransformCoord(&vUp, &vUp, &matRotation);
		}

		TheCamera.setRight(&vRight), TheCamera.setLook(&vLook), TheCamera.setUp(&vUp);
	}

	lastMousePosition = currMousePosition;
}

void HandleRealTimeUserInput(float timeDelta)
{
	//
	// Handle mouse input...
	//
	HandleMouseInput();

	//
	// Handle keyboard input...
	//
	HandleKeyboardInput(timeDelta);
}

bool DrawSkybox(IDirect3DDevice9* device, const Camera* camera) 
{
	static Cube skybox(device, "skybox.config", Cube::TEXTYPE_INTERNAL);
	const float skyboxScale = 200.0f;
	const float yOffsetToCamera = 0.0f;

	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	device->LightEnable(0, false);

	D3DXMATRIX P, T, S;
	D3DXMatrixScaling(&S, skyboxScale, skyboxScale, skyboxScale);
	D3DXVECTOR3 cameraPosition;
	TheCamera.getPosition(&cameraPosition);
	D3DXMatrixTranslation(&T, 
		cameraPosition.x, cameraPosition.y + yOffsetToCamera, cameraPosition.z);
	P = S * T;
	skybox.draw(&P, &d3d::WHITE_MTRL);

	return true;
}

//
// Drawing Basic Scene
//
// Pass 0 to device for cleanup
//
bool DisplayBasicScene(IDirect3DDevice9* device)
{
	static bool                    isCreated = false;
	static Snowman*                snowman = 0;
	static Cube*                   crate = 0;
	static Terrain*                terrain = 0;

	static D3DXVECTOR3             lightDirection(-0.5f, -0.5f, -1.0f);
	static D3DXCOLOR               lightColor(1.0f, 1.0f, 1.0f, 1.0f);
	static D3DLIGHT9               light = d3d::InitDirectionalLight(&lightDirection, &lightColor);

	if (device == 0) 
	{
		d3d::Delete<Snowman*>(snowman);
		d3d::Delete<Cube*>(crate);
		d3d::Delete<Terrain*>(terrain);
	}
	else if (!isCreated)
	{
		snowman = new Snowman(device);
		crate = new Cube(device, "crate.config", Cube::TEXTYPE_BOTH_SIDES);

		terrain = new Terrain(device, "castlehm257.raw", 20, 20, 10, 0.05f);
		//D3DXVECTOR3 L = -lightDirection;
		D3DXVECTOR3 L(0.5f, 1.0f, 0.5f);
		terrain->genTexture(&L);

		isCreated = true;
	}
	else
	{
		//
		// Pre-Render Setup
		//
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

		device->SetLight(0, &light);
		device->LightEnable(0, true);
		device->SetRenderState(D3DRS_LIGHTING, TRUE);
		device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		device->SetRenderState(D3DRS_SPECULARENABLE, true);

		//
		// Render
		//

		D3DXMATRIX P, R, T, S, C;

		// draw terrain
		D3DXMatrixTranslation(&T, 0.0f, -12.5f, 0.0f);
		terrain->draw(&T, false);

		// draw crates and snowmen
		D3DXMatrixTranslation(&C, 0.0f, 0.0f, 15.0f);

		D3DXMatrixTranslation(&T, 0.0f, -2.0f, 0.0f);
		P = T * C;
		snowman->draw(&P);  // draw the 1st snowman

		static float fDegree = 0.0f;
		fDegree += 0.001f;
		if (fDegree > 2.0f * D3DX_PI) fDegree = 0.0f;

		D3DXMatrixTranslation(&T, 10.0f, -1.0f, 0.0f);
		D3DXMatrixRotationY(&R, fDegree);
		P = T * R * C;
		crate->draw(&P, &d3d::WHITE_MTRL);

		// collision detection
		d3d::BoundingBox crateBox = crate->getBoundingBox();
		D3DXVECTOR3 cameraPosition;
		TheCamera.getPosition(&cameraPosition);
		D3DXMATRIX X;
		D3DXMatrixInverse(&X, 0, &P);
		D3DXVec3TransformCoord(&cameraPosition, &cameraPosition, &X);
		if (crateBox.isPointInside(cameraPosition)) 
			IsOrbiting = true;

		if (IsOrbiting) { 
			// the camera orbits the 1st snowman
			D3DXMatrixTranslation(&T, 0.0f, 2.0f, 0.0f);
			D3DXVECTOR3 newPosition(0.0f, 0.0f, 0.0f);
			P = P * T;
			D3DXVec3TransformCoord(&newPosition, &newPosition, &P);
			TheCamera.setPosition(&newPosition);
		}

		D3DXMatrixScaling(&S, 0.6, 0.6, 0.6);
		D3DXMatrixTranslation(&T, 10.0f, 0.0f, 0.0f);
		P = S * T * R * C;
		snowman->draw(&P);  // draw the 2nd snowman	
	}
	return true;
}

void Cleanup()
{
	d3d::Delete<psys::PSystem*>(Sno);
	DisplayBasicScene(0);
}

bool Display(float timeDelta)
{
	if( Device )
	{
		//
		// Update the scene:
		//

		HandleRealTimeUserInput(timeDelta); 

		D3DXMATRIX V;
		TheCamera.getViewMatrix(&V);
		Device->SetTransform(D3DTS_VIEW, &V);

		Sno->update(timeDelta);

		//
		// Draw the scene:
		//
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();

		DrawSkybox(Device, &TheCamera);

		DisplayBasicScene(Device);

		// order important, render snow last.
		D3DXMATRIX I;  D3DXMatrixIdentity(&I);
		Device->SetTransform(D3DTS_WORLD, &I);
		Sno->render();

		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}


//
// WndProc
//
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		if( wParam == VK_ESCAPE )
			::DestroyWindow(hwnd);
		break;

	case WM_LBUTTONDOWN:
		IsMousing = true;
		break;
	case WM_LBUTTONUP:
		IsMousing = false;
		break;
	}
		   
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
				   HINSTANCE prevInstance, 
				   PSTR cmdLine,
				   int showCmd)
{
	HWnd = d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device);

	if (!HWnd)
	{
		::MessageBox(0, "InitD3D() - FAILED", 0, 0);
		return 0;
	}
		
	if(!Setup())
	{
		::MessageBox(0, "Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop( Display );

	Cleanup();

	Device->Release();

	return 0;
}

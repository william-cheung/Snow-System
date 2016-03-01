//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: snowman.cpp
// 
// Author: William Cheung
//
// Desc: Provides an interface to create and render a snowman.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "snowman.h"
#include "d3dUtility.h"

Snowman::Snowman(IDirect3DDevice9* device) {
	_device = device;
	D3DXCreateSphere(device, 0.4f, 20, 20, &_head_ball, 0);
	D3DXCreateSphere(device, 0.05f, 20, 20, &_eye_ball, 0);
	D3DXCreateSphere(device, 0.08f, 20, 20, &_nose_ball, 0);
	D3DXCreateSphere(device, 0.6f, 20, 20, &_chest_ball, 0);
	D3DXCreateSphere(device, 1.0f, 20, 20, &_body_ball, 0);
	D3DXCreateCylinder(_device, 0.05f, 0.05f, 1.0f, 20, 20, &_arm_pillar, 0);
}

Snowman::~Snowman() {
	d3d::Release<ID3DXMesh*>(_head_ball);
	d3d::Release<ID3DXMesh*>(_eye_ball);
	d3d::Release<ID3DXMesh*>(_nose_ball);
	d3d::Release<ID3DXMesh*>(_chest_ball);
	d3d::Release<ID3DXMesh*>(_body_ball);
	d3d::Release<ID3DXMesh*>(_arm_pillar);
}

bool Snowman::draw(const D3DXMATRIX* world) {
	D3DXMATRIX P, T, RX, RZ, W;

	if (world) W = *world;
	else D3DXMatrixIdentity(&W);

	_device->SetTexture(0, 0);
	_device->SetMaterial(&d3d::WHITE_MTRL);

	// draw body
	D3DXMatrixTranslation(&T, 0.0f, 1.0f, 0.0f);
	P = T * W;
	_device->SetTransform(D3DTS_WORLD, &P);
	_body_ball->DrawSubset(0); 

	// draw chest
	D3DXMatrixTranslation(&T, 0.0f, 2.0f, 0.0f);
	P = T * W;
	_device->SetTransform(D3DTS_WORLD, &P);
	_chest_ball->DrawSubset(0);

	// draw head
	D3DXMatrixTranslation(&T, 0.0f, 2.8f, 0.0f);
	P = T * W;
	_device->SetTransform(D3DTS_WORLD, &P);
	_head_ball->DrawSubset(0);

	// draw eyes
	_device->SetMaterial(&d3d::BLUE_MTRL);

	D3DXMatrixTranslation(&T, 0.1f, 2.84f, -0.38f);
	P = T * W;
	_device->SetTransform(D3DTS_WORLD, &P);
	_eye_ball->DrawSubset(0);
	D3DXMatrixTranslation(&T, -0.1f, 2.84f, -0.38f);
	P = T * W;
	_device->SetTransform(D3DTS_WORLD, &P);
	_eye_ball->DrawSubset(0);

	// draw nose
	_device->SetMaterial(&d3d::RED_MTRL);
	D3DXMatrixTranslation(&T, 0.0f, 2.74f, -0.38f);
	P = T * W;
	_device->SetTransform(D3DTS_WORLD, &P);
	_nose_ball->DrawSubset(0);


	// draw arms
	_device->SetMaterial(&d3d::GREEN_MTRL);
	D3DXMatrixRotationX(&RX, -D3DX_PI / 2.0f);
	
	D3DXMatrixRotationZ(&RZ, -D3DX_PI / 5.0f);
	D3DXMatrixTranslation(&T, 0.6f, 2.4f, 0.0f);
	P = RX * RZ * T * W;
	_device->SetTransform(D3DTS_WORLD, &P);
	_arm_pillar->DrawSubset(0);
	D3DXMatrixRotationZ(&RZ, D3DX_PI / 5.0f);
	D3DXMatrixTranslation(&T, -0.6f, 2.4f, 0.0f);
	P = RX * RZ * T * W;
	_device->SetTransform(D3DTS_WORLD, &P);
	_arm_pillar->DrawSubset(0);

	return true;
}
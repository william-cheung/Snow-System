//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: snowman.h
// 
// Author: William Cheung
//
// Desc: Provides an interface to create and render a snowman.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __snowmanH__
#define __snowmanH__

#include <d3dx9.h>
#include <vector>

class Snowman {
public:
	Snowman(IDirect3DDevice9* device);
	~Snowman();
	bool draw(const D3DXMATRIX* world);
private:
	IDirect3DDevice9*        _device;
	ID3DXMesh*               _head_ball;
	ID3DXMesh*               _chest_ball;
	ID3DXMesh*               _body_ball;
	ID3DXMesh*               _eye_ball;
	ID3DXMesh*               _nose_ball;
	ID3DXMesh*               _arm_pillar;
};

#endif  // __snowmanH__
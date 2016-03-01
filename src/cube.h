//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: cube.h
// 
// Author: William Cheung
//
// Desc: Provides an interface to create and render a cube.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __cubeH__
#define __cubeH__

#include <d3dx9.h>
#include <string>

#include "d3dUtility.h"

class Cube
{
public:
	enum TexturingType { TEXTYPE_BOTH_SIDES, TEXTYPE_EXTERNAL, TEXTYPE_INTERNAL };
	Cube(IDirect3DDevice9* device, 
		std::string texConfig = "", TexturingType texType = TEXTYPE_EXTERNAL);
	~Cube();
	bool draw(const D3DXMATRIX* world, const D3DMATERIAL9* mtrl);

protected:
	class Face {
	public: 
		Face(IDirect3DDevice9* device, 
			d3d::Vertex vertices[4], IDirect3DTexture9* texture);
		~Face();

		bool draw();
	private:
		IDirect3DDevice9*        _device;
		IDirect3DVertexBuffer9*  _vertexBuffer;
		IDirect3DIndexBuffer9*   _indexBuffer;
		IDirect3DTexture9*       _texture;
	};

private:
	IDirect3DDevice9*     _device;
	Face*                 _faces[6];
	TexturingType         _texturingType;
};
#endif //__cubeH__
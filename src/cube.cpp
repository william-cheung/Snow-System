//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: cube.cpp
// 
// Author: William Cheung
//
// Desc: Provides an interface to create and render a cube.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#include <fstream>
#include <sstream>
#include <map>

#include "cube.h"
using d3d::Vertex;

static void parseTexConfig(std::string config, 
	std::map<std::string, std::string>& mapping) {
	std::ifstream fin(config);
	char buffer[256];
	mapping.clear();
	while (fin.getline(buffer, 256)) {
		std::string line(buffer), key, sep, value;
		std::istringstream iss(line);
		iss >> key >> sep >> value;
		mapping[key] = value;
	}
}

Cube::Cube(IDirect3DDevice9* device, std::string texConfig, TexturingType texType)
{
	// save a ptr to the device
	_device = device;

	std::string facenames[] = { "Front", "Back", "Top", "Bottom", "Left", "Right" };
	Vertex vertices[6][4];
	
	std::ifstream fin;
	if (texType == Cube::TEXTYPE_INTERNAL)
		fin = std::ifstream("cube-intern.data");
	else
		fin = std::ifstream("cube-extern.data");

	// fill in vertex data of all faces
	for (int iface = 0; iface < 6; iface++) {
		for (int ivrtx = 0; ivrtx < 4; ivrtx++) {
			float x, y, z, nx, ny, nz, u, v;
			fin >> x >> y >> z >> nx >> ny >> nz >> u >> v;
			vertices[iface][ivrtx] = Vertex(x, y, z, nx, ny, nz, u, v);
		}
	}

	std::map<std::string, std::string> texFileMap;
	parseTexConfig(texConfig, texFileMap);

	for (int iface = 0; iface < 6; iface++) {
		IDirect3DTexture9* tex = 0;
		D3DXCreateTextureFromFile(
			_device,
			texFileMap[facenames[iface]].c_str(),
			&tex);
		_faces[iface] = new Face(_device, vertices[iface], tex);
	}
}

Cube::~Cube()
{
	for (int iface = 0; iface < 6; iface++) {
		if (_faces[iface])
			delete _faces[iface];
	}
}

d3d::BoundingBox Cube::getBoundingBox() const {
	d3d::BoundingBox box;
	box._min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	box._max = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	return box;
}

bool Cube::draw(const D3DXMATRIX* world, const D3DMATERIAL9* mtrl)
{
	if (world)
		_device->SetTransform(D3DTS_WORLD, world);
	else {
		D3DXMATRIX I;  D3DXMatrixIdentity(&I);
		_device->SetTransform(D3DTS_WORLD, &I);
	}

	if (mtrl)
		_device->SetMaterial(mtrl);

	switch (_texturingType) {
	case Cube::TEXTYPE_BOTH_SIDES:
		_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		break;
	case Cube::TEXTYPE_EXTERNAL:
		_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		break;
	case Cube::TEXTYPE_INTERNAL:
		_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		break;
	}

	for (int iface = 0; iface < 6; iface++)
		_faces[iface]->draw();

	_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	_device->SetTexture(0, 0);

	return true;
}

Cube::Face::Face(IDirect3DDevice9* device, 
	d3d::Vertex vertices[4], IDirect3DTexture9* tex) {
	_device = device;

	_device->CreateVertexBuffer(
		4 * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&_vertexBuffer,
		0);

	Vertex* v;
	_vertexBuffer->Lock(0, 0, (void**)&v, 0);

	for (int i = 0; i < 4; i++)
		v[i] = vertices[i];
	
	_vertexBuffer->Unlock();

	_device->CreateIndexBuffer(
		6 * sizeof(WORD),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&_indexBuffer,
		0);

	WORD* i = 0;
	_indexBuffer->Lock(0, 0, (void**)&i, 0);
	
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	_indexBuffer->Unlock();

	_texture = tex;
}

Cube::Face::~Face() {
	d3d::Release<IDirect3DVertexBuffer9*>(_vertexBuffer);
	d3d::Release<IDirect3DIndexBuffer9*>(_indexBuffer);
	d3d::Release<IDirect3DTexture9*>(_texture);
}

bool Cube::Face::draw() {
	_device->SetTexture(0, _texture);
	_device->SetStreamSource(0, _vertexBuffer, 0, sizeof(Vertex));
	_device->SetIndices(_indexBuffer);
	_device->SetFVF(Vertex::FVF);
	_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	return true;
}

/*

// fill in the front face vertex data
vertices[0][0] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
vertices[0][1] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
vertices[0][2] = Vertex(1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
vertices[0][3] = Vertex(1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);

// fill in the back face vertex data
vertices[1][0] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
vertices[1][1] = Vertex(1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
vertices[1][2] = Vertex(1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
vertices[1][3] = Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);

// fill in the top face vertex data
vertices[2][0] = Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
vertices[2][1] = Vertex(-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
vertices[2][2] = Vertex(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
vertices[2][3] = Vertex(1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);

// fill in the bottom face vertex data
vertices[3][0] = Vertex(-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
vertices[3][1] = Vertex(1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
vertices[3][2] = Vertex(1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
vertices[3][3] = Vertex(-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

// fill in the left face vertex data
vertices[4][0] = Vertex(-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
vertices[4][1] = Vertex(-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
vertices[4][2] = Vertex(-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
vertices[4][3] = Vertex(-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

// fill in the right face vertex data
vertices[5][0] = Vertex(1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
vertices[5][1] = Vertex(1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
vertices[5][2] = Vertex(1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
vertices[5][3] = Vertex(1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

*/
#include "Character.h"
#include "Renderer.h"


Character::Character(char* fileName)
{
	model = new Mesh(Renderer::m_pD3DDevice, Renderer::m_pImmediateContext);
	model->LoadObjModel(fileName);
}


Character::~Character()
{
}

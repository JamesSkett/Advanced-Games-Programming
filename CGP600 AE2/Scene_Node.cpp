#include "Scene_Node.h"



Scene_Node::Scene_Node()
{
	m_pModel = NULL;

	m_x = 0.0f; m_y = 0.0f; m_z = 0.0f;
	m_xangle = 0.0f; m_yangle = 0.0f; m_zangle = 0.0f;
	m_scale = 1.0f;
}


Scene_Node::~Scene_Node()
{
}

//Getters
float Scene_Node::GetXPos()
{
	return m_x;
}
float Scene_Node::GetYPos()
{
	return m_y;
}
float Scene_Node::GetZPos()
{
	return m_z;
}
float Scene_Node::GetXAngle()
{
	return m_xangle;
}
float Scene_Node::GetYAngle()
{
	return m_yangle;
}
float Scene_Node::GetZAngle()
{
	return m_zangle;
}

float Scene_Node::GetScale()
{
	return m_scale;
}

//Setters
void Scene_Node::SetXPos(float x)
{
	m_x = x;
}
void Scene_Node::SetYPos(float y)
{
	m_y = y;
}
void Scene_Node::SetZPos(float z)
{
	m_z = z;
}
void Scene_Node::SetXAngle(float xAngle)
{
	m_xangle = xAngle;
}
void Scene_Node::SetYAngle(float yAngle)
{
	m_yangle = yAngle;
}
void Scene_Node::SetZAngle(float zAngle)
{
	m_zangle = zAngle;
}

void Scene_Node::SetScale(float scale)
{
	m_scale = scale;
}

void Scene_Node::UpdateXPos(float distance)
{
	m_x = +distance;
}

void Scene_Node::UpdateYPos(float distance)
{
	m_y += distance;
}

void Scene_Node::UpdateZPos(float distance)
{
	m_z += distance;
}

void Scene_Node::UpdateXangle(float angle)
{
	m_xangle += angle;
}

void Scene_Node::UpdateYangle(float angle)
{
	m_yangle += angle;
}

void Scene_Node::UpdateZangle(float angle)
{
	m_zangle += angle;
}

void Scene_Node::SetModel(Mesh * mesh)
{
	m_pModel = mesh;
}

void Scene_Node::AddChildNode(Scene_Node * n)
{
	m_children.push_back(n);
}

bool Scene_Node::DetachNode(Scene_Node * n)
{
	for (int i = 0; i < m_children.size(); i++)
	{
		if (n == m_children[i])
		{
			m_children.erase(m_children.begin() + i);
			return true;
		}

		if (m_children[i]->DetachNode(n) == true) return true;
	}
	return false;
}

void Scene_Node::Execute(XMMATRIX * world, XMMATRIX * view, XMMATRIX * projection)
{

	XMMATRIX local_world = DirectX::XMMatrixIdentity();

	local_world = DirectX::XMMatrixRotationX(XMConvertToRadians(m_xangle));
	local_world *= DirectX::XMMatrixRotationY(XMConvertToRadians(m_yangle));
	local_world *= DirectX::XMMatrixRotationZ(XMConvertToRadians(m_zangle));

	local_world *= DirectX::XMMatrixScaling(m_scale, m_scale, m_scale);

	local_world *= DirectX::XMMatrixTranslation(m_x, m_y, m_z);

	//passed in world matrix contains concatenated transformations of all 
	//parent nodes so that this nodes transformations are relative to those
	local_world *= *world;

	// only draw if there is a model attached
	if (m_pModel) m_pModel->Draw(&local_world, view, projection);

	// traverse all child nodes, passing in the concatenated world matrix
	for (int i = 0; i< m_children.size(); i++)
	{
		m_children[i]->Execute(&local_world, view, projection);
	}

}

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

bool Scene_Node::UpdateXPos(float distance, Scene_Node* rootNode)
{
	float old_x = m_x;	// save current state 
	m_x += distance;		// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	rootNode->UpdateCollisionTree(&identity, 1.0f);

	// check for collision of this node (and children) against all other nodes
	if (CheckCollision(rootNode) == true)
	{
		// if collision restore state
		m_x = old_x;

		return true;
	}

	return false;

}

bool Scene_Node::UpdateYPos(float distance, Scene_Node* rootNode)
{
	float old_y = m_y;	// save current state 
	m_y += distance;		// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	rootNode->UpdateCollisionTree(&identity, 1.0f);

	// check for collision of this node (and children) against all other nodes
	if (CheckCollision(rootNode) == true)
	{
		// if collision restore state
		m_y = old_y;

		return true;
	}

	return false;
}

bool Scene_Node::UpdateZPos(float distance, Scene_Node* rootNode)
{
	float old_z = m_z;	// save current state 
	m_z += distance;		// update state

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	rootNode->UpdateCollisionTree(&identity, 1.0f);

	// check for collision of this node (and children) against all other nodes
	if (CheckCollision(rootNode) == true)
	{
		// if collision restore state
		m_z = old_z;

		return true;
	}

	return false;
}

bool Scene_Node::UpdateXangle(float angle, Scene_Node* rootNode)
{
	float old_xangle = m_xangle;
	m_xangle += angle;

	m_dx = sinf(XMConvertToRadians(m_yangle));
	//m_dy = atan(XMConvertToRadians(m_xangle));
	m_dz = cosf(XMConvertToRadians(m_yangle));

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	rootNode->UpdateCollisionTree(&identity, 1.0f);

	// check for collision of this node (and children) against all other nodes
	if (CheckCollision(rootNode) == true)
	{
		// if collision restore state
		m_xangle = old_xangle;

		return true;
	}

	return false;
}

bool Scene_Node::UpdateYangle(float angle, Scene_Node* rootNode)
{
	float old_yangle = m_yangle;
	m_yangle += angle;

	m_dx = sinf(XMConvertToRadians(m_yangle));
	//m_dy = atan(XMConvertToRadians(m_xangle));
	m_dz = cosf(XMConvertToRadians(m_yangle));

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	rootNode->UpdateCollisionTree(&identity, 1.0f);

	// check for collision of this node (and children) against all other nodes
	if (CheckCollision(rootNode) == true)
	{
		// if collision restore state
		m_yangle = old_yangle;

		return true;
	}

	return false;
}

bool Scene_Node::UpdateZangle(float angle, Scene_Node* rootNode)
{
	float old_zangle = m_zangle;
	m_zangle += angle;

	m_dx = sinf(XMConvertToRadians(m_yangle));
	//m_dy = atan(XMConvertToRadians(m_xangle));
	m_dz = cosf(XMConvertToRadians(m_yangle));

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	rootNode->UpdateCollisionTree(&identity, 1.0f);

	// check for collision of this node (and children) against all other nodes
	if (CheckCollision(rootNode) == true)
	{
		// if collision restore state
		m_zangle = old_zangle;

		return true;
	}

	return false;
}

bool Scene_Node::MoveForward(float speed, Scene_Node* rootNode)
{
	float old_z = m_z;	// save current state 
	float old_x = m_x;

	m_x += m_dx * speed;
	//m_y += m_dy * speed;
	m_z += m_dz * speed;

	XMMATRIX identity = XMMatrixIdentity();

	// since state has changed, need to update collision tree
	// this basic system requires entire hirearchy to be updated
	// so start at root node passing in identity matrix
	rootNode->UpdateCollisionTree(&identity, 1.0f);

	// check for collision of this node (and children) against all other nodes
	if (CheckCollision(rootNode) == true)
	{
		// if collision restore state
		m_z = old_z;
		m_x = old_x;

		return true;
	}

	return false;
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
	for (unsigned int i = 0; i < m_children.size(); i++)
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

	local_world *= DirectX::XMMatrixTranslation(m_x + m_dx, m_y + m_dy, m_z + m_dy);

	//passed in world matrix contains concatenated transformations of all 
	//parent nodes so that this nodes transformations are relative to those
	local_world *= *world;

	if (m_canDraw == true)
	{
		// only draw if there is a model attached
		if (m_pModel) m_pModel->Draw(&local_world, view, projection);
	}
	

	// traverse all child nodes, passing in the concatenated world matrix
	for (unsigned int i = 0; i< m_children.size(); i++)
	{
		m_children[i]->Execute(&local_world, view, projection);
	}

	//UpdateCollisionTree(world, m_scale);

}

XMVECTOR Scene_Node::GetWorldCentrePos()
{
	return XMVectorSet(m_world_centre_x, m_world_centre_y, m_world_centre_z, 0.0f);
}

void Scene_Node::UpdateCollisionTree(XMMATRIX* world, float scale)
{
	// the local_world matrix will be used to calculate the local transformations for this node
	m_local_world_matrix = XMMatrixIdentity();

	m_local_world_matrix = XMMatrixRotationX(XMConvertToRadians(m_xangle));
	m_local_world_matrix *= XMMatrixRotationY(XMConvertToRadians(m_yangle));
	m_local_world_matrix *= XMMatrixRotationZ(XMConvertToRadians(m_zangle));

	m_local_world_matrix *= XMMatrixScaling(m_scale, m_scale, m_scale);

	m_local_world_matrix *= XMMatrixTranslation(m_x, m_y, m_z);

	// the local matrix is multiplied by the passed in world matrix that contains the concatenated
	// transformations of all parent nodes so that this nodes transformations are relative to those
	m_local_world_matrix *= *world;

	// calc the world space scale of this object, is needed to calculate the  
	// correct bounding sphere radius of an object in a scaled hierarchy
	m_world_scale = scale * m_scale;

	XMVECTOR v;
	if (this->m_pModel)
	{
		v = XMVectorSet(m_pModel->GetBoundingSphere_x(),
			m_pModel->GetBoundingSphere_y(),
			m_pModel->GetBoundingSphere_z(), 0.0);
	}
	else v = XMVectorSet(0, 0, 0, 0); // no model, default to 0

									  // find and store world space bounding sphere centre
	v = XMVector3Transform(v, m_local_world_matrix);
	m_world_centre_x = XMVectorGetX(v);
	m_world_centre_y = XMVectorGetY(v);
	m_world_centre_z = XMVectorGetZ(v);

	// traverse all child nodes, passing in the concatenated world matrix and scale
	for (unsigned int i = 0; i< m_children.size(); i++)
	{
		m_children[i]->UpdateCollisionTree(&m_local_world_matrix, m_world_scale);
	}



}

bool Scene_Node::CheckCollision(Scene_Node* compare_tree)
{
	return CheckCollision(compare_tree, this);
}


bool Scene_Node::CheckCollision(Scene_Node * compareTree, Scene_Node * objectTreeRoot)
{
	//check to see if root of tree being compared is same as root node of     
	//object tree being checked
	// i.e. stop object node and children being checked against each other
	if (objectTreeRoot == compareTree) return false;

	if (compareTree->m_canCollide == false)
	{
		return false;
	}

	if (m_pModel && compareTree->m_pModel)
	{
		XMVECTOR v1 = GetWorldCentrePos();
		XMVECTOR v2 = compareTree->GetWorldCentrePos();
		XMVECTOR vdiff = v1 - v2;

		float x1 = XMVectorGetX(v1);
		float x2 = XMVectorGetX(v2);
		float y1 = XMVectorGetY(v1);
		float y2 = XMVectorGetY(v2);
		float z1 = XMVectorGetZ(v1);
		float z2 = XMVectorGetZ(v2);

		float dx = x1 - x2;
		float dy = y1 - y2;
		float dz = z1 - z2;

		float distance = sqrt(dx*dx + dy*dy + dz*dz);
		float sumOfRadii = (compareTree->m_pModel->GetBoundingSphereRadius() * compareTree->m_world_scale) + (this->m_pModel->GetBoundingSphereRadius() * m_world_scale);
		
		if (distance <= sumOfRadii)
		{
			return true;
		}

	}

	for (unsigned int i = 0; i< compareTree->m_children.size(); i++)
	{
		//check for collsion against all compared tree child nodes 
		if (CheckCollision(compareTree->m_children[i], objectTreeRoot))
			return true;
	}

	for (unsigned int i = 0; i< m_children.size(); i++)
	{
		if (m_children[i]->CheckCollision(compareTree, objectTreeRoot))
			return true;
	}


	return false;
}

void Scene_Node::setCanCollide(bool canCollide)
{
	m_canCollide = canCollide;
}

void Scene_Node::setCanDraw(bool canDraw)
{
	m_canDraw = canDraw;
}
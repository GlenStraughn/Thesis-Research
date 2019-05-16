//================================================================================//
// Transform.cpp
//--------------------------------------------------------------------------------//
// DESCRIPTION: Transform class to track translation, rotation, and scale
// AUTHOR: Glen K. Straughn
// DATE: 8/31/2017
//================================================================================//

#include"Transform.hpp"
#include"TypeDef.hpp"
#include"Util.hpp"

Transform::Transform()
{
	m_scale = glm::vec3(1.0,1.0,1.0);
}


//---------------------------------------------------------------------------------//


void Transform::setTransform(const glm::mat4x4 &trans)
{
	///*
	glm::vec3 dummySkew; glm::vec4 dummyPerspective;
	//bool notDecomposed = !glm::decompose(trans, m_scale, m_rotation, m_position, dummySkew, dummyPerspective);
	decomposeMatrix(trans, m_position, m_rotation, m_scale);
	
	/*
	if (notDecomposed)
	{
		if (trans == ZERO_MATRIX_4X4)
		{
			m_position = glm::vec3(0);
			m_rotation = glm::quat(0.0, 0.0, 0.0, 0.0);
			m_scale = glm::vec3(0);
		}
	}
	//*/
}


//--------------------------------------------------------------------------------//


void Transform::operator =(const glm::mat4x4& trans)
{
	this->setTransform(trans);

	/*
	glm::vec3 dummySkew; glm::vec4 dummyPerspective;
	bool notDecomposed = !glm::decompose(trans, m_scale, m_rotation, m_position, dummySkew, dummyPerspective);
	m_rotation = m_rotation;
	if (notDecomposed)
	{
		if (trans == ZERO_MATRIX_4X4)
		{
			m_position = glm::vec3(0);
			m_rotation = glm::quat(0.0,0.0,0.0,0.0);
			m_scale = glm::vec3(0);
		}
	}
	*/
	// [break point]
}


//--------------------------------------------------------------------------------//


glm::mat4x4 Transform::operator *(const Transform& tran)
{
	return toMat4x4() * tran.toMat4x4();
}


//--------------------------------------------------------------------------------//


glm::mat4x4 Transform::operator +(const Transform& tran)
{
	return toMat4x4() + tran.toMat4x4();
}


//--------------------------------------------------------------------------------//


void Transform::operator =(const Transform& tran)
{
	m_rotation = tran.getRotation();
	m_position = tran.getTranslation();
	m_scale = tran.getScale();
}
#pragma once
/*
Object.hpp

DESCRIPTION: Class for representing an object in the scene.
AUTHOR: Glen Straughn
DATE: 4/5/2017
COMPILER: Visual Studio 2015
*/

#include <glm/glm.hpp>
#include <vector>

#include "Mesh.hpp"
#include "TypeDef.hpp"
#include "Transform.hpp"

class Object
{
public:
	Object();
	Object(glm::mat4x4 transform) : Object() { m_transform = transform; }

	inline void setTransform(const glm::mat4x4 &transform) { m_transform = transform; }
	inline void setTransform(const Transform &transform) { m_transform = transform; }
	inline Transform getTransform() { return m_transform; }

	inline void setMesh(Mesh* mesh) { mp_mesh = mesh; }
	inline Mesh* getMesh() { return mp_mesh; }

	inline UID getUID() { return m_UID; }

protected:
	Transform m_transform;
	Mesh *mp_mesh;
	UniqueID m_UID;
};

Object::Object()
{
	static UniqueID nextID = 0;
	m_UID = nextID;
	nextID++;
}
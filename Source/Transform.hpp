//================================================================================//
// Transform.hpp
//--------------------------------------------------------------------------------//
// DESCRIPTION: Transform class to track translation, rotation, and scale
// AUTHOR: Glen K. Straughn
// DATE: 8/31/2017
//================================================================================//

#pragma once

#include<glm/glm.hpp>
#include<glm/ext.hpp>
#include<glm/gtx/matrix_decompose.hpp>

class Transform
{
public:
	Transform();

	inline void setTranslation(const glm::vec3 &loc) { m_position = loc; }
	inline void setRotation(const glm::quat &rot) { m_rotation = rot; }
	inline void setScale(const glm::vec3 &scale) { m_scale = scale; }
	void setTransform(const glm::mat4x4 &trans);

	inline glm::vec3	getTranslation() const { return m_position; }
	inline glm::quat	getRotation() const	{ return m_rotation; }
	inline glm::vec3	getRotationEuler() const { return glm::eulerAngles(m_rotation); }
	inline glm::vec3	getScale() const { return m_scale; }
	inline glm::mat4x4	toMat4x4() const { return glm::translate(m_position)*glm::toMat4(m_rotation)*glm::scale(m_scale); }

	inline operator glm::mat4x4() { return glm::translate(m_position)*glm::toMat4(m_rotation)*glm::scale(m_scale); }

	inline glm::mat4x4 operator *(const glm::mat4x4& mat) const { return(toMat4x4()*mat); }
	inline glm::mat4x4 operator +(const glm::mat4x4& mat) const { return(toMat4x4()+mat); }
	void operator =(const glm::mat4x4& mat);

	glm::mat4x4 operator *(const Transform& tran);
	glm::mat4x4 operator +(const Transform& tran);
	void operator =(const Transform& tran);

protected:
	glm::vec3 m_position;	// Translation
	glm::quat m_rotation;	// Quaternian representing 3D rotation
	glm::vec3 m_scale;		// Actually, this one's pretty self-explanatory
};
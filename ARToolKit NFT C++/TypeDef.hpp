//===========================================================================//
// TypeDef.h
//	- TypeDefs and global constants.
//---------------------------------------------------------------------------//
// AUTHOR: Glen Straughn
// DATE: 2017
// COMPILER: Visual C++
//===========================================================================//
#include<glm/glm.hpp>
#include<glm/ext.hpp>

#pragma once

typedef unsigned int UniqueID;
typedef UniqueID UID;

typedef char byte;
typedef unsigned char ubyte;

typedef glm::highp_dmat4x4 doubleMat4x4;
typedef doubleMat4x4 ARPose;
typedef glm::vec4 Color;


// GLOBAL CONSTANTS
static const glm::mat4x4 ZERO_MATRIX_4X4(0.0f, 0.0f, 0.0f, 0.0f,
										 0.0f, 0.0f, 0.0f, 0.0f,
										 0.0f, 0.0f, 0.0f, 0.0f,
										 0.0f, 0.0f, 0.0f, 0.0f);

static const glm::mat4x4 IDENTITY_MATRIX_4X4(1.0f, 0.0f, 0.0f, 0.0f,
											0.0f, 1.0f, 0.0f, 0.0f,
											0.0f, 0.0f, 1.0f, 0.0f,
											0.0f, 0.0f, 0.0f, 1.0f);

static const glm::vec3 UP_VECTOR(0.0, 1.0, 0.0);
static const glm::vec3 FORWARD_VECTOR(0.0, 0.0, -1.0);
static const glm::vec3 RIGHT_VECTOR(1.0, 0.0, 0.0);

const static Color RED = Color(1.0, 0.0, 0.0, 1.0);
const static Color GREEN = Color(0.0, 1.0, 0.0, 1.0);
const static Color BLUE = Color(0.0, 0.0, 1.0, 1.0);
const static Color ORANGE = Color(1.0, 0.5, 0.0, 1.0);
const static Color WHITE = Color(1.0, 1.0, 1.0, 1.0);
const static Color MAGENTA = Color(1.0, 0.0, 1.0, 1.0);


static const float AR_FACE_SCALE_FACTOR = 1.0;
static const float AR_DM_SCALE_FACTOR = 2.5;
#ifndef include_MatrixStack44
#define include_MatrixStack44
#pragma once

//-----------------------------------------------------------------------------------------------
#include <stack>
#include "Matrix44.hpp"


//-----------------------------------------------------------------------------------------------
class MatrixStack44
{
public:
	MatrixStack44();
	void PushMatrix();
	void PopMatrix();
	void MakeIdentity();
	const Matrix44& GetCurrentMatrix() const;
	Matrix44& GetCurrentMatrix();
	void ApplyTranslation( float translateX, float translateY, float translateZ );
	void ApplyRotation( float angleDegrees, float rotateX, float rotateY, float rotateZ );
	void ApplyScale( float scaleX, float scaleY, float scaleZ );
	void ApplyOrtho( float leftCoord, float rightCoord, float bottomCoord, float topCoord, float zNearDistance, float zFarDistance );
	void ApplyPerspective( float fovyDegrees, float aspectRatio, float zNearDistance, float zFarDistance );

	std::stack<Matrix44>	m_matrixStack;
};


#endif
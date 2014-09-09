#include "MatrixStack44.hpp"
#include "NewMacroDef.hpp"


//-----------------------------------------------------------------------------------------------
MatrixStack44::MatrixStack44()
{
	m_matrixStack.push( Matrix44() );
	MakeIdentity();
}


//-----------------------------------------------------------------------------------------------
void MatrixStack44::PushMatrix()
{
	m_matrixStack.push( m_matrixStack.top() );
}


//-----------------------------------------------------------------------------------------------
void MatrixStack44::PopMatrix()
{
	if( m_matrixStack.size() > 1 )
		m_matrixStack.pop();
}


//-----------------------------------------------------------------------------------------------
void MatrixStack44::MakeIdentity()
{
	if( m_matrixStack.empty() )
		return;

	Matrix44 identityMat;
	identityMat.MakeIdentity();

	m_matrixStack.top() = identityMat;
}


//-----------------------------------------------------------------------------------------------
const Matrix44& MatrixStack44::GetCurrentMatrix() const
{
	return m_matrixStack.top();
}


//-----------------------------------------------------------------------------------------------
Matrix44& MatrixStack44::GetCurrentMatrix()
{
	return m_matrixStack.top();
}


//-----------------------------------------------------------------------------------------------
void MatrixStack44::ApplyTranslation( float translateX, float translateY, float translateZ )
{
	Matrix44 translateMatrix;
	translateMatrix.MakeTranslation( translateX, translateY, translateZ );
	m_matrixStack.top().ApplyTransformation( translateMatrix );
}


//-----------------------------------------------------------------------------------------------
void MatrixStack44::ApplyRotation( float angleDegrees, float rotateX, float rotateY, float rotateZ )
{
	Matrix44 rotateMatrix;
	rotateMatrix.MakeRotation( angleDegrees, rotateX, rotateY, rotateZ );
	m_matrixStack.top().ApplyTransformation( rotateMatrix );
}


//-----------------------------------------------------------------------------------------------
void MatrixStack44::ApplyScale( float scaleX, float scaleY, float scaleZ )
{
	Matrix44 scaleMatrix;
	scaleMatrix.MakeScale( scaleX, scaleY, scaleZ );
	m_matrixStack.top().ApplyTransformation( scaleMatrix );
}


//-----------------------------------------------------------------------------------------------
void MatrixStack44::ApplyOrtho( float leftCoord, float rightCoord, float bottomCoord, float topCoord, float zNearDistance, float zFarDistance )
{
	Matrix44 orthoMatrix;
	orthoMatrix.MakeOrtho( leftCoord, rightCoord, bottomCoord, topCoord, zNearDistance, zFarDistance );
	m_matrixStack.top().ApplyTransformation( orthoMatrix );
}


//-----------------------------------------------------------------------------------------------
void MatrixStack44::ApplyPerspective( float fovyDegrees, float aspectRatio, float zNearDistance, float zFarDistance )
{
	Matrix44 perspectiveMatrix;
	perspectiveMatrix.MakePerspective( fovyDegrees, aspectRatio, zNearDistance, zFarDistance );
	m_matrixStack.top().ApplyTransformation( perspectiveMatrix );
}
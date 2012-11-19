//=============================================================================
//                                                                            
//   Exercise code for Introduction to Computer Graphics
//   LGG - EPFL
//   Thibaut Weise, Yuliy Schwartzburg
//                                                                            
//=============================================================================
//=============================================================================
//
//   CLASS Object3D
//
//=============================================================================


#ifndef OBJECT3D_HH
#define OBJECT3D_HH


//== INCLUDES =================================================================

#include "../math/Matrix4.h"

//== CLASS DEFINITION =========================================================





/*  
	 Basic 3D Object
*/

class Object3D
{
  
public:
    //------------------------------------------------ public methods

	//! constructor which sets the object to world transformation to identity
	Object3D()
    {
		m_transformationMatrix.loadIdentity();
	}
	
	//! destructor
	virtual ~Object3D() {}
	
	//! return the transformation from object to world coordinates
	const Matrix4 & getTransformation() const
    {
        return m_transformationMatrix;
    }

	//! set the current transformation
	void setTransformation( const Matrix4 & mat )
    {
        m_transformationMatrix = mat;
    }
	
	//! return origin of object in world coordinates
	Vector3 origin() const {
		return m_transformationMatrix * Vector3(0,0,0);
	}

	//! set transformation matrix to identity
	void setIdentity() {
		m_transformationMatrix.loadIdentity();
	}
	
	//! translates the object in the world coordinate system
	void translateWorld( const Vector3 & _trans )
	{
        Matrix4 translationMatrix = getTranslationMatrix(_trans);
		// ((( Exercise 3.4 )))
		m_transformationMatrix = translationMatrix*m_transformationMatrix;
	}
	
	
	//! translates the object in the object coordinate systems
	void translateObject( const Vector3 & _trans )
	{
        Matrix4 translationMatrix = getTranslationMatrix(_trans);
		// ((( Exercise 3.4 )))
		m_transformationMatrix = m_transformationMatrix*translationMatrix;
	}
	
	//! scales the object in the world coordinate system
	void scaleWorld( const Vector3 & _scl )
	{
        Matrix4 scaleMatrix = getScaleMatrix(_scl);
		// ((( Exercise 3.4 )))
		m_transformationMatrix = scaleMatrix*m_transformationMatrix;
	}

	//! scales the object in the object coordinate systems
	void scaleObject( const Vector3 & _scl )
	{
        Matrix4 scaleMatrix = getScaleMatrix(_scl);
		// ((( Exercise 3.4 )))
		m_transformationMatrix = m_transformationMatrix*scaleMatrix;
	}

	//! rotates the object in the world coordinate system
	void rotateWorld( const Vector3& _axis, float _angle )
	{
        Matrix4 rotationMatrix = getRotationMatrix(_axis,_angle);
		// ((( Exercise 3.4 )))
		
		m_transformationMatrix = rotationMatrix*m_transformationMatrix;
	}

	//! rotates the object in the object coordinate system
	void rotateObject( const Vector3& _axis, float _angle )
	{
        Matrix4 rotationMatrix = getRotationMatrix(_axis,_angle);
		// ((( Exercise 3.4 )))
		m_transformationMatrix = m_transformationMatrix*rotationMatrix;
	}


	//! rotates the object around an arbitrary axis in world coordinate system
	void rotateAroundAxisWorld( const Vector3 & _pt, const Vector3& _axis, float _angle )
	{
        Matrix4 rotationMatrix = getRotationMatrix(_axis,_angle);
		// ((( Exercise 3.4 )))
		m_transformationMatrix = getTranslationMatrix(_pt)*rotationMatrix*getTranslationMatrix(-_pt)*m_transformationMatrix;
	}

	//! rotates the object around an arbitrary axis in object coordinate system
	void rotateAroundAxisObject( const Vector3 & _pt, const Vector3& _axis, float _angle )
	{
        Matrix4 rotationMatrix = getRotationMatrix(_axis,_angle);
		// ((( Exercise 3.4 )))
		m_transformationMatrix = m_transformationMatrix*getTranslationMatrix(_pt)*rotationMatrix*getTranslationMatrix(-_pt);
	}


	//! calculate translation matrix from vector
	static Matrix4 getTranslationMatrix(const Vector3 & _trans) {

		Matrix4 translationMatrix;
        
		// ((( Exercise 3.4 )))
        translationMatrix.loadIdentity();
        translationMatrix=Matrix4(1,0,0,_trans[0],
                                  0,1,0,_trans[1],
                                  0,0,1,_trans[2],
                                  0,0,0,1);

		return translationMatrix;
	}
	
	//! calculate scale matrix from vector
	static Matrix4 getScaleMatrix(const Vector3 & _scale) {
		
		Matrix4 scaleMatrix;
        
		// ((( Exercise 3.4 )))
        scaleMatrix.loadIdentity();
		scaleMatrix = Matrix4(_scale[0],0,0,0,
							  0,_scale[1],0,0,
							  0,0,_scale[2],0,
							  0,0,0,1);		

		return scaleMatrix;
	}
	
	//! calculate rotation matrix from rotation axis and angle in radian
	static Matrix4 getRotationMatrix(const Vector3 & axis, double angle) {
		double cosa = cos(angle);
		double sina = sin(angle);
		Matrix4 rotationMatrix;

		// ((( Exercise 3.4 )))
		rotationMatrix.loadIdentity();

		double x = axis[0];
		double y = axis[1];
		double z = axis[2];

		rotationMatrix = Matrix4(x*x*(1-cosa)+cosa, x*y*(1-cosa)-z*sina, x*z*(1-cosa)+y*sina, 0,
								 y*x*(1-cosa)+z*sina, y*y*(1-cosa)+cosa, y*z*(1-cosa)-x*sina, 0,
								 x*z*(1-cosa)-y*sina, y*z*(1-cosa)+x*sina, z*z*(1-cosa)+cosa, 0,
								 0,0,0,1);
								 
		return rotationMatrix;
	}
	
protected:
    //------------------------------------------------------- protected data
    //------------- any subclasses of Object3D will have access to this data

	//! transformation matrix from object to world coordinate system
	Matrix4 m_transformationMatrix;
};


//=============================================================================
#endif // OBJECT3D_HH defined
//=============================================================================


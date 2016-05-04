#ifndef _HZ_makeRollNode
#define _HZ_makeRollNode
//
// Copyright (C) 
// 
// File: HZ_makeRollNode.h
//
// Dependency Graph Node: HZ_makeRoll
//
// Author: Maya Plug-in Wizard 2.0
//

#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFloatVector.h>
#include <maya/MTime.h>
#include <maya/MGlobal.h>
#include <maya/MFloatMatrix.h>
#include <maya/MFloatPoint.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MEulerRotation.h>
#include <maya/MMatrix.h>
#include <maya/MAngle.h>
#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <math.h>
#include <maya/MFnEnumAttribute.h>
class HZ_makeRoll : public MPxNode
{
public:
						HZ_makeRoll();
	virtual				~HZ_makeRoll(); 

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );

	static  void*		creator();
	static  MStatus		initialize();
public:
	
	static  MObject    input_position;
	static  MObject    input_positionX;
	static  MObject    input_positionY;
	static  MObject    input_positionZ;

	static  MObject    input_upVector;   
	static  MObject    input_upVectorX;
	static  MObject    input_upVectorY;
	static  MObject    input_upVectorZ;

	static  MObject    input_radius;
	static  MObject    input_time;
	static  MObject    input_currentTime;
	static  MObject    input_weight;
	static  MObject    input_matrix;

	static  MObject    output_rotate;
	static  MObject    output_rotateX;
	static  MObject    output_rotateY;
	static  MObject    output_rotateZ;

	static	MObject    rotate_axis;

	static  MObject    __oldPosition__;
	static  MObject    __oldPositionX__;
	static  MObject    __oldPositionY__;
	static  MObject    __oldPositionZ__;
	static	MTypeId		id;
	static  MFloatVector _X_;
	static  MFloatVector _Y_;
	static  MFloatVector _Z_;
};

#endif

/*
------------------------------------
日期：2016 / 04 / 25
作者：浩子
邮箱：yinhao.ti@gmail.com
QQ : 97135036
------------------------------------
球体自由旋转节点

构建的旋转坐标系为 Y轴(upVector 参数）
球体的运动方向为   X轴(move_vector)
旋转轴为          Z轴(rotate_vector)
也就是和世界坐标一样的性质的坐标
所以绕Z轴向旋转时 根据距离
*/

#include "HZ_makeRollNode.h"


MTypeId     HZ_makeRoll::id(0x00001);

MObject     HZ_makeRoll::input_position;
MObject     HZ_makeRoll::input_positionX;
MObject     HZ_makeRoll::input_positionY;
MObject     HZ_makeRoll::input_positionZ;

MObject     HZ_makeRoll::input_upVector;
MObject     HZ_makeRoll::input_upVectorX;
MObject     HZ_makeRoll::input_upVectorY;
MObject     HZ_makeRoll::input_upVectorZ;

MObject     HZ_makeRoll::input_radius;
MObject     HZ_makeRoll::input_time;
MObject     HZ_makeRoll::input_currentTime;
MObject     HZ_makeRoll::input_weight;
MObject     HZ_makeRoll::input_matrix;

MObject     HZ_makeRoll::output_rotate;
MObject     HZ_makeRoll::output_rotateX;
MObject     HZ_makeRoll::output_rotateY;
MObject     HZ_makeRoll::output_rotateZ;

MObject     HZ_makeRoll::rotate_axis;

MObject     HZ_makeRoll::__oldPosition__;
MObject     HZ_makeRoll::__oldPositionX__;
MObject     HZ_makeRoll::__oldPositionY__;
MObject     HZ_makeRoll::__oldPositionZ__;

MFloatVector HZ_makeRoll::_X_(1.0,0.0,0.0);
MFloatVector HZ_makeRoll::_Y_(0.0, 1.0, 0.0);
MFloatVector HZ_makeRoll::_Z_(1.0, 0.0, 1.0);

HZ_makeRoll::HZ_makeRoll() {}
HZ_makeRoll::~HZ_makeRoll() {}



MStatus HZ_makeRoll::compute(const MPlug& plug, MDataBlock& data)
{
	MStatus returnStatus;
	

	if (plug == output_rotate || plug == output_rotateX || plug == output_rotateY || plug == output_rotateZ)
	{   
		// 判断需不需要运算的数据
		double time_dou_data        = data.inputValue(input_time, &returnStatus).asTime().as(MTime::kSeconds);
		double currentTime_dou_data = data.inputValue(input_currentTime, &returnStatus).asTime().as(MTime::kSeconds);
		float  weight_flo_data      = data.inputValue(input_weight).asFloat();
		if (currentTime_dou_data > time_dou_data)
		{
			return MS::kUnknownParameter;
		}
		// 初始化数据 
		MFloatVector & currentPoition_flv_data = data.inputValue(input_position).asFloatVector();

		MFloatVector & upVector_flv_data       = data.inputValue(input_upVector).asFloatVector();
		float radias_Flo_data                  = data.inputValue(input_radius).asFloat();
		
		MDataHandle beforePosition_hand        = data.inputValue(__oldPosition__);
		MFloatVector & beforePosition_flv_data = beforePosition_hand.asFloatVector();
			
		MDataHandle outputRotate_hand        = data.outputValue(output_rotate);
		MFloatVector & outputRotate_flv_data = outputRotate_hand.asFloatVector();
		int rotateAxis = data.inputValue(rotate_axis).asShort();

		//----------------------------构建旋转旋转矩阵-----------------------------------
		//  X 轴 运动方向
		MFloatVector X_axis_flv_data = currentPoition_flv_data - beforePosition_flv_data ;
		X_axis_flv_data.normalize();
		
		// Y 轴 向上轴向
		MFloatVector  Y_axis_flv_data = upVector_flv_data - currentPoition_flv_data;
		Y_axis_flv_data.normalize();
		float temp = round(X_axis_flv_data * Y_axis_flv_data);
		if (temp == 1.0 || temp == -1.0)
		{
			beforePosition_hand.setMFloatVector(currentPoition_flv_data);
			return MS::kSuccess;
		}
		// Z 轴 自定义的球体旋转轴
		MFloatVector Z_axis_flv_data = X_axis_flv_data ^ Y_axis_flv_data;
		Z_axis_flv_data.normalize();
		
		// Y 轴 通过 X 和 Z 差乘得到新的Z轴 使得 X，Y，Z 是标准的相互垂直的坐标系。
		Y_axis_flv_data = Z_axis_flv_data ^ X_axis_flv_data;
		Y_axis_flv_data.normalize();
		
		//把获得X,Y,Z轴拼装成一个矩阵
		MMatrix four_matrix;
		// 当为0时，为三个轴都旋转。
		if (rotateAxis==0) // 三个轴都旋转 构建咱的矩阵
		{
			
			four_matrix[0][0] = X_axis_flv_data.x;
			four_matrix[0][1] = X_axis_flv_data.y;
			four_matrix[0][2] = X_axis_flv_data.z;

			four_matrix[1][0] = Y_axis_flv_data.x;
			four_matrix[1][1] = Y_axis_flv_data.y;
			four_matrix[1][2] = Y_axis_flv_data.z;

			four_matrix[2][0] = Z_axis_flv_data.x;
			four_matrix[2][1] = Z_axis_flv_data.y;
			four_matrix[2][2] = Z_axis_flv_data.z;
		}

		MMatrix four_matrix_inverse(four_matrix);
		four_matrix_inverse = four_matrix_inverse.inverse();

		//----------------------------构建旋转旋转矩阵-----------------------------------
		//球运动距离转为对应半径圆转了多少角度，转为欧拉角（就是Z轴旋转的度数）再转化为绕Z轴旋转的矩阵
		float moveDistance_flo_data = (currentPoition_flv_data - beforePosition_flv_data).length();
		double roate_mangle = (-1.0*moveDistance_flo_data / radias_Flo_data)*weight_flo_data;
		
		// 位移前球体的旋转矩阵
		MEulerRotation  current_euleRoat(outputRotate_flv_data.x*(3.1415926 / 180.0),
										outputRotate_flv_data.y*(3.1415926 / 180.0),
										outputRotate_flv_data.z*(3.1415926 / 180.0), MEulerRotation::kXYZ);
		MMatrix current_mmatrix = current_euleRoat.asMatrix();
		MEulerRotation  after_euleRoat;
		if (rotateAxis !=0)
		{
			switch (rotateAxis)
			{
				case 1://X
					if (round(X_axis_flv_data * _X_) == 1.0 || round(X_axis_flv_data * _X_) == -1.0)//只有X轴旋转时 位移X轴，应当不旋转
					{
						beforePosition_hand.setMFloatVector(currentPoition_flv_data);
						return MS::kSuccess;
					}
					
					if (X_axis_flv_data.z > 0)
					{ after_euleRoat.setValue(roate_mangle*-1, 0.0, 0.0, MEulerRotation::kXYZ); }
					else 
					{ after_euleRoat.setValue(roate_mangle, 0.0, 0.0, MEulerRotation::kXYZ); }
					break;
				case 2://Y
					if (round(X_axis_flv_data * _Y_) == 1.0 || round(X_axis_flv_data * _Y_) == -1.0)//只有Y轴旋转时 位移Y轴，应当不旋转
					{
						beforePosition_hand.setMFloatVector(currentPoition_flv_data);
						return MS::kSuccess;
					}
					if (X_axis_flv_data.x < 0)
					{ after_euleRoat.setValue(0.0, roate_mangle*-1, 0.0, MEulerRotation::kXYZ); }
					else
					{ after_euleRoat.setValue(0.0, roate_mangle, 0.0, MEulerRotation::kXYZ); }
					break;
				case 3://Z
					if (round(X_axis_flv_data * _Z_) == 1.0 || round(X_axis_flv_data * _Z_) == -1.0) //只有Z轴旋转时 位移Z轴，应当不旋转
					{
						beforePosition_hand.setMFloatVector(currentPoition_flv_data);
						return MS::kSuccess;
					}
					if (X_axis_flv_data.x < 0)
					{ after_euleRoat.setValue(0.0, 0.0, roate_mangle*-1, MEulerRotation::kXYZ); }
					else
					{ after_euleRoat.setValue(0.0, 0.0, roate_mangle, MEulerRotation::kXYZ); }
					break;
			}
		}
		else
		{
			after_euleRoat.setValue(0.0, 0.0, roate_mangle, MEulerRotation::kXYZ);
		}
		// 位移后的欧拉角,转到矩阵。
		//MEulerRotation  after_euleRoat(0.0, 0.0, roate_mangle, MEulerRotation::kXYZ);
		MMatrix after_mmatrix = after_euleRoat.asMatrix();
		
		//------------------------------------------------------------------------------
		//球运动距离转为对应半径圆转了多少角度，转为欧拉角（就是Z轴旋转的度数）
		//再转化为绕Z轴旋转的矩阵

		//-----------------------球体旋转之前的矩阵相对于构建矩阵---------------------------
		MMatrix qiu_with_four_matrix = current_mmatrix * four_matrix_inverse;
		MMatrix qiu_with_four_matrix_inverse(qiu_with_four_matrix);
		qiu_with_four_matrix_inverse = qiu_with_four_matrix_inverse.inverse();
		//-----------------------球体旋转之前的矩阵相对于构建矩阵---------------------------

		//-----------------------将各个矩阵相乘，获得最后的输出矩阵-------------------------
		MMatrix output_matrix ;
		output_matrix = qiu_with_four_matrix * after_mmatrix * qiu_with_four_matrix_inverse *current_mmatrix;
		MTransformationMatrix output_tranMatr(output_matrix);
		MEulerRotation output_euleRota = output_tranMatr.eulerRotation();
		//-----------------------将各个矩阵相乘，获得最后的输出矩阵-------------------------

		//----------------------------------设置数值--------------------------------------
		//outputRotateX_hand.setDouble(output_euleRota.x*180.0 / 3.1415926);

		MFloatVector out_flv(float(output_euleRota.x*180.0 / 3.1415926), float(output_euleRota.y*180.0 / 3.1415926), float(output_euleRota.z*180.0 / 3.1415926));
		outputRotate_hand.setMFloatVector(out_flv);

		beforePosition_hand.setMFloatVector(currentPoition_flv_data);
		//----------------------------------设置数值--------------------------------------
		data.setClean(plug);
	}
	else {
		return MS::kSuccess;
	}
	
	return MS::kSuccess;
	
}

void* HZ_makeRoll::creator()
{
	return new HZ_makeRoll;
}

MStatus HZ_makeRoll::initialize()

{

	MFnNumericAttribute numer_attr;
	MFnMatrixAttribute  matrix_attr;
	MFnUnitAttribute    unit_attr;
	MFnEnumAttribute    enum_attr;
	
	//input 位置
	input_positionX = numer_attr.create("positionX", "iX", MFnNumericData::kFloat, 0.0);
	input_positionY = numer_attr.create("positionY", "iY", MFnNumericData::kFloat, 0.0);
	input_positionZ = numer_attr.create("positionZ", "iZ", MFnNumericData::kFloat, 0.0);
	input_position  = numer_attr.create("position", "in", input_positionX, input_positionY, input_positionZ);
	numer_attr.setWritable(true);
	numer_attr.setStorable(true);
	numer_attr.setChannelBox(true);

	//input 向上向量
	input_upVectorX = numer_attr.create("upVetorX", "upX", MFnNumericData::kFloat, 0.0);
	input_upVectorY = numer_attr.create("upVetorY", "upY", MFnNumericData::kFloat, 0.0);
	input_upVectorZ = numer_attr.create("upVetorZ", "upZ", MFnNumericData::kFloat, 0.0);
	input_upVector  = numer_attr.create("upVetor", "up", input_upVectorX, input_upVectorY, input_upVectorZ);
	numer_attr.setWritable(true);
	numer_attr.setStorable(true);
	numer_attr.setChannelBox(true);
	
	//input 半径
	input_radius = numer_attr.create("radius", "ra", MFnNumericData::kFloat, 0.0);
	numer_attr.setWritable(true);
	numer_attr.setStorable(true);
	numer_attr.setChannelBox(true);
	
	//input 系统帧
	input_time = unit_attr.create("inputTime", "it", MFnUnitAttribute::kTime, 1.0);
	unit_attr.setChannelBox(true);
	
	//input 运算开始帧
	input_currentTime = unit_attr.create("currentTime", "ct", MFnUnitAttribute::kTime, 0.0);
	unit_attr.setChannelBox(true);

	//input 权重值
	input_weight = numer_attr.create("weight", "we", MFnNumericData::kFloat, 1.0);
	numer_attr.setWritable(true);
	numer_attr.setStorable(true);
	numer_attr.setChannelBox(true);
	numer_attr.setMin(0.0);
	numer_attr.setMax(1.0);

	//input 矩阵
	input_matrix = matrix_attr.create("inputMatrix", "inm", MFnMatrixAttribute::kFloat);
	matrix_attr.setWritable(true);
	matrix_attr.setStorable(true);

	//input 轴向
	rotate_axis = enum_attr.create("rotateAxis", "rax", 0);
	enum_attr.addField("All", 0);
	enum_attr.addField("X", 1);
	enum_attr.addField("Y", 2);
	enum_attr.addField("Z", 3);
	enum_attr.setWritable(true);
	enum_attr.setStorable(true);
	enum_attr.setChannelBox(true);

	/*
	XAis = numer_attr.create("aisX", "aX", MFnNumericData::kBoolean, true);
	YAis = numer_attr.create("aisY", "aY", MFnNumericData::kBoolean, true);
	ZAis = numer_attr.create("aisZ", "aZ", MFnNumericData::kBoolean, true);
	numer_attr.setWritable(true);
	numer_attr.setStorable(true);
	*/

	//input 上一针位置 内部属性
	__oldPositionX__ = numer_attr.create("oldPositionX", "olX", MFnNumericData::kFloat, 0.0);
	__oldPositionY__ = numer_attr.create("oldPositionY", "olY", MFnNumericData::kFloat, 0.0);
	__oldPositionZ__ = numer_attr.create("oldPositionZ", "olZ", MFnNumericData::kFloat, 0.0);
	__oldPosition__  = numer_attr.create("oldPosition", "old", __oldPositionX__, __oldPositionY__, __oldPositionZ__);
	numer_attr.setWritable(true);
	numer_attr.setStorable(true);

	//output 输出欧拉角
	output_rotateX = numer_attr.create("output_rotateX", "outX", MFnNumericData::kFloat, 0.0);
	output_rotateY = numer_attr.create("output_rotateY", "outY", MFnNumericData::kFloat, 0.0);
	output_rotateZ = numer_attr.create("output_rotateZ", "outZ", MFnNumericData::kFloat, 0.0);
	output_rotate = numer_attr.create("output_rotate", "outr", output_rotateX, output_rotateY, output_rotateZ);



	addAttribute(input_position);
	addAttribute(input_positionX);
	addAttribute(input_positionY);
	addAttribute(input_positionZ);
	addAttribute(input_upVector);
	addAttribute(input_upVectorX);
	addAttribute(input_upVectorY);
	addAttribute(input_upVectorZ);
	addAttribute(input_radius);
	addAttribute(input_time);
	addAttribute(input_currentTime);
	addAttribute(input_weight);
	addAttribute(input_matrix);
	addAttribute(output_rotate);
	addAttribute(output_rotateX);
	addAttribute(output_rotateY);
	addAttribute(output_rotateZ);
	addAttribute(rotate_axis);
	addAttribute(__oldPosition__);
	addAttribute(__oldPositionX__);
	addAttribute(__oldPositionY__);
	addAttribute(__oldPositionZ__);



	attributeAffects(input_position, output_rotate);
	attributeAffects(input_positionX, output_rotate);
	attributeAffects(input_positionY, output_rotate);
	attributeAffects(input_positionZ, output_rotate);

	attributeAffects(input_upVector, output_rotate);
	attributeAffects(input_upVectorX, output_rotate);
	attributeAffects(input_upVectorY, output_rotate);
	attributeAffects(input_upVectorZ, output_rotate);

	attributeAffects(input_radius, output_rotate);
	attributeAffects(input_time, output_rotate);
	attributeAffects(input_currentTime, output_rotate);
	attributeAffects(input_weight, output_rotate);
	attributeAffects(input_matrix, output_rotate);



	attributeAffects(rotate_axis, output_rotate);

	attributeAffects(__oldPosition__, output_rotate);
	attributeAffects(__oldPositionX__, output_rotate);
	attributeAffects(__oldPositionY__, output_rotate);
	attributeAffects(__oldPositionZ__, output_rotate);

	return MS::kSuccess;

}


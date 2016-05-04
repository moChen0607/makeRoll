# -*- coding: utf-8 -*-
#
'''
------------------------------------
日期：2016/03/25
作者：浩子
邮箱：yinhao.ti@gmail.com
QQ : 97135036
------------------------------------
球体自由旋转节点

构建的旋转坐标系为 Y轴 (upVector 参数）
球体的运动方向为   X轴 (move_vector)
旋转轴为          Z轴 (rotate_vector)
也就是和世界坐标一样的性质的坐标
所以绕Z轴向旋转时 根据距离

'''
import sys
import maya.OpenMaya as om
import maya.OpenMayaMPx as omm

NODENAME = 'makeRollPlug'
NODEID = om.MTypeId(0x87005)


# noinspection PyCallByClass
class makeRollNode(omm.MPxNode):
    # 输入属性
    input_position    = om.MObject()  # 记录球体位置
    input_positionX   = om.MObject()
    input_positionY   = om.MObject()
    input_positionZ   = om.MObject()
    
    input_upVector    = om.MObject()  # 向上向量
    input_upVectorX   = om.MObject()
    input_upVectorY   = om.MObject()
    input_upVectorZ   = om.MObject()
    input_radius      = om.MObject()  # 球体半径
    input_time        = om.MObject()  # 输入的时间
    input_currentTime = om.MObject()  # maya时间
    input_weight      = om.MObject()  # 比重值
    input_matrix      = om.MObject()  # 矩阵属性
    # 输出属性
    output_rotate     = om.MObject()
    output_rotateX    = om.MObject()
    output_rotateY    = om.MObject()
    output_rotateZ    = om.MObject()
    # 布尔
    # XAis              = om.MObject()  # X 轴向旋转
    # YAis              = om.MObject()  # Y 轴向旋转
    # ZAis              = om.MObject()  # Z 轴向旋转

    rotate_axis       = om.MObject()
    # 内部属性
    __oldPosition__   = om.MObject()  # 上一帧之前的位置
    __oldPositionX__  = om.MObject()
    __oldPositionY__  = om.MObject()
    __oldPositionZ__  = om.MObject()

    def __init__(self):
        super(makeRollNode, self).__init__()
        #0 是默认三个轴都旋转的
        #当不是0时 four_matrix_list 就是单位矩阵.
        self.rotate_axis = {0:(0.0,0.0,1.0),1:(1.0,0.0,0.0),2:(0.0,1.0,0.0),3:(0.0,0.0,1.0)}
        self.axis        = {0:None,1:'X',2:'Y',3:'Z'}
    # noinspection PyTypeChecker
    def MMatrix_to_MFloatMatrix(self, MMatrix):
        data_list = [
            MMatrix(0, 0), MMatrix(0, 1), MMatrix(0, 2), MMatrix(0, 3),
            MMatrix(1, 0), MMatrix(1, 1), MMatrix(1, 2), MMatrix(1, 3),
            MMatrix(2, 0), MMatrix(2, 1), MMatrix(2, 2), MMatrix(2, 3),
            MMatrix(3, 0), MMatrix(3, 1), MMatrix(3, 2), MMatrix(3, 3)]
        out_MFloatMatrix = om.MFloatMatrix()
        om.MScriptUtil.createFloatMatrixFromList(data_list, out_MFloatMatrix)
        return out_MFloatMatrix

    # noinspection PyTypeChecker
    def MFloatMatrix_to_MMatrix(self, MFloatMatrix):
        data_list = [
            MFloatMatrix(0, 0), MFloatMatrix(0, 1), MFloatMatrix(0, 2), MFloatMatrix(0, 3),
            MFloatMatrix(1, 0), MFloatMatrix(1, 1), MFloatMatrix(1, 2), MFloatMatrix(1, 3),
            MFloatMatrix(2, 0), MFloatMatrix(2, 1), MFloatMatrix(2, 2), MFloatMatrix(2, 3),
            MFloatMatrix(3, 0), MFloatMatrix(3, 1), MFloatMatrix(3, 2), MFloatMatrix(3, 3)]
        out_MMatrix = om.MMatrix()
        om.MScriptUtil.createMatrixFromList(data_list, out_MMatrix)
        return out_MMatrix

    # noinspection PyTypeChecker
    def compute(self, plug, dataBlock):
        # dataBlock -->> MDataBlock
        # plug      -->> MPlug

        # if plug.parent()  == makeRollNode.output_rotate :
        if plug in (makeRollNode.output_rotate,
                    makeRollNode.output_rotateX,
                    makeRollNode.output_rotateY,
                    makeRollNode.output_rotateZ):
            



            # ------------------根据输入时间等等其他属性，判断是否运行------------------------
            time_value        = dataBlock.inputValue(makeRollNode.input_time).asTime().value()
            currentTime_value = dataBlock.inputValue(makeRollNode.input_currentTime).asInt()
            weight_value      = dataBlock.inputValue(makeRollNode.input_weight).asFloat()

            if currentTime_value > time_value:
                return om.kUnknownParameter
            if weight_value == 0.0:
                return om.kUnknownParameter
            # ------------------根据输入时间等等其他属性，判断是否运行------------------------
            



            # ----------------------------------初始化数据------------------------------------
            # 获取输入属性的   和储存节点里的 MDataHandle
            position_value = dataBlock.inputValue(makeRollNode.input_position).asFloatVector()
            radius_value   = dataBlock.inputValue(makeRollNode.input_radius).asFloat()
            upVector_temp  = dataBlock.inputValue(makeRollNode.input_upVector).asFloatVector()

            upVector_value = om.MFloatVector(upVector_temp-position_value)
            upVector_value.normalize()

            #input_matrix_value = dataBlock.inputValue(makeRollNode.input_matrix).asFloatMatrix()
            # XAis_value          = dataBlock.inputValue(makeRollNode.XAis).asBool()
            # YAis_value          = dataBlock.inputValue(makeRollNode.YAis).asBool()
            # ZAis_value          = dataBlock.inputValue(makeRollNode.ZAis).asBool()

            axis_value          = dataBlock.inputValue(makeRollNode.rotate_axis).asShort()
            axis_vector         = om.MFloatVector(*self.rotate_axis[axis_value])


            old_position_data   = dataBlock.inputValue(makeRollNode.__oldPosition__)
            old_position_value  = old_position_data.asFloatVector()

            output_rotate_data  = dataBlock.outputValue(makeRollNode.output_rotateX)
            output_rotate_value = output_rotate_data.asFloatVector()
            # ----------------------------------初始化数据------------------------------------




            # ----------------------------构建旋转旋转矩阵-----------------------------------
            # X
            move_vector = position_value-old_position_value
            move_vector.normalize()

            # 如果 运动方向和 Y 同向或反向 不旋转
            if round((move_vector*upVector_value), 5) == 1 or round((move_vector*upVector_value), 5) == -1:
                old_position_data.setMFloatVector(position_value)
                return om.kUnknownParameter

            # Z =  X ^ Y
            rotae_vector = move_vector ^ upVector_value
            rotae_vector.normalize()
            # Y =  Z ^ X
            upVector_value = rotae_vector ^ move_vector
            upVector_value.normalize()



            if not axis_value==0 :
                if round((move_vector * axis_vector),5) ==1 or round((move_vector * axis_vector),5) == -1 :
                    old_position_data.setMFloatVector(position_value)
                    return om.kUnknownParameter

                four_matrix_list = [1.0,0.0,0.0,0.0,
                                    0.0,1.0,0.0,0.0,
                                    0.0,0.0,1.0,0.0,
                                    0.0,0.0,0.0,1.0]
            else:
                four_matrix_list = [move_vector.x, move_vector.y, move_vector.z, 0.0,
                                    upVector_value.x, upVector_value.y, upVector_value.z, 0.0,
                                    rotae_vector.x, rotae_vector.y, rotae_vector.z, 0.0,
                                    0.0, 0.0, 0.0, 1.0]
            four_float_matrix = om.MFloatMatrix()
            om.MScriptUtil.createFloatMatrixFromList(four_matrix_list, four_float_matrix)
            # four_matrix_inverse
            four_float_matrix_inverse = om.MFloatMatrix(four_float_matrix)
            four_float_matrix_inverse = four_float_matrix_inverse.inverse()
            # ----------------------------构建旋转旋转矩阵-----------------------------------

            # --------------------当单轴向旋转时，确定位移方向的正反方向-----------------------
            if self.axis[axis_value] == 'X':   #只有X轴旋转
                if move_vector.z > 0 :
                    axis_vector.x = -1
            elif self.axis[axis_value] == 'Z': #只有Z轴旋转
                if move_vector.x < 0 :
                    axis_vector.z = -1
            elif move_vector.x < 0 :           #只有Y轴旋转
                    axis_vector.y = -1
            # --------------------当单轴向旋转时，确定位移方向的正反方向-----------------------

            # -----------------------球体运动距离转换为矩阵(旋转轴为Z)--------------------------
            move_distance = round((position_value-old_position_value).length(), 5)
            out_Tf        = om.MTransformationMatrix()
            out_Tf.rotateTo(om.MEulerRotation(output_rotate_value.x*(3.1415926/180.0),
                                              output_rotate_value.y*(3.1415926/180.0),
                                              output_rotate_value.z*(3.1415926/180.0),
                                              om.MEulerRotation.kXYZ))

            old_position_matrix_value        = self.MMatrix_to_MFloatMatrix(out_Tf.asMatrix())
            rotate_mangle                    = (-1.0*move_distance/radius_value)*weight_value
            
            rotate_euler                     = om.MEulerRotation(rotate_mangle*axis_vector.x,
                                                                 rotate_mangle*axis_vector.y,
                                                                 rotate_mangle*axis_vector.z,
                                                                 om.MEulerRotation.kXYZ)
            rotate_float_matrix              = self.MMatrix_to_MFloatMatrix(rotate_euler.asMatrix())
            # -----------------------球体运动距离转换为矩阵(旋转轴为Z)--------------------------
            
            # -----------------------球体旋转之前的矩阵相对于构建矩阵---------------------------
            qiu_to_four_float_matrix         = old_position_matrix_value*four_float_matrix_inverse
            qiu_to_four_float_matrix_inverse = om.MFloatMatrix(qiu_to_four_float_matrix)
            qiu_to_four_float_matrix_inverse = qiu_to_four_float_matrix_inverse.inverse()
            # -----------------------球体旋转之前的矩阵相对于构建矩阵---------------------------




            # ---------------乘法矩阵链并输出到output_rotate_data==>> MeulerRotate------------
            out_float_matrix      = qiu_to_four_float_matrix*rotate_float_matrix*qiu_to_four_float_matrix_inverse\
                                    *old_position_matrix_value
            transformation_matrix = om.MTransformationMatrix(self.MFloatMatrix_to_MMatrix(out_float_matrix))
            # 从矩阵中提取出旋转
            output_rotate_data    = transformation_matrix.eulerRotation()
            # ---------------乘法矩阵链并输出到output_rotate_data==>> MeulerRotate------------




            # ---------------获取输出数值的 MDataHandle 并设置输出值---------------
            # rotate_data       = dataBlock.outputValue(makeRollNode.output_rotateX)
            rotateX_data = dataBlock.outputValue(makeRollNode.output_rotateX)
            rotateY_data = dataBlock.outputValue(makeRollNode.output_rotateY)
            rotateZ_data = dataBlock.outputValue(makeRollNode.output_rotateZ)
            rotateX_data.setFloat(output_rotate_data.x*180.0/3.1415926)
            rotateY_data.setFloat(output_rotate_data.y*180.0/3.1415926)
            rotateZ_data.setFloat(output_rotate_data.z*180.0/3.1415926)
            # ---------------获取输出数值的 MDataHandle 并设置输出值---------------




            # ---------------设置old_position_value 和 old_position_matrix_value------------------
            old_position_data.setMFloatVector(position_value)
            # ---------------设置old_position_value 和 old_position_matrix_value------------------

            dataBlock.setClean(plug)
        else:
            return om.kUnknownParameter

    @classmethod
    def nodeInitializer(self):
        numer_attr                   = om.MFnNumericAttribute()
        matrix_attr                  = om.MFnMatrixAttribute()
        unit_attr                    = om.MFnUnitAttribute()
        enum_attr                    = om.MFnEnumAttribute()
        # 创建Position
        makeRollNode.input_positionX = numer_attr.create('positionX', 'pX', om.MFnNumericData.kFloat)
        makeRollNode.input_positionY = numer_attr.create('positionY', 'pY', om.MFnNumericData.kFloat)
        makeRollNode.input_positionZ = numer_attr.create('positionZ', 'pZ', om.MFnNumericData.kFloat)
        makeRollNode.input_position  = numer_attr.create('position', 'ps', makeRollNode.input_positionX,
                                                        makeRollNode.input_positionY,
                                                        makeRollNode.input_positionZ)
        numer_attr.setWritable(True)
        numer_attr.setStorable(True)
        numer_attr.setChannelBox(True)

        # 创建upVector
        makeRollNode.input_upVectorX = numer_attr.create('upVectorX', 'upX', om.MFnNumericData.kFloat)
        makeRollNode.input_upVectorY = numer_attr.create('upVectorY', 'upY', om.MFnNumericData.kFloat)
        makeRollNode.input_upVectorZ = numer_attr.create('upVectorZ', 'upZ', om.MFnNumericData.kFloat)
        makeRollNode.input_upVector  = numer_attr.create('upVector', 'up', makeRollNode.input_upVectorX,
                                                        makeRollNode.input_upVectorY,
                                                        makeRollNode.input_upVectorZ)
        numer_attr.setWritable(True)
        numer_attr.setStorable(True)
        numer_attr.setChannelBox(True)

        # 创建 radius
        makeRollNode.input_radius = numer_attr.create('radius', 'ra', om.MFnNumericData.kFloat, 1.0)
        numer_attr.setWritable(True)
        numer_attr.setStorable(True)
        numer_attr.setChannelBox(True)

        # 创建 time
        makeRollNode.input_time  = unit_attr.create('inputTime', 'it', om.MFnUnitAttribute.kTime, 1.0)

        # 创建 currentTime
        makeRollNode.input_currentTime = numer_attr.create('currentTime', 'ct', om.MFnNumericData.kInt, 100)
        numer_attr.setWritable(True)
        numer_attr.setStorable(True)
        numer_attr.setChannelBox(True)

        # 创建 weight
        makeRollNode.input_weight = numer_attr.create('weight', 'we', om.MFnNumericData.kFloat, 1.0)
        numer_attr.setWritable(True)
        numer_attr.setStorable(True)
        numer_attr.setChannelBox(True)
        numer_attr.setMax(1.0)
        numer_attr.setMin(0.0)

        # 创建 matrix
        makeRollNode.input_matrix   = matrix_attr.create('inputMatrix', 'im', om.MFnNumericData.kFloat)
        matrix_attr.setWritable(True)
        matrix_attr.setStorable(True)

        # 创建 rotate
        makeRollNode.output_rotateX = numer_attr.create('outputRotateX', 'orX', om.MFnNumericData.kFloat)
        makeRollNode.output_rotateY = numer_attr.create('outputRotateY', 'orY', om.MFnNumericData.kFloat)
        makeRollNode.output_rotateZ = numer_attr.create('outputRotateZ', 'orZ', om.MFnNumericData.kFloat)
        makeRollNode.output_rotate  = numer_attr.create('outputRotate', 'or', makeRollNode.output_rotateX,
                                                       makeRollNode.output_rotateY,
                                                       makeRollNode.output_rotateZ)
        numer_attr.setWritable(True)
        numer_attr.setStorable(True)
        numer_attr.setChannelBox(True)

        # 创建 布尔
        # makeRollNode.XAis = numer_attr.create('aisX', 'aX', om.MFnNumericData.kBoolean, True)
        # makeRollNode.YAis = numer_attr.create('aisY', 'aY', om.MFnNumericData.kBoolean, True)
        # makeRollNode.ZAis = numer_attr.create('aisZ', 'aZ', om.MFnNumericData.kBoolean, True)
        # numer_attr.setWritable(True)
        # numer_attr.setStorable(True)
        # numer_attr.setChannelBox(True)
        makeRollNode.rotate_axis = enum_attr.create('rotateAxis','rax',0)
        enum_attr.addField('All',0)
        enum_attr.addField('X',1)
        enum_attr.addField('Y',2)
        enum_attr.addField('Z',3)
        enum_attr.setWritable(True)
        enum_attr.setStorable(True)
        enum_attr.setChannelBox(True)        # 创建内部属性

        makeRollNode.__oldPositionX__ = numer_attr.create('oldPositionX', 'oX', om.MFnNumericData.kFloat)
        makeRollNode.__oldPositionY__ = numer_attr.create('oldPositionY', 'oY', om.MFnNumericData.kFloat)
        makeRollNode.__oldPositionZ__ = numer_attr.create('oldPositionZ', 'oZ', om.MFnNumericData.kFloat)
        makeRollNode.__oldPosition__  = numer_attr.create('oldPosition', 'op', makeRollNode.__oldPositionX__,
                                                         makeRollNode.__oldPositionY__,
                                                         makeRollNode.__oldPositionZ__)
        numer_attr.setStorable(True)
        numer_attr.setHidden(True)


        # 添加属性
        for attr in (makeRollNode.input_position,
                     makeRollNode.input_upVector,
                     makeRollNode.input_radius,
                     makeRollNode.input_time,
                     makeRollNode.input_currentTime,
                     makeRollNode.input_weight,
                     makeRollNode.input_matrix,
                     makeRollNode.output_rotate,
                     makeRollNode.rotate_axis,
                     makeRollNode.__oldPosition__):
            makeRollNode.addAttribute(attr)
        # 设置关联方式
        for attr in (makeRollNode.input_position,
                     makeRollNode.input_positionX,
                     makeRollNode.input_positionY,
                     makeRollNode.input_positionZ,
                     makeRollNode.input_upVector,
                     makeRollNode.input_radius,
                     makeRollNode.input_time,
                     makeRollNode.input_currentTime,
                     makeRollNode.input_matrix,
                     makeRollNode.input_weight):
            makeRollNode.attributeAffects(attr, makeRollNode.output_rotate)


def nodeCreate():
    return omm.asMPxPtr(makeRollNode())


def initializePlugin(mobject):
    PLUGIN = omm.MFnPlugin(mobject, 'tyh', '1.0', 'Any')
    try:
        PLUGIN.registerNode(NODENAME, NODEID, nodeCreate, makeRollNode.nodeInitializer)
    except:
        sys.stderr.write(r'安装完成')
        raise


def uninitializePlugin(mobject):
    PLUGIN = omm.MFnPlugin(mobject)
    try:
        PLUGIN.deregisterNode(NODEID)
    except:
        sys.stderr.write(r'卸载完成')

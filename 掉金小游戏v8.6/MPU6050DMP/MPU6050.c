#include "inv_mpu_dmp_motion_driver.h"
#include "MPU6050.h"
#include "inv_mpu.h"
#include "math.h"
#include "Control.h"
#include "IOI2C.h"
#include "filter.h"

#define PRINT_ACCEL     (0x01)
#define PRINT_GYRO      (0x02)
#define PRINT_QUAT      (0x04)
#define ACCEL_ON        (0x01)
#define GYRO_ON         (0x02)
#define MOTION          (0)
#define NO_MOTION       (1)
#define DEFAULT_MPU_HZ  (200)
#define FLASH_SIZE      (512)
#define FLASH_MEM_START ((void*)0x1800)
static signed char gyro_orientation[9] = {-1, 0, 0,
                                           0,-1, 0,
                                           0, 0, 1};

static  unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}


static  unsigned short inv_orientation_matrix_to_scalar(
    const signed char *mtx)
{
    unsigned short scalar;
    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;


    return scalar;
}

static void run_self_test(void)
{
    int result;
    long gyro[3], accel[3];

    result = mpu_run_self_test(gyro, accel);
    if (result == 0x07) 
	{
        /* Test passed. We can trust the gyro data here, so let's push it down
         * to the DMP.
         */
        float sens;
        unsigned short accel_sens;
        mpu_get_gyro_sens(&sens);
        gyro[0] = (long)(gyro[0] * sens);
        gyro[1] = (long)(gyro[1] * sens);
        gyro[2] = (long)(gyro[2] * sens);
        dmp_set_gyro_bias(gyro);
        mpu_get_accel_sens(&accel_sens);
        accel[0] *= accel_sens;
        accel[1] *= accel_sens;
        accel[2] *= accel_sens;
        dmp_set_accel_bias(accel);
    }
}



uint8_t buffer[14];

/**************************实现函数********************************************
*函数原型:		uint8_t MPU6050_getDeviceID(void)
*功　　能:	    读取  MPU6050 WHO_AM_I 标识	 将返回 0x68
*******************************************************************************/
uint8_t MPU6050_getDeviceID(void) 
{
    IICreadBytes(devAddr, MPU6050_RA_WHO_AM_I, 1, buffer);
    return buffer[0];
}

/**************************实现函数********************************************
*函数原型:		uint8_t MPU6050_testConnection(void)
*功　　能:	    检测MPU6050 是否已经连接
*******************************************************************************/
uint8_t MPU6050_testConnection(void) {
   if(MPU6050_getDeviceID() == 0x68)  //0b01101000;
   return 1;
   	else return 0;
}

/**************************实现函数********************************************
*函数原型:		void MPU6050_initialize(void)
*功　　能:	    初始化 	MPU6050 以进入可用状态。
*******************************************************************************/
void MPU6050_initialize(void) 
{
	IICwriteByte(devAddr,MPU6050_RA_GYRO_CONFIG,0x10);		//陀螺仪最大量程 +-1000度每秒,不自检
	IICwriteByte(devAddr,MPU6050_RA_ACCEL_CONFIG,0x00);		//加速度度最大量程 +-2G,不自检
	IICwriteByte(devAddr,MPU6050_RA_USER_CTRL,0x40);
	IICwriteByte(devAddr,MPU6050_RA_INT_PIN_CFG,0x80);
	IICwriteByte(devAddr,MPU6050_RA_PWR_MGMT_1,0x02);		//设置时钟,进入工作状态
}

/**************************************************************************
函数功能：MPU6050内置DMP的初始化
入口参数：无
返回  值：无
作    者：平衡小车之家
**************************************************************************/
void DMP_Init(void)
{ 
	u8 temp[1]={0};
	
	IICreadBytes(0x68,0x75,1,temp);
	if(temp[0]!=0x68)
	{
		//NVIC_SystemReset();
	}
	if(!mpu_init())
	{
		if(!mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL));
		if(!mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL));
		if(!mpu_set_sample_rate(DEFAULT_MPU_HZ));
		if(!dmp_load_motion_driver_firmware());
		if(!dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation)));
		if(!dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
		DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
		DMP_FEATURE_GYRO_CAL));
		if(!dmp_set_fifo_rate(DEFAULT_MPU_HZ));
		run_self_test();
		if(!mpu_set_dmp_state(1));
	}
}
/**************************************************************************
函数功能：读取MPU6050内置DMP的姿态信息
入口参数：无
返回  值：无
作    者：平衡小车之家
**************************************************************************/
#define q30  1073741824.0f
void Read_DMP(void)
{	
	short accel[3], sensors;
	unsigned long sensor_timestamp;
	float q0=1.0f,q1=0.0f,q2=0.0f,q3=0.0f;
	unsigned char more;
	long quat[4];
	
	dmp_read_fifo(&DMP_Data.gyro[0], accel, quat, &sensor_timestamp, &sensors, &more);
	if (sensors & INV_WXYZ_QUAT )
	{
		q0=quat[0] / q30;
		q1=quat[1] / q30;
		q2=quat[2] / q30;
		q3=quat[3] / q30;
		
		DMP_Data.Pitch = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3-Ctl_Data.Origin_Offset; 	
//		DMP_Data.Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3;
//		DMP_Data.Yaw   = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;
		
		DMP_Data.Angular_Velocity=DMP_Data.gyro[1];         //角速度
	}
}

#define PI 				3.14159265
void Get_Angle(void)
{ 
	float Accel_X,Accel_Y,Accel_Z,Gyro_X;
	
	Gyro_X=(IICreadByte(devAddr,MPU6050_RA_GYRO_XOUT_H)<<8)+IICreadByte(devAddr,MPU6050_RA_GYRO_XOUT_L);    //读取Y轴陀螺仪
	Accel_Y=(IICreadByte(devAddr,MPU6050_RA_ACCEL_YOUT_H)<<8)+IICreadByte(devAddr,MPU6050_RA_ACCEL_YOUT_L); //读取X轴加速度计
	Accel_Z=(IICreadByte(devAddr,MPU6050_RA_ACCEL_ZOUT_H)<<8)+IICreadByte(devAddr,MPU6050_RA_ACCEL_ZOUT_L); //读取Z轴加速度计
	
	if(Gyro_X>32768)  Gyro_X-=65536;                       //数据类型转换  也可通过short强制类型转换
	if(Accel_Y>32768) Accel_Y-=65536;                      //数据类型转换
	if(Accel_Z>32768) Accel_Z-=65536;                      //数据类型转换
	
	Accel_X=atan2(Accel_Y,Accel_Z)*180/PI;                 //计算倾角
	Gyro_X=Gyro_X/16.4;                                    //陀螺仪量程转换
	Kalman_Filter(Accel_X,Gyro_X);//卡尔曼滤波	
	
	DMP_Data.Pitch=angle;
}

/**************************************************************************
函数功能：读取MPU6050内置温度传感器数据
入口参数：无
返回  值：摄氏温度
作    者：平衡小车之家
**************************************************************************/
int Read_Temperature(void)
{	   
	  float Temp;
	  Temp=(IICreadByte(devAddr,MPU6050_RA_TEMP_OUT_H)<<8)+IICreadByte(devAddr,MPU6050_RA_TEMP_OUT_L);
		if(Temp>32768) Temp-=65536;
		Temp=(36.53+Temp/340)*10;
	  return (int)Temp;
}
//------------------End of File----------------------------

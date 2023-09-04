#define MPU_ADDR 0x68<<1
#define MPU_WHO_AM_I 0x75
#define MPU_SENS_START 0x3B
#define MPU_SENS_LEN 14
#define MPU_FIFO_EN 0x23
#define MPU_FIFO_SET 0x78
#define MPU_PWR_MGMT_1 0x6B
#define MPU_PWR_MGMT_1_SET 0x00
#define MPU_SMPLRT_DIV 0x19
#define MPU_SMPLRT_DIV_SET 0x07
#define MPU_GYRO_CONFIG	0x1B
#define MPU_GYRO_CONFIG_SET 0x10
#define MPU_I2C_CONFIG 55
#define MPU_I2C_CONFIG_SET 0x02
#define MAG_ADDR 0x0C<<1
#define MAG_WHO_AM_I 0x00
#define MAG_CNTL1 0x0A
#define MAG_CNTL1_SET 0x16
#define MAG_SENS_START 0x03
#define MAG_SENS_LEN 7
#define MAG_ST1 0x02
#define MAG_RESET 0x0B
#define MAG_RESET_SET 0x01
#define INTERRUPT_ENABLE 0x38
#define _RAW_DATA_READY 0x01
#define INT_PIN 55
#define _INT_ANYRD_2CLEAR_ANY_READ 1<<4
#define	_ACTL_LOW 1<<7
#define _OPEN_DRAIN 1<<6
#define _LATCH_INT_EN_UNTIL_CLEARED 1<<5

#define MAG_READ_PERIOD_US 15000
#define ACC_GYRO_READ_PERIOD_US 4500

#define accXoffs 0
#define accYoffs 0.225
#define accZoffs 0.8

#define accXmult 1
#define accYmult 0.9975
#define accZmult 0.992

#define gyroXoffs 1.5807
#define gyroYoffs 1.6857
#define gyroZoffs 0.3176

#define gyroXmult 1
#define gyroYmult 1
#define gyroZmult 1

#define magXOffs -75
#define magYOffs 2.5
#define magZOffs 113.5

#define magXmult 0.8741
#define magYmult 0.8849
#define magZmult 0.8818

#include "MPU9255.h"
#include "math.h"

static I2C_HandleTypeDef* mpu9255i2c;
static float* ACx, *ACy, *ACz, *GYx, *GYy, *GYz, *MAGx, *MAGy, *MAGz;
static uint8_t i2cAccReceivedFlag = 0, i2cMagReceivedFlag = 0;
static uint8_t MPUdata[16]={0};
static uint64_t magTime=0, accGyroTime = 0;
//static const uint8_t ASAX=178, ASAY=178, ASAZ=167;
static uint8_t magDataReady = 0, accGyroDataReady = 0;
static uint8_t* overload;

static void I2C_writeByte(I2C_HandleTypeDef* i2c, uint8_t devAddr, uint8_t regAddr, uint8_t data){
	HAL_I2C_Mem_Write(i2c, devAddr, regAddr, I2C_MEMADD_SIZE_8BIT, &data, 1, 10);
}
static uint8_t I2C_readByte(I2C_HandleTypeDef* i2c, uint8_t devAddr, uint8_t regAddr){
	uint8_t data;
	HAL_I2C_Mem_Read(i2c, devAddr, regAddr, I2C_MEMADD_SIZE_8BIT, &data, 1, 10);
	return data;
}

void MPU9255setup(I2C_HandleTypeDef* i2c, float* _ACx, float* _ACy, float* _ACz, float* _GYx,\
		float* _GYy, float* _GYz, float* _MAGx, float* _MAGy, float* _MAGz, uint8_t *_overload){
		mpu9255i2c = i2c;
	
		HAL_Delay(10);
		I2C_writeByte(mpu9255i2c, MPU_ADDR, MPU_FIFO_EN, MPU_FIFO_SET);
		I2C_writeByte(mpu9255i2c, MPU_ADDR, MPU_PWR_MGMT_1, MPU_PWR_MGMT_1_SET);
		I2C_writeByte(mpu9255i2c, MPU_ADDR, MPU_SMPLRT_DIV, MPU_SMPLRT_DIV_SET);
		I2C_writeByte(mpu9255i2c, MPU_ADDR, MPU_GYRO_CONFIG, MPU_GYRO_CONFIG_SET);
			
		I2C_writeByte(mpu9255i2c, MPU_ADDR, MPU_I2C_CONFIG, MPU_I2C_CONFIG_SET);
		I2C_readByte(mpu9255i2c, MAG_ADDR, MAG_WHO_AM_I);
		HAL_Delay(1);
		I2C_writeByte(mpu9255i2c, MAG_ADDR, MAG_RESET, MAG_RESET_SET);
		I2C_writeByte(mpu9255i2c, MAG_ADDR, MAG_CNTL1, MAG_CNTL1_SET);
//		HAL_Delay(1);
//		I2C_writeByte(mpu9255i2c, MPU_ADDR, INT_PIN, _INT_ANYRD_2CLEAR_ANY_READ | _ACTL_LOW |\
//			_OPEN_DRAIN);
//		I2C_writeByte(mpu9255i2c, MPU_ADDR, INTERRUPT_ENABLE, _RAW_DATA_READY);
			
		overload = _overload;

		ACx = _ACx;
		ACy = _ACy;
		ACz = _ACz;

		GYx = _GYx;
		GYy = _GYy;
		GYz = _GYz;

		MAGx = _MAGx;
		MAGy = _MAGy;
		MAGz = _MAGz;	
}
	
void MPU9255_IT(){
	if ( mpu9255i2c->State == 0x20 && i2cAccReceivedFlag == 1 )
		i2cAccReceivedFlag = 2;
	if ( mpu9255i2c->State == 0x20 && i2cMagReceivedFlag == 1 )
		i2cMagReceivedFlag = 2;		
}

uint8_t MPU9255_ACC_GYRO_isReady(){
	uint8_t ready = accGyroDataReady;
	accGyroDataReady = 0;
	return ready;
}

uint8_t MPU9255_MAG_isReady(){
	uint8_t ready = magDataReady;
	magDataReady = 0;
	return ready;	
}

uint8_t MPU9255_cycle(uint64_t micros){
	uint8_t status = 0;
	if(micros - accGyroTime >= ACC_GYRO_READ_PERIOD_US && mpu9255i2c->State == 0x20 && i2cAccReceivedFlag == 0 && i2cMagReceivedFlag == 0){		
		HAL_I2C_Mem_Read_IT(mpu9255i2c, MPU_ADDR, MPU_SENS_START, I2C_MEMADD_SIZE_8BIT, MPUdata, MPU_SENS_LEN);
		i2cAccReceivedFlag = 1;
		
		accGyroTime = micros;
		
		status |= 1<<0;
	}		
		
	if(micros - magTime >= MAG_READ_PERIOD_US && mpu9255i2c->State == 0x20 && i2cMagReceivedFlag == 0  && i2cAccReceivedFlag == 0){
			HAL_I2C_Mem_Read_IT(mpu9255i2c, MAG_ADDR, MAG_SENS_START, I2C_MEMADD_SIZE_8BIT, MPUdata, MAG_SENS_LEN);		
			i2cMagReceivedFlag = 1;
			magTime = micros;
			status |= 1<<1;
	}
	
 if (i2cAccReceivedFlag == 2){
	i2cAccReceivedFlag = 0;

	*ACx = (int16_t)(MPUdata[0]<<8)|MPUdata[1];
	*ACy = (int16_t)(MPUdata[2]<<8)|MPUdata[3];
	*ACz = (int16_t)(MPUdata[4]<<8)|MPUdata[5];

	*GYx = (int16_t)(MPUdata[8]<<8)|MPUdata[9];
	*GYy = (int16_t)(MPUdata[10]<<8)|MPUdata[11];
	*GYz = (int16_t)(MPUdata[12]<<8)|MPUdata[13];	
	
	if( fabs(*ACx) > 32000 || fabs(*ACy) > 32000 || fabs(*ACz) > 32000 )
		*overload |= 1 << 2;
	else
		*overload &= 0xFB;	
	 
	if( fabs(*GYx) > 32000 || fabs(*GYy) > 32000 || fabs(*GYz) > 32000 )
		*overload |= 1 << 1;
	else
		*overload &= 0xFD;

	*ACx = (*ACx/1638.4 - accXoffs)*accXmult;
	*ACy = (*ACy/1638.4 - accYoffs)*accYmult;
	*ACz = (*ACz/1638.4 - accZoffs)*accZmult;

	*GYx = (*GYx/32.8 - gyroXoffs)*gyroXmult;
	*GYy = (*GYy/32.8 - gyroYoffs)*gyroYmult;
	*GYz = (*GYz/32.8 - gyroZoffs)*gyroZmult;
	 
	accGyroDataReady = 1;
	
	status |= 1<<2;
 }	
 if (i2cMagReceivedFlag == 2){
	i2cMagReceivedFlag = 0;
	*MAGx = (int16_t)(MPUdata[1]<<8)|MPUdata[0];
	*MAGy = (int16_t)(MPUdata[3]<<8)|MPUdata[2];
	*MAGz = (int16_t)(MPUdata[5]<<8)|MPUdata[4];
	 
	*MAGx = (*MAGx - magXOffs)*magXmult;
	*MAGy = (*MAGy - magYOffs)*magYmult;
	*MAGz = (*MAGz - magZOffs)*magZmult;
	 
	 magDataReady = 1;
	 status |= 1<<3;
 }
 
 return status;
}


//static int16_t adjustMag(int16_t magVal, uint8_t adjVal){
//	return magVal*( ((float)adjVal-128)/256 + 1 );
//}




/*	//magnetic self-test
	I2C_writeByte(MAG_ADDR, MAG_CNTL1, 0b00010000);
	I2C_writeByte(MAG_ADDR, 0x0C, 0b01000000);
	I2C_writeByte(MAG_ADDR, MAG_CNTL1, 0b00011000);
	while (1){
		uint8_t data = I2C_readByte(MAG_ADDR, 0x02);
		if (data & 0x01 == 1) break;
	}
	HAL_I2C_Mem_Read(&hi2c1, MAG_ADDR, MAG_SENS_START, I2C_MEMADD_SIZE_8BIT, MAGdata, MAG_SENS_LEN, 100);
	HAL_Delay(10);
	MAGx=(int16_t)(MAGdata[1]<<8)|MAGdata[0];
	MAGy=(int16_t)(MAGdata[3]<<8)|MAGdata[2];
	MAGz=(int16_t)(MAGdata[5]<<8)|MAGdata[4];
	I2C_writeByte(MAG_ADDR, 0x0C, 0x00);
	I2C_writeByte(MAG_ADDR, MAG_CNTL1, 0b00010000);
	HAL_I2C_Mem_Read(&hi2c1, MAG_ADDR, 0x10, I2C_MEMADD_SIZE_8BIT, MAGdata, 3, 100);
	HAL_Delay(10);
*/

#undef MPU_ADDR
#undef MPU_WHO_AM_I
#undef MPU_SENS_START
#undef MPU_SENS_LEN
#undef MPU_FIFO_EN
#undef MPU_FIFO_SET
#undef MPU_PWR_MGMT_1
#undef MPU_PWR_MGMT_1_SET
#undef MPU_SMPLRT_DIV
#undef MPU_SMPLRT_DIV_SET
#undef MPU_GYRO_CONFIG
#undef MPU_GYRO_CONFIG_SET
#undef MPU_I2C_CONFIG
#undef MPU_I2C_CONFIG_SET
#undef MAG_ADDR
#undef MAG_WHO_AM_I
#undef MAG_CNTL1
#undef MAG_CNTL1_SET
#undef MAG_SENS_START
#undef MAG_SENS_LEN
#undef MAG_ST1
#undef MAG_RESET
#undef MAG_RESET_SET
#undef INTERRUPT_ENABLE
#undef _RAW_DATA_READY
#undef INT_PIN
#undef _INT_ANYRD_2CLEAR_ANY_READ
#undef	_ACTL_LOW
#undef _OPEN_DRAIN
#undef _LATCH_INT_EN_UNTIL_CLEARED

#undef MAG_READ_PERIOD_US
#undef ACC_GYRO_READ_PERIOD_US

#undef accXoffs
#undef accYoffs
#undef accZoffs

#undef accXmult
#undef accYmult
#undef accZmult

#undef gyroXoffs
#undef gyroYoffs
#undef gyroZoffs

#undef gyroXmult
#undef gyroYmult
#undef gyroZmult

#undef magXOffs
#undef magYOffs
#undef magZOffs

#undef magXmult
#undef magYmult
#undef magZmult

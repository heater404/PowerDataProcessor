#pragma once
#include "../SensorCfg/DtSensorCfg.h"
#define SIF2610_I2C_DEV_ADDR 0x20
#define SIF2610_I2C_DEV_PRO_COVER(value) ((value&0x000000FF)<<24|(value&0xFF000000)>>24|(value&0x00FF0000)>>8|(value&0x0000FF00)<<8)
class DtComm
{
private:
	/// <summary>
	/// 设备ID
	/// </summary>
	int devID;
	DtComm();
public:
	/// <summary>
	/// 单例对象
	/// </summary>
	/// <returns></returns>
	static DtComm* Instance();
	/// <summary>
	/// 打开设备
	/// </summary>
	/// <returns>
	/// @retval DT_ERROR_OK：打开设备成功
	/// @retval DT_ERROR_FAILD：打开设备失败
	/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
	/// @retval DT_ERROR_PARAMETER_INVALID：参数无效
	/// </returns>
	int OpenDevice();
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	int CloseDevice();
	int InitDevice(char* path);

	int StartStream(unsigned short width, unsigned short height);
	int StopStream();
	void MIPIRaw12ToP12(unsigned char* pIn, unsigned char* pOut, unsigned short DesW, unsigned short DesH);

	int GetSensorImage(unsigned char* pOutBuf);

	int WriteSif2610Reg(uint16_t regAddr, uint32_t regVlaue);
	int ReadSif2610Reg(uint16_t regAdr, uint32_t* regValue);

	int LoadSif2610Regs(char* path);
};




#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <dtccm2.h>
#include "DtComm.h"
#include "../IniFileHelper/inifile.h"
#include <iomanip>
using namespace inifile;
using namespace std;
#pragma comment(lib,"lib\\x64\\dtccm2.lib")
#pragma comment(lib,"lib\\x64\\DtccmKit64.lib")

DtComm* DtComm::Instance()
{
	static DtComm comm;
	return &comm;
}
DtComm::DtComm() :devID(DEFAULT_DEV_ID)
{

}
 
int DtComm::OpenDevice()
{
	int ret = DT_ERROR_OK;

	char* deviceName[16] = { 0 };
	int deviceNum;

	ret = EnumerateDevice(deviceName, 16, &deviceNum);
	if (ret != DT_ERROR_OK)
	{
		printf("EnumerateDevice Fail:%d", ret);
		return ret;
	}

	if (deviceNum <= 0)
	{
		printf("deviceNum<=0:%d", ret);
		return ret;
	}

	ret = _DTCCM2_H_::OpenDevice(deviceName[0], &devID, 0);
	if (ret != DT_ERROR_OK)
	{
		printf("OpenDevice Fail:%d", ret);
		return ret;
	}

	unsigned long verBuf[4] = { 0 };
	if (DT_ERROR_OK == GetLibVersion(verBuf, devID))
		printf("LIB Ver:%d.%d.%d.%d\r\n",
			verBuf[0], verBuf[1], verBuf[2], verBuf[3]);

	printf("device:%s open sucessful!!!\r\n", deviceName[0]);
	return ret;
}

int DtComm::CloseDevice()
{
	return	_DTCCM2_H_::CloseDevice();
}

int DtComm::InitDevice(char* path)
{
	DtSensorCfg cfg;
	return cfg.ConfigDtSensor(devID, path);
}

int DtComm::StartStream(unsigned short width, unsigned short height)
{
	SetSensorPort(SENSOR_PORT_MIPI, width, height, devID);

	int iRet = _DTCCM2_H_::InitDevice(NULL, width, height, SENSOR_PORT_MIPI, FORMAT_MIPI_RAW12, CHANNEL_A, NULL, devID);
	if (DT_ERROR_OK != iRet)
		printf("%s InitDevice failed with err:%d\r\n", __FUNCTION__, iRet);

	iRet = SetRoiEx(0, 0, width, height, 0, 0, 1, 1, TRUE, devID);
	if (DT_ERROR_OK != iRet)
		printf("%s SetRoiEx failed with err:%d\n", __FUNCTION__, iRet);

	unsigned int frameLength = (width * height * 2);
	iRet = OpenVideo(frameLength, devID);
	if (DT_ERROR_OK != iRet)
		printf("%s OpenVideo failed with err:%d\r\n", __FUNCTION__, iRet);

	return 0;
}

int DtComm::StopStream()
{
	int iRet = CloseVideo(devID);
	if (DT_ERROR_OK != iRet)
		printf("%s Stop Stream failed with err:%d\r\n", __FUNCTION__, iRet);

	return 0;
}

void DtComm::MIPIRaw12ToP12(unsigned char* pIn, unsigned char* pOut, unsigned short DesW, unsigned short DesH)
{
	unsigned int index = 0;
	unsigned int tSrcW = DesW * 3 / 2;
	unsigned int tBaseOffset;

	for (USHORT j = 0; j < DesH; j++) {
		for (USHORT i = 0; i < tSrcW; i += 3) {
			tBaseOffset = j * tSrcW + i;
			pOut[index++] = ((pIn[tBaseOffset] << 4) & 0xf0) + (pIn[tBaseOffset + 2] & 0xf);
			pOut[index++] = (pIn[tBaseOffset] >> 4) & 0xf;
			//pIn[i+2] >>= 4;
			pOut[index++] = ((pIn[tBaseOffset + 1] << 4) & 0xf0) + ((pIn[tBaseOffset + 2] >> 4) & 0xf);
			pOut[index++] = (pIn[tBaseOffset + 1] >> 4) & 0xf;
		}
	}
}

int DtComm::GetSensorImage(unsigned char* pOutBuf)
{
	FrameInfoEx tFrameInfo;
	unsigned char* pMipiBuf = NULL;

	int iRet = GrabFrameDirect(&pMipiBuf, &tFrameInfo, devID);
	if (DT_ERROR_OK != iRet) {
		printf("%s failed with err:%d \r\n", __FUNCTION__, iRet);
		return -1;
	}

	//printf("%s deltaTime:%8f\r\n", __FUNCTION__, tFrameInfo.fFETimeStamp - tFrameInfo.fFSTimeStamp);

	MIPIRaw12ToP12(pMipiBuf, pOutBuf, tFrameInfo.uWidth, tFrameInfo.uHeight);

	return 0;
}


int DtComm::WriteSif2610Reg(uint16_t regAddr, uint32_t regValue)
{
	uint32_t regValueNew = SIF2610_I2C_DEV_PRO_COVER(regValue);
	int ret = WriteSensorI2cEx(SIF2610_I2C_DEV_ADDR, regAddr, 2,
		(BYTE*)&regValueNew, 4, devID);

	std::cout << "Write " << hex << showbase << regAddr
		<< ":" << regValue << endl;
	return ret;
}

int DtComm::ReadSif2610Reg(uint16_t regAddr, uint32_t* regValue)
{
	int ret = ReadSensorI2cEx(SIF2610_I2C_DEV_ADDR, regAddr, 2,
		(BYTE*)regValue, 4, true, devID);

	*regValue = SIF2610_I2C_DEV_PRO_COVER(*regValue);
	std::cout << "Read " << hex << showbase << regAddr
		<< ":" <<*regValue << endl;
	return ret;
}

int DtComm::LoadSif2610Regs(char* path)
{
	IniFile iniFile;

	if (0 != iniFile.Load(path)) {
		cout << "can't load " << path << endl;
		return -1;
	}

	IniSection* section = iniFile.getSection("Sensor");
	if (section == 0)
		return -1;

	for (size_t i = 0; i < section->items.size(); i++)
	{
		IniItem item = section->items.at(i);
		uint32_t key, val;
		sscanf_s(item.key.c_str(), "%x", &key);
		sscanf_s(item.value.c_str(), "%x", &val);
		WriteSif2610Reg(key, val);
	}
	return 0;
}

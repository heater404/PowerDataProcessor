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

int DtComm::InitSPI()
{
	int ret = 0;
	ret = WriteSif2610Reg(SPI_CTRL, 0x0000000F);
	ret = WriteSif2610Reg(SPI_TIMING_CTRL, 0x13271313);

	return ret;
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
	int ret = 0;
	DtSensorCfg cfg;
	ret = cfg.ConfigDtSensor(devID, path);
	return ret;
}

int DtComm::StartStream(unsigned short width, unsigned short height)
{
	WriteSif2610Reg(SIF2610_SW_STANDBY, 1);
	WriteSif2610Reg(SIF2610_SEQUENCER_GO, 1);

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
	std::cout;
	std::cout << "Write 0x" << setfill('0') << setw(4) << hex << regAddr
		<< ":0x" << setfill('0') << setw(8) << hex << regValue << endl;
	return ret;
}

int DtComm::ReadSif2610Reg(uint16_t regAddr, uint32_t* regValue)
{
	int ret = ReadSensorI2cEx(SIF2610_I2C_DEV_ADDR, regAddr, 2,
		(BYTE*)regValue, 4, true, devID);

	*regValue = SIF2610_I2C_DEV_PRO_COVER(*regValue);
	std::cout << "Read 0x" << setfill('0') << setw(4) << hex << regAddr
		<< ":0x" << setfill('0') << setw(8) << hex << *regValue << endl;
	return ret;
}

int DtComm::WriteVcselDriver(uint8_t regAddr, uint8_t regValue)
{
	std::cout << "\n" << endl;
	bool ret = true;

	ret = WriteSif2610Reg(SPI_CHAN_CTRL(1, 1), JOIN_SPI_CHANNEL_CTRL1(0, 0, 1));

	if (ret) {
		ret = WriteSif2610Reg(SPI_CHAN_CTRL(1, 2), JOIN_SPI_CHANNEL_CTRL2(0, 0, 0, 0, 0));
	}

	if (ret) {
		ret = WriteSif2610Reg(SPI_TRANS_BUF_HEAD,
			(regValue << 24) |
			(SPI_WRITE_CTRL(regAddr) << 16) |
			(SPI_RECV_NUM(0) << 8) |
			(SPI_TRAN_NUM(2)));
	}

	if (ret) {
		ret = WriteSif2610Reg(SPI_GO, SPI_GO_CHANNEL(1));
	}

	std::cout << "Write VcselDriver 0x" << setfill('0') << setw(2) << hex << (uint32_t)regAddr
		<< ":0x" << setfill('0') << setw(2) << hex << (uint32_t)regValue << endl;

	return ret;
}

int DtComm::ReadVcselDriver(uint8_t regAddr, uint8_t* regValue)
{
	std::cout << "\n" << endl;
	bool ret = true;
	uint32_t r_value = 0;

	ret = WriteSif2610Reg(SPI_CHAN_CTRL(1, 1), JOIN_SPI_CHANNEL_CTRL1(0, 0, 1));

	if (ret) {
		ret = WriteSif2610Reg(SPI_CHAN_CTRL(1, 2), JOIN_SPI_CHANNEL_CTRL2(0, 0, 0, 0, 0));
	}

	if (ret) {
		ret = WriteSif2610Reg(SPI_TRANS_BUF_HEAD,
			(NULL_DATA << 24) |
			(SPI_READ_CTRL(regAddr) << 16) |
			(SPI_RECV_NUM(1) << 8) |
			(SPI_TRAN_NUM(1)));
	}

	if (ret) {
		ret = WriteSif2610Reg(SPI_GO, SPI_GO_CHANNEL(1));
	}

	if (ret) {
		Sleep(1);
		ret = ReadSif2610Reg(SPI_RECEV_BUF_HEAD, &r_value);
	}

	if (ret) {
		*regValue = (r_value & 0xFF);
	}
	else {
		*regValue = 0xFF;
	}

	std::cout << "Read VcselDriver 0x" << setfill('0') << setw(2) << hex << (uint32_t)(regAddr)
		<< ":0x" << setfill('0') << setw(2) << hex << (uint32_t)(*regValue) << endl;

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

int DtComm::LoadVcselDriverRegs(char* path)
{
	IniFile iniFile;

	if (0 != iniFile.Load(path)) {
		cout << "can't load " << path << endl;
		return -1;
	}

	IniSection* section = iniFile.getSection("VcselDriver");
	if (section == 0)
		return -1;

	uint8_t value;
	for (size_t i = 0; i < section->items.size(); i++)
	{
		IniItem item = section->items.at(i);
		uint32_t key, val;
		sscanf_s(item.key.c_str(), "%x", &key);
		sscanf_s(item.value.c_str(), "%x", &val);
		WriteVcselDriver(key, val);
		//ReadVcselDriver(key, &value);
	}
	return 0;
}

/// <summary>
/// 通过修改0x0429寄存器来控制帧率
/// </summary>
/// <param name="fps">MIPI帧帧率</param>
/// <returns></returns>
int DtComm::SetFPS(uint16_t fps)
{
	//计算需要填写的寄存器的值
	uint32_t value = (uint32_t)(1 * 1000 * 1000 / fps * (990 / 8));

	WriteSif2610Reg(0x0429, value);

	return 0;
}

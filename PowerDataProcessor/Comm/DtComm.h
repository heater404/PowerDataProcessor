#pragma once
#include "../SensorCfg/DtSensorCfg.h"
#include <cassert>
#define SIF2610_I2C_DEV_ADDR 0x20
#define SIF2610_I2C_DEV_PRO_COVER(value) ((value&0x000000FF)<<24|(value&0xFF000000)>>24|(value&0x00FF0000)>>8|(value&0x0000FF00)<<8)
#define SIF2610_SW_STANDBY 0xc29
#define SIF2610_SEQUENCER_GO 0x400
#define SPI_TRANS_BUF_HEAD 0x700 
#define SPI_RECEV_BUF_HEAD 0x710 
#define SPI_GO 0x720 
#define SPI_CTRL 0x721 
#define SPI_CHAN0_CTRL1 0x722 
#define SPI_CHAN1_CTRL1 0x723 
#define SPI_CHAN2_CTRL1 0x724 
#define SPI_CHAN0_CTRL2 0x725 
#define SPI_CHAN1_CTRL2 0x726 
#define SPI_CHAN2_CTRL2 0x727 
#define SPI_SYNC_TRI_DLY_CHAN0 0x728 
#define SPI_SYNC_TRI_DLY_CHAN1 0x729 
#define SPI_SYNC_TRI_DLY_CHAN2 0x72a 
#define SPI_TIMING_CTRL 0x72b 
#define SPI_WRITE_CTRL(addr) (addr & 0x7F) 
#define SPI_READ_CTRL(addr) (addr | 0x80) 
#define SPI_TRAN_NUM(num) num 
#define SPI_RECV_NUM(num) num 
#define NULL_DATA 0 
#define SPI_CHAN_CTRL(channel, ctrl) SPI_CHAN##channel##_CTRL##ctrl 
#define SPI_GO_CHANNEL(channel) (1<<(channel))
static inline uint32_t JOIN_SPI_CHANNEL_CTRL1(uint32_t trans_ptr, uint32_t receive_ptr, uint32_t transaction_num) 
{ 
	assert(trans_ptr>=0 && trans_ptr<64); 
	assert(receive_ptr>=0 && receive_ptr<64); 
	assert(transaction_num>=0 && trans_ptr<64); 
	return ((transaction_num<<16)|(receive_ptr<<8)|trans_ptr); 
}
static inline uint32_t JOIN_SPI_CHANNEL_CTRL2(uint32_t auto_trigger, uint32_t trigger_point, uint32_t tx_repeat_ptr, uint32_t rx_repeat_ptr, uint32_t auto_interval)
{ 
	assert(auto_trigger >= 0 && auto_trigger < 2); 
	assert(trigger_point >= 0 && trigger_point < 2); 
	assert(tx_repeat_ptr >= 0 && tx_repeat_ptr < 64); 
	assert(rx_repeat_ptr >= 0 && rx_repeat_ptr < 64); 
	assert(auto_interval >= 0 && auto_interval < 256); 
	return ((auto_interval << 24) | (rx_repeat_ptr << 16) | (tx_repeat_ptr << 8) | (trigger_point << 1) | auto_trigger);
}

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
	int InitSPI();
	int StartStream(unsigned short width, unsigned short height);
	int StopStream();
	void MIPIRaw12ToP12(unsigned char* pIn, unsigned char* pOut, unsigned short DesW, unsigned short DesH);

	int GetSensorImage(unsigned char* pOutBuf);

	int WriteSif2610Reg(uint16_t regAddr, uint32_t regVlaue);
	int ReadSif2610Reg(uint16_t regAdr, uint32_t* regValue);

	int WriteVcselDriver(uint8_t regAddr, uint8_t regValue);
	int ReadVcselDriver(uint8_t regAddr, uint8_t* regValue);

	int LoadSif2610Regs(char* path);
	int LoadVcselDriverRegs(char* path);

	int SetFPS(uint16_t fps);
};




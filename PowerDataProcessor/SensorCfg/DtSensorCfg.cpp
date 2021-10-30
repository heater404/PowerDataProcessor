using namespace std;
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include "DtSensorCfg.h"
#include <dtccm2.h>
#include "../IniFileHelper/inifile.h"
using namespace inifile;

int DtSensorCfg::SetSensorPower(BOOL bOnOff, int devID)
{
	int iRet = DT_ERROR_OK;

	SENSOR_POWER Power[7];
	int Volt[7];
	int Rise[7];
	BOOL OnOff[7];
	int Current[7];
	CURRENT_RANGE Range[7];
	int SampleSpeed[7];
	int PowerNum = 7;
	DWORD dwDevKit = GetKitType(devID);
	if (dwDevKit <= CTG970)
	{
		PowerNum = 5;
	}
	if (true)
	{
		Power[0] = POWER_AVDD;
		Volt[0] = (int)(config.PIXEL_mv); // 2.8V
		Current[0] = 600; // 500mA
		Rise[0] = 5000;
		OnOff[0] = TRUE;
		Range[0] = CURRENT_RANGE_MA;
		SampleSpeed[0] = 500;

		Power[1] = POWER_DOVDD;
		Volt[1] = (int)(config.ANALOG_mv); // 1.8V
		Current[1] = 600; // 500mA
		Rise[1] = 5000;
		OnOff[1] = TRUE;

		if (true)
		{
			Range[1] = CURRENT_RANGE_MA;
		}
		else
		{
			Range[1] = CURRENT_RANGE_UA;
		}
		SampleSpeed[1] = 500;

		Power[2] = POWER_DVDD;
		Volt[2] = (int)(config.DIGITAL_mv);// 1.2V
		Current[2] = 600;// 600mA
		Rise[2] = 5000;
		OnOff[2] = TRUE;
		Range[2] = CURRENT_RANGE_MA;
		SampleSpeed[2] = 500;

		Power[3] = POWER_AFVCC;
		Volt[3] = (int)(config.VCSEL_DRV_mv); // 3.3V
		Current[3] = 600; // 600mA
		Rise[3] = 5000;
		OnOff[3] = TRUE;
		Range[3] = CURRENT_RANGE_MA;
		SampleSpeed[3] = 500;

		if (dwDevKit == MU950_TOF)
		{
			Power[4] = POWER_AUX1;
		}
		else if (dwDevKit == G40 || dwDevKit == G41 || dwDevKit == G42 || dwDevKit == G22)
		{
			Power[4] = POWER_VPP2;
		}
		else
		{
			Power[4] = POWER_VPP;
		}
		Volt[4] = (int)(config.MIX_DRV_mv); // 2.8V
		Current[4] = 600; // 600mA
		Rise[4] = 5000;
		OnOff[4] = TRUE;
		Range[4] = CURRENT_RANGE_MA;
		SampleSpeed[4] = 500;

		if (dwDevKit == G40 || dwDevKit == G41 || dwDevKit == G42 || dwDevKit == G22)
		{
			Power[5] = POWER_AUX1;
		}
		else
		{
			Power[5] = POWER_OISVDD;
		}
		Volt[5] = (int)(0 * 1000);
		Current[5] = 600;
		Rise[5] = 5000;
		OnOff[5] = TRUE;
		Range[5] = CURRENT_RANGE_MA;
		SampleSpeed[5] = 500;

		Power[6] = POWER_AVDD2;
		Volt[6] = (int)(0 * 1000);
		Current[6] = 600;
		Rise[6] = 5000;
		OnOff[6] = TRUE;
		Range[6] = CURRENT_RANGE_MA;
		SampleSpeed[6] = 500;

		iRet = PmuSetRise(Power, Rise, PowerNum, devID);
		if (iRet != DT_ERROR_OK)
		{
			printf("PmuSetRise failed with err:%d\r\n", iRet);
			//return iRet;
		}

		iRet = PmuSetSampleSpeed(Power, SampleSpeed, PowerNum, devID);
		if (iRet != DT_ERROR_OK)
		{
			printf("PmuSetSampleSpeed failed with err:%d\r\n", iRet);
			//return iRet;
		}

#if 1          

		//Volt[0] = (int)(2.8f * 1000);
		//Volt[1] = (int)(1.8f * 1000);
		//Volt[2] = (int)(1.8f * 1000);
		//Volt[3] = (int)(2.8f * 1000);
		//Volt[4] = (int)(2.8f * 1000);
		//Volt[5] = (int)(0 * 1000);
		//Volt[6] = (int)(2.8f * 1000);

		iRet = PmuSetVoltage(Power, Volt, PowerNum, devID);
		if (iRet != DT_ERROR_OK)
		{
			printf("PmuSetVoltage failed with err:%d\r\n", iRet);
			return iRet;
		}
		Sleep(20);
		OnOff[0] = TRUE;
		OnOff[1] = TRUE;
		OnOff[2] = TRUE;
		OnOff[3] = TRUE;
		OnOff[4] = TRUE;
		OnOff[5] = TRUE;
		OnOff[6] = TRUE;

		iRet = PmuSetOnOff(Power, OnOff, PowerNum, devID);
		if (iRet != DT_ERROR_OK)
		{
			printf("PmuSetOnOff failed with err:%d\r\n", iRet);
			return iRet;
		}
#endif

		iRet = PmuSetOcpCurrentLimit(Power, Current, PowerNum, devID);
		if (iRet != DT_ERROR_OK)
		{
			printf("PmuSetOcpCurrentLimit failed with err:%d\r\n", iRet);
			return iRet;
		}

		iRet = PmuSetCurrentRange(Power, Range, 7, devID);
		if (iRet != DT_ERROR_OK)
		{
			printf("PmuSetCurrentRange failed with err:%d\r\n", iRet);
			return iRet;
		}
		if (dwDevKit == G40 || dwDevKit == G41 || dwDevKit == G42 || dwDevKit == G22 || MU950_TOF)
		{
			PmuCurrentMeasurement_t sPmuCurrentMeasurementAux1;

			sPmuCurrentMeasurementAux1.PowerType = POWER_AUX1;
			sPmuCurrentMeasurementAux1.bPosEn = TRUE;
			sPmuCurrentMeasurementAux1.MeasureMode = MEASUREMENT_MODE_NORMAL;
			sPmuCurrentMeasurementAux1.lfTriggerPoint = 0;
			memset(sPmuCurrentMeasurementAux1.Rsv, 0x00, sizeof(sPmuCurrentMeasurementAux1.Rsv));

			iRet = PmuSetCurrentMeasurement(&sPmuCurrentMeasurementAux1, devID);
			if (iRet != DT_ERROR_OK)
			{
				printf("PmuSetCurrentMeasurement failed with err:%d\r\n", iRet);
				return iRet;
			}

			PmuCurrentMeasurement_t sPmuCurrentMeasurementDvdd;

			sPmuCurrentMeasurementDvdd.PowerType = POWER_DVDD;
			sPmuCurrentMeasurementDvdd.bPosEn = TRUE;
			sPmuCurrentMeasurementDvdd.lfTriggerPoint = 0;
			memset(sPmuCurrentMeasurementDvdd.Rsv, 0x00, sizeof(sPmuCurrentMeasurementDvdd.Rsv));

			iRet = PmuSetCurrentMeasurement(&sPmuCurrentMeasurementDvdd, devID);
			if (iRet != DT_ERROR_OK)
			{
				printf("PmuSetCurrentMeasurement failed with err:%d\r\n", iRet);
				return iRet;
			}

		}
	}
	else
	{
		Power[0] = POWER_AVDD;
		Volt[0] = 0;
		OnOff[0] = FALSE;

		Power[1] = POWER_DOVDD;
		Volt[1] = 0;
		OnOff[1] = FALSE;

		Power[2] = POWER_DVDD;
		Volt[2] = 0;
		OnOff[2] = FALSE;

		Power[3] = POWER_AFVCC;
		Volt[3] = 0;
		OnOff[3] = FALSE;

		if (dwDevKit == MU950_TOF)
		{
			Power[4] = POWER_AUX1;
		}
		else if (dwDevKit == G40 || dwDevKit == G41 || dwDevKit == G42 || dwDevKit == G22)
		{
			Power[4] = POWER_VPP2;
		}
		else
		{
			Power[4] = POWER_VPP;
		}
		Volt[4] = 0;
		OnOff[4] = FALSE;

		if (dwDevKit == G40 || dwDevKit == G41 || dwDevKit == G42 || dwDevKit == G22)
		{
			Power[5] = POWER_AUX1;
		}
		else
		{
			Power[5] = POWER_OISVDD;
		}
		Volt[5] = 0;
		OnOff[5] = FALSE;

		Power[6] = POWER_AVDD2;
		Volt[6] = 0;
		OnOff[6] = FALSE;

		iRet = PmuSetVoltage(Power, Volt, PowerNum, devID);
		if (iRet != DT_ERROR_OK)
		{
			printf("PmuSetVoltage failed with err:%d\r\n", iRet);
			return iRet;
		}
		Sleep(150);

		iRet = PmuSetOnOff(Power, OnOff, PowerNum, devID);
		if (iRet != DT_ERROR_OK)
		{
			printf("PmuSetOnOff failed with err:%d\r\n", iRet);
			return iRet;
		}
	}

	return iRet;
}

int DtSensorCfg::InitSoftPin(int dev)
{
	int iRet = DT_ERROR_OK;

	UCHAR PinDef[26] = { PIN_NC };

	PinDef[0] = PIN_NC;
	PinDef[1] = PIN_D0;
	PinDef[2] = PIN_D2;
	PinDef[3] = PIN_D1;
	PinDef[4] = PIN_D3;
	PinDef[5] = PIN_D4;
	PinDef[6] = PIN_D5;
	PinDef[7] = PIN_D6;
	PinDef[8] = PIN_D7;
	PinDef[9] = PIN_D8;
	PinDef[10] = PIN_D9;
	PinDef[11] = PIN_NC;
	PinDef[12] = PIN_PCLK;
	PinDef[13] = PIN_HSYNC;
	PinDef[14] = PIN_VSYNC;
	PinDef[15] = PIN_NC;
	PinDef[16] = PIN_NC;		// CS
	PinDef[17] = PIN_MCLK;		// MCLK
	PinDef[18] = PIN_PWDN;		// PWDN				
	PinDef[19] = PIN_RESET;		// RST
	PinDef[20] = PIN_SCL;       // SCL            
	PinDef[21] = PIN_SDA;		// SDA                    
	PinDef[22] = PIN_GPIO2;		// PO2
	PinDef[23] = PIN_GPIO1;     // PO1      
	PinDef[24] = PIN_NC;        // PO3     
	PinDef[25] = PIN_NC;		// PO4

	iRet = SetSoftPinPullUp(TRUE, dev);
	if (iRet != DT_ERROR_OK)
	{
		printf("%s SetSoftPinPullUp failed with err:%d\r\n", __FUNCTION__, iRet);
		return iRet;
	}

	iRet = SetSoftPin(PinDef, dev);
	if (iRet != DT_ERROR_OK)
	{
		printf("%s SetSoftPin failed with err:%d\r\n", __FUNCTION__, iRet);
		return iRet;
	}

	iRet = EnableSoftPin(TRUE, dev);
	if (iRet != DT_ERROR_OK)
	{
		printf("%s EnableSoftPin failed with err:%d\r\n", __FUNCTION__, iRet);
		return iRet;
	}

	iRet = EnableGpio(TRUE, dev);
	if (iRet != DT_ERROR_OK)
	{
		printf("%s EnableGpio failed with err:%d\r\n", __FUNCTION__, iRet);
		return iRet;
	}

	return iRet;
	return 0;
}

int DtSensorCfg::SetSensorEnable(BYTE& pin, BOOL bEna, int devID)
{
	int iRet;

	if (bEna)
	{
		// ����reset pin, pwdn pin
		BYTE Pwdn2 = 0;
		BYTE Pwdn1 = 0;
		BYTE Reset = 0;

		SensorEnable(pin ^ RESET_H, TRUE, devID);
		Sleep(10);
		SensorEnable(pin, TRUE, devID);
		Sleep(50);

		Pwdn2 = pin & PWDN_H ? PWDN2_L : PWDN2_H;   //pwdn2 neg to pwdn1
		Pwdn1 = pin & PWDN_H ? PWDN_H : PWDN_L;     //pwdn1
		Reset = pin & RESET_H ? RESET_H : RESET_L;  //reset

		pin = Pwdn2 | Pwdn1 | Reset;

		iRet = SensorEnable(pin, 1, devID); //reset
	}
	else
	{
		iRet = SensorEnable(pin, FALSE, devID);
	}

	return iRet;
}

int DtSensorCfg::LoadConfig(char* path)
{
	IniFile iniFile;
	if (0 != iniFile.Load(path)) {
		cout << "can't load " << path << endl;
		return -1;
	}
	iniFile.GetIntValueOrDefault("NORMAL", "I2C_CLK_k", &config.I2C_CLK_k, SIF2610_I2C_CLK_k);
	iniFile.GetIntValueOrDefault("NORMAL", "EXT_CLK_FREQ_k", &config.EXT_CLK_FREQ_k, SIF2610_EXT_CLK_FREQ);
	iniFile.GetIntValueOrDefault("NORMAL", "RESET_PIN_NUMBER", &config.RESET_PIN_NUMBER, SIF2610_RESET_PIN_NUMBER);

	iniFile.GetIntValueOrDefault("MIPI", "MIPI_PHY_LANE_CNT", &config.MIPI_PHY_LANE_CNT, SIF2610_MIPI_PHY_LANE_CNT);
	iniFile.GetIntValueOrDefault("MIPI", "MIPI_PHY_TYPE", &config.MIPI_PHY_TYPE, SIF2610_MIPI_PHY_TYPE);

	iniFile.GetIntValueOrDefault("DPS", "PIXEL_mv", &config.PIXEL_mv, SIF2610_VOL_PIXEL_mv);
	iniFile.GetIntValueOrDefault("DPS", "ANALOG_mv", &config.ANALOG_mv, SIF2610_VOL_ANALOG_mv);
	iniFile.GetIntValueOrDefault("DPS", "DIGITAL_mv", &config.DIGITAL_mv, SIF2610_VOL_DIGITAL_mv);
	iniFile.GetIntValueOrDefault("DPS", "VCSEL_DRV_mv", &config.VCSEL_DRV_mv, SIF2610_VOL_VCSEL_DRV_mv);
	iniFile.GetIntValueOrDefault("DPS", "MIX_DRV_mv", &config.MIX_DRV_mv, SIF2610_VOL_MIX_DRV_mv);


	cout << "PIXEL(mv): " << config.PIXEL_mv << endl;
	cout << "ANALOG(mv): " << config.ANALOG_mv << endl;
	cout << "DIGITAL(mv): " << config.DIGITAL_mv << endl;
	cout << "VCSEL_DRV(mv): " << config.VCSEL_DRV_mv << endl;
	cout << "MIX_DRV(mv): " << config.MIX_DRV_mv << endl;
	cout << "I2C_CLK(k): " << config.I2C_CLK_k << endl;

	return 0;
}

int DtSensorCfg::ConfigDtSensor(int devID, char* cfgPath)
{
	int iRet = 0;

	if (LoadConfig(cfgPath))
	{
		printf("LoadConfig failed @%s\n", __FUNCTION__);
	}

	/* 初始化sensor电源 */
	iRet = SetSensorPower(true, devID);
	if (iRet != DT_ERROR_OK)
	{
		printf("%s SetSensorPower failed with err:%d\n", __FUNCTION__, iRet);
	}

	/* 初始化柔性接口 */
	iRet = InitSoftPin(PORT_MIPI);
	if (iRet != DT_ERROR_OK)
	{
		printf("%s InitSoftPin failed with err:%d\n", __FUNCTION__, iRet);
	}
	/* 初始化sensor时钟 */
	iRet = SetSensorClock(TRUE, config.EXT_CLK_FREQ_k, devID);
	if (iRet != DT_ERROR_OK)
	{
		printf("%s SetSensorClock failed with err:%d\n", __FUNCTION__, iRet);
	}

	Sleep(20);

	/* Mipi Port,Lane个数设置 */
	MipiCtrlEx_t sMipiCtrlEx;
	iRet = GetMipiCtrlEx(&sMipiCtrlEx, devID);
	if (iRet != DT_ERROR_OK)
	{
		printf("%s GetMipiCtrlEx failed with err:%d\n", __FUNCTION__, iRet);
	}

	sMipiCtrlEx.byPhyType = config.MIPI_PHY_TYPE;
	sMipiCtrlEx.byLaneCnt = config.MIPI_PHY_LANE_CNT;
	sMipiCtrlEx.dwCtrl &= (~MIPI_CTRL_LP_EN);

	// 启用MIPI LP-10状态检查
	// 注意MIPI_CTRL_CLK_LP10_CHK要求CLK Lane使用非连续时钟，并且同时启用了MIPI_CTRL_NON_CONT

	//sMipiCtrlEx.dwCtrl |= MIPI_CTRL_CLK_LP01_CHK|MIPI_CTRL_LP_EN|MIPI_CTRL_DAT_LP01_CHK|MIPI_CTRL_NON_CONT;

	sMipiCtrlEx.dwCtrl |= MIPI_CTRL_NON_CONT;
	// some sensor will need to open MIPI_CTRL_LP_EN , some sensor is not ; if mipi is unstall , debug it ;
	iRet = SetMipiCtrlEx(&sMipiCtrlEx, devID);
	if (iRet != DT_ERROR_OK)
	{
		printf("%s SetMipiCtrlEx failed with err:%d\n", __FUNCTION__, iRet);
	}

	/* 等待时钟或PLL稳定 */
	Sleep(50);

	BYTE resetPin = config.RESET_PIN_NUMBER;
	/* 使能Sensor， Sensor进入工作状态，一般是使复位管脚设为高电平 */
	iRet = SetSensorEnable(resetPin, TRUE, devID);
	if (iRet != DT_ERROR_OK)
	{
		printf("%s SetSensorEnable failed with err:%d\n", __FUNCTION__, iRet);
	}
	Sleep(40);

	SetI2CInterval(0, devID);
	//SetSensorI2cRate(FALSE,m_nm_devID);
	SetSensorI2cRateEx(config.I2C_CLK_k, devID); //can setting 100-1000K; 
	SetSensorI2cPullupResistor(PULLUP_RESISTOR_4_7K, devID);
	SetSensorI2cAckWait(100, devID);// 


	iRet = SetMipiImageVC(0x0, TRUE, CHANNEL_A, devID); //Virtal channel will use this function to filter ; 

	iRet = SetRawFormat(RAW_RGGB, devID);
	if (iRet != DT_ERROR_OK)
	{
		printf("%s SetRawFormat failed with err:%d\n", __FUNCTION__, iRet);
	}
	return 0;
}

DtSensorCfg::DtSensorCfg()
{

}

#pragma once

#define SIF2610_EXT_CLK_FREQ	    (198) //19.8MHz*10
#define SIF2610_I2C_CLK_k           (400)
#define SIF2610_RESET_PIN_NUMBER    (0x02)

#define SIF2610_MIPI_PHY_LANE_CNT   (2)
#define SIF2610_MIPI_PHY_TYPE       (MIPI_DPHY_1_5G)

#define SIF2610_VOL_PIXEL_mv        (2700)
#define SIF2610_VOL_ANALOG_mv       (1500)
#define SIF2610_VOL_DIGITAL_mv      (1200)
#define SIF2610_VOL_VCSEL_DRV_mv    (3300)
#define SIF2610_VOL_MIX_DRV_mv      (2700)

class DtSensorCfg
{
public:
	int ConfigDtSensor(int devID, char* path);
	DtSensorCfg();

private:
	struct Config
	{
		int EXT_CLK_FREQ_k;
		int	I2C_CLK_k;
		int RESET_PIN_NUMBER;

		int MIPI_PHY_LANE_CNT;
		int MIPI_PHY_TYPE;

		int PIXEL_mv;
		int ANALOG_mv;
		int DIGITAL_mv;
		int VCSEL_DRV_mv;
		int MIX_DRV_mv;
		
	} config;

	int SetSensorPower(BOOL bOnOff, int devID);
	int InitSoftPin(int dev);
	int SetSensorEnable(BYTE& pin, BOOL bEna, int devID);
	int LoadConfig(char* path);
};
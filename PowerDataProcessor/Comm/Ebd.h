/**
 * @file ebd.h
 * @author liang.li@si-in.com
 * @date 2020-Oct-19
 * @brief Embedded data structure and related
 * @see EBD excel description file
 */
#define EBD_Data_ByteLen 116
#ifndef EBD_H
#define EBD_H
#include <stdint.h>

 /// EBD data type (1.manaully optimized for memory alignment. 2.currently only for little-endian machine!)
 // if you need to change it, do it very carefully.
typedef struct
{
	// ***ManualMemOpt: combine first uint8_t with later data to form a uint32
	union {
		// 32-bit bit field, using EBD first 4-byte to optimize memory alignment.
		uint32_t val;
		struct {
			// first byte, LSB->MSB
			uint32_t dummy1 : 8;

			uint32_t Pll_locked : 1;
			uint32_t Dll_locked : 1;
			uint32_t Dll_locked_fake : 1;
			uint32_t Dll_clksel : 1;
			uint32_t Dll_lckref : 1;
			uint32_t pll_prediv : 2;
			uint32_t dummy2 : 1;

			uint32_t pll_fbdiv : 7;
			uint32_t dummy3 : 1;

			uint32_t ckcont : 1;
			uint32_t lane_mode : 1;
			uint32_t mipi_vid : 2;
			uint32_t mipi_raw : 2;
			uint32_t dummy4 : 2;
		} bits;
	} anaConfig;

	uint16_t depth_map_cnt;
	uint16_t seq_cnt;
	uint16_t frame_cnt;
	uint16_t subframe_cnt;

	// IMPORTANT!: the following 3 uint8_t data fields are the cause of memory unalignment
	// to deal with this, manual optimization is applied with Mark: ***ManualMemOpt
	/// phase_ctrl.phase_cnt, only 4LSBs are used.
	uint8_t phase_cnt;
	/// sequencer.seq_sel, only 2LSBs are used
	uint8_t seq_sel;
	uint8_t seq_schedule;

	//***ManualMemOpt: this is a uint16_t, but separated due to optimization
	// access to seq_num will then be two memory access + shifting, but fortunately this value is not used often.
	uint8_t seq_num[2];

	union {
		uint8_t val;
		struct {
			uint8_t seq_work_mode : 4;
			uint8_t frame_mode : 3;
			uint8_t seq_position : 1;
		} bits;
	} PhaseCtrl1;

	union {
		uint8_t val;
		struct {
			uint8_t frame_int_en : 1;
			uint8_t frame_int_sel : 2;
			uint8_t dummy1 : 5;
		} bits;
	} SeqCtrl1;

	//***ManualMemOpt: this is a uint16_t, but separated due to optimization
	uint8_t seq_int_duty[2];

	//***ManualMemOpt: the following two was natrually a uint16 union, but due to optimization, separated.
	union {
		uint8_t val;
		struct {
			uint8_t seq_mod_freq : 4;
			uint8_t seq_mode_freq_sel : 3;
			uint8_t dummy1 : 1;
		} bits;
	} SeqCtrl2;

	union {
		uint8_t val;
		struct {
			uint8_t seq_phase : 5;
			uint8_t dummy2 : 3;
		} bits;
	} SeqCtrl3;

	union {
		uint8_t val;
		struct {
			uint8_t big_packet_en : 1;
			uint8_t big_packet_cfg : 2;
			uint8_t dummy1 : 5;
		} bits;
	} SeqPacket;

	uint16_t burst_num;
	/// seqeuncer.x_start, only 10LSBs are used.
	uint16_t x_start;

	uint8_t x_size;
	/// sequencer.x_step, only 5 LSBs are used.
	uint8_t x_step;

	/// seqeuncer.y_start, only 9LSBs are used.
	uint16_t y_start;
	/// sequencer.y_size, 9 LSBs are used.
	uint16_t y_size;

	/// sequencer.x_step, only 5 LSBs are used.
	uint8_t y_step;

	union {
		uint8_t val;
		struct {
			uint8_t v_mirror : 1;
			uint8_t h_mirror : 1;
			uint8_t dummy1 : 6;
		} bits;
	} MirrorConfig;

	union {
		uint16_t val;
		struct {
			uint16_t tsensor_data : 12;
			uint16_t tsensor_en : 1;
			uint16_t tsenso_oc_dis : 1;
			uint16_t dummy1 : 2;
		} bits;
	} TsensorData;

	/// reg_dig.tsensor_oc_data, 12 LSBs are used.
	uint16_t tsensor_oc_data;

	union {
		uint8_t val;
		struct {
			uint8_t mixdrv_dc : 5;
			uint8_t out_mode : 3;
		} bits;
	} MIXDRV_OUTMODE;

	/// 5 LSBs are used.
	uint8_t led_drv_dc;
	uint8_t SW_ID_LSBs[8];
	// uint32_t SW_ID_MSBs;
	uint8_t rx_rpt_prt0;
	uint8_t rx_rpt_prt1;
	uint8_t SPI_RX_BUFFER[64];
} EMBEDDED_DATA_t;
//manully optimized for memory alignment, no need to use (packed) attribute.


void AddInfoLineToEbd(byte* line)
{
	for (size_t i = 1; i < EBD_Data_ByteLen * 2; i++)
	{
		line[i] = (i >= EBD_Data_ByteLen) ? 0 : line[2 * i];
	}
}


//#define UNIT_TEST
#ifdef UNIT_TEST
/// EBD test data from digital simulation
uint8_t EBDTestData[116] = {
	0x00, 0x09, 0x19, 0x02, 0x01, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0xe4, 0x00,
	0x00, 0x60, 0x06, 0xce, 0x05, 0x4c, 0x0d, 0x05,
	0x01, 0x00, 0x00, 0x00, 0x80, 0x01, 0x00, 0x00,
	0x87, 0x01, 0x01, 0x03, 0xb0, 0x1d, 0x90, 0x06,
	0xad, 0x14, 0xd2, 0x9f, 0xd3, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00 };

/// TODO: change to an opensource unittest framework
uint32_t UT_CheckEBDStructure()
{
	EMBEDDED_DATA_t* pEBD = reinterpret_cast<EMBEDDED_DATA_t*>(EBDTestData);

	// check if struct based parsing is the same as manual parsing (using excel sheet as ref)
	// 1st byte
	assert(1 == pEBD->anaConfig.bits.Pll_locked);
	// 2nd byte
	assert(1 == pEBD->anaConfig.bits.Dll_clksel);
	// 3rd byte
	assert(0x19 == pEBD->anaConfig.bits.pll_fbdiv);
	// 4th byte
	assert(1 == pEBD->anaConfig.bits.lane_mode);
	// 5,6th byte
	assert(1 == pEBD->depth_map_cnt);
	// 9,10th byte
	assert(1 == pEBD->frame_cnt);
	// 11,12th byte
	assert(1 == pEBD->subframe_cnt);

	/// TODO: the above should be enough, but can add more test

	return true;
}

#endif

#endif

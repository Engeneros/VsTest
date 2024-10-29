#ifndef __VSDC3_REG_DEFS_H_
#define __VSDC3_REG_DEFS_H_

#define	MEM_SIZE				(0x003F0000)
#define MEM_RANGE				(MEM_SIZE-1)

#define OSC_SIZE				(0x001F8000)//MEMORY_SIZE//(MEMORY_SIZE - 10)
#define OSC_RANGE				(OSC_SIZE-1)

#define VME_BASE_ADDR_MASK	(0x007FFFF0)
#define VME_GCR_BASE		(0x007FFFF0)
#define VME_CH1_BASE		(0x007FFFE0)
#define VME_REPRO_BASE		(0x007FFFD0)
#define VME_CH3_BASE		(0x007FFFC0)
#define VME_INT_BUFF_MEM	(0x00400000)
#define VME_AUX_BASE		(0x003FFFF0)
#define VME_CH0_BASE		(0x003FFFE0)
#define VME_RESERVE			(0x003FFFD0)
#define VME_CH2_BASE		(0x003FFFC0)

#define VSDC_CH_COUNT       (2)
//#define SUPER_OLD_VSDC      (1)
//#define VSDC_CH_COUNT       (4)


//#if VSDC_CH_COUNT == 4
#define VME_CH3_OSC_MEM		((0x003F0000*3)/4)
#define VME_CH2_OSC_MEM		(0x003F0000/4)
#define VME_CH1_OSC_MEM		((0x003F0000)*2/4)
#define VME_CH0_OSC_MEM		(0x00000000)
static const unsigned int VME_CH_OSC_MEM[4] = {VME_CH0_OSC_MEM, VME_CH1_OSC_MEM, VME_CH2_OSC_MEM, VME_CH3_OSC_MEM};
//#else
//#ifdef SUPER_OLD_VSDC
//#define VME_CH1_OSC_MEM		(0x00400000)
//#else
//#define VME_CH1_OSC_MEM		(0x003F0000/2)
//#endif //SUPER_OLD_VSDC
//#define VME_CH0_OSC_MEM		(0x00000000)
//static const unsigned int VME_CH_OSC_MEM[VSDC_CH_COUNT] = {VME_CH0_OSC_MEM, VME_CH1_OSC_MEM};
//#endif //VSDC_CH_COUNT

//Global Registers
	#define 	DEV_ID 		((VME_GCR_BASE + 0) * 4)
		#define		DEV_ID_VAL	(0x01010A00)	//SWVER_SWREV_HWVER_HWREV
	#define GCR 			((VME_GCR_BASE + 1) * 4)
		#define PSTART_ALL	1
		#define PSTOP_ALL	2
		#define CALIB_ALL	4
		#define PCLR_ALL	8

	#define GSR				((VME_GCR_BASE + 2) * 4)
	#define REFH			((VME_GCR_BASE + 3) * 4)
	#define REFL			((VME_GCR_BASE + 4) * 4)
        #define SAVE 0x65766153//ASCI Save
		#define REWR 0x72576552//ASCI ReWr
        #define LOAD 0x64616f4c//ASCI Load
	#define TIME_QUANT  	((VME_GCR_BASE + 5) * 4)
		#define ADC_TIME 	0x3656BF95
	#define INT_LINE		((VME_GCR_BASE + 6) * 4)
    #define CAL_DAC_VAL		((VME_GCR_BASE + 7) * 4)

    #define AUZ_GND_MUX_DELAY ((VME_GCR_BASE + 8) * 4)
	#define AUZ_PAUSE_NUM	((VME_GCR_BASE + 9) * 4)
	#define AUZ_SW_NUM		((VME_GCR_BASE + 10) * 4)
	#define	AUZ_FULL_NUM	((VME_GCR_BASE + 11) * 4)
	#define CAL_PAUSE		((VME_GCR_BASE + 12) * 4)
	#define SW_GND_NUM		((VME_GCR_BASE + 13) * 4)
	#define GND_NUM			((VME_GCR_BASE + 14) * 4)
	#define GAIN_NUM		((VME_GCR_BASE + 15) * 4)


//Aux Registers
    #define TG_CSR              ((VME_AUX_BASE +  0) * 4)
    //control
#define  TG_RUN 			(1<<0)
#define	 TG_STOP			(1<<1)
    //status
#define	 TG_IS_RUNNING		(1<<0)
#define  TG_IRQ_FLAG		(1<<31)

    #define TG_SETTINGS			((VME_AUX_BASE +  1) * 4)
#define TG_START_SRC		(0xF << 0)
#define TG_START_TIMER_SRC	(0 << 0)
#define TG_START_SYNC1_SRC	(1 << 0)
#define TG_START_SYNC2_SRC	(2 << 0)
#define TG_START_SYNC3_SRC	(3 << 0)
#define TG_START_SYNC4_SRC	(4 << 0)
#define TG_START0_BP_SRC	(5 << 0)
#define TG_STOP0_BP_SRC     (6 << 0)
#define TG_START1_BP_SRC	(7 << 0)
#define TG_STOP1_BP_SRC     (8 << 0)

#define TG_SYNC_OUT_ENA     (1 << 4)
#define TG_SYNC_NEG_POL     (1 << 5)
#define TG_CH0_ENA			(1 <<  6)
#define TG_CH1_ENA			(1 <<  7)
#define TG_CH2_ENA			(1 <<  8)
#define TG_CH3_ENA			(1 <<  9)
#define TG_IRQ_ENA			(1 << 10)
#define TG_CYCLE_NUM		(0xFFFFF << 12)
    #define TG_CH0_PHASE		((VME_AUX_BASE +  2) * 4)
    #define TG_CH1_PHASE		((VME_AUX_BASE +  3) * 4)
    #define TG_CH2_PHASE		((VME_AUX_BASE +  4) * 4)
    #define TG_CH3_PHASE		((VME_AUX_BASE +  5) * 4)
    #define TG_TMR_PERIOD		((VME_AUX_BASE +  6) * 4)
    #define TG_IRQ_CSR			((VME_AUX_BASE +  7) * 4)

//BUFFER OF INTEGRALS

	#define INT_BUFF_STATUS		((VME_AUX_BASE +  8) * 4)
		#define INT_BUFF_STATUS_OFFSET		(0)
		#define INT_BUFF_STATUS_MASK		(0xFFFFFFF)
        #define INT_BUFF_STATUS_REG_MASK 	(INT_BUFF_STATUS_MASK << INT_BUFF_STATUS_OFFSET)
		#define INT_BUFF_IS_BUSY			(0x80000000)
		#define INT_BUFF_CHNUM_OFFSET		(28)
		#define INT_BUFF_CHNUM_MASK			(0xF)
        #define INT_BUFF_CHNUM_REG_MASK 	(INT_BUFF_CHNUM_MASK << INT_BUFF_CHNUM_OFFSET)
	#define INT_BUFF_SW_TIME	((VME_AUX_BASE +  9) * 4)
	#define INT_BUFF_SW_ZERO	((VME_AUX_BASE + 10) * 4)
	#define INT_BUFF_FULL_ZERO	((VME_AUX_BASE + 11) * 4)
	#define INT_BUFF_INTEGRAL	((VME_AUX_BASE + 12) * 4)
	#define INT_BUF_CTRL		((VME_AUX_BASE + 13) * 4)
	#define INT_BUFF_READ_POS	((VME_AUX_BASE + 14) * 4)
	#define INT_BUFF_WRITE_POS	((VME_AUX_BASE + 15) * 4)

static unsigned int VME_CH_BASE[4] = {VME_CH0_BASE, VME_CH1_BASE, VME_CH2_BASE, VME_CH3_BASE};


	#define ADC0_CSR_REG 	((VME_CH0_BASE + 0) * 4)	//input mux, gain, pstart, calib, sw_control -for debug
    #define ADC1_CSR_REG 	((VME_CH1_BASE + 0) * 4)
    #define ADC_CSR_REG(n) 	((VME_CH_BASE[n] + 0) * 4)
//===================================
		#define PSTART			0x01 // on read 1 if integration (channel state)
		#define PSTOP			0x02//
		#define CALIB			0x04 // on read 1 during calibration
		#define PCLR		   	0x08
	//-------------------------
		#define GAIN_ERR		0x20 // RO // if calib failure
		#define OVRNG			0x40 // W1C
		#define MEM_OVF			0x80 // W1C //if GAIN_HIGH REFH isn't set, would be REFL
		#define MISS_INT		0x100 // W1C //if irq aren't processed
	//=========================
		#define MISS_START		0x200
//		#define RESERVE_3		0x400 // +2V
//		#define RESERVE_4		0x800// +0.2V
        #define IRQ_FLAG		0x800 // if calib failure
	//-------------------------
        #define INTEGRAL_RDY	0x1000 // who knows when reset it
//		#define RESERVE_6		[31:13]
        #define CSR_W1C_MASK		(INTEGRAL_RDY | IRQ_FLAG | MISS_START | MEM_OVF | OVRNG | MISS_INT)
//		#define CSR_SW_BIT_MASK		(GAIN_ERR  | IRQ_FLAG)
//		#define CSR_HW_BIT_MASK		(PSTART | PSTOP | CALIB | PCLR | OVRNG | MEM_OVF | MISS_INT | MISS_START)

#define AVCH_RANGE_OFFSET           (24)
#define AVCH_RANGE_MASK				(0xF << AVCH_RANGE_OFFSET)
    #define AVCH_RANGE_ERROR			(0x0 << AVCH_RANGE_OFFSET)
    #define AVCH_RANGE_0V2				(0x1 << AVCH_RANGE_OFFSET)
    #define AVCH_RANGE_0V5				(0x2 << AVCH_RANGE_OFFSET)
    #define AVCH_RANGE_1V				(0x3 << AVCH_RANGE_OFFSET)
    #define AVCH_RANGE_2V				(0x4 << AVCH_RANGE_OFFSET)
    #define AVCH_RANGE_5V				(0x5 << AVCH_RANGE_OFFSET)
    #define AVCH_RANGE_10V				(0x6 << AVCH_RANGE_OFFSET)

//===================================
	#define ADC0_SETT_REG 	((VME_CH0_BASE + 1) * 4)
	#define ADC1_SETT_REG 	((VME_CH1_BASE + 1) * 4)
#define ADC_SETT_REG(n) 	((VME_CH_BASE[n] + 1) * 4)
//start source
#define ADC_START_SRC_OFFS  (0)
#define ADC_START_SRC_MUX   (0x7 << ADC_START_SRC_OFFS)
#define ADC_START_PSTART    0x0000
#define ADC_START_SYNCA     0x0001
#define ADC_START_SYNCB     0x0002
#define ADC_START_SYNCC     0x0003
#define ADC_START_SYNCD     0x0004
#define ADC_START_BP        0x0005
//stop source
#define ADC_STOP_SRC_OFFS  (3)
#define ADC_STOP_SRC_MUX   (0x7 << ADC_STOP_SRC_OFFS)
#define ADC_STOP_TIMER      0x0000
#define ADC_STOP_PSTOP      0x0008
#define ADC_STOP_SYNCA      0x0010
#define ADC_STOP_SYNCB      0x0018
#define ADC_STOP_SYNCC      0x0020
#define ADC_STOP_SYNCD      0x0028
#define ADC_STOP_BP         0x0030
//input mux
#define ADC_REF_H           0x0080
#define ADC_REF_L           0x00C0
#define ADC_GND             0x0040
#define ADC_SIG             0x0000
#define ADC_MUX_M_OFFS      (6)
#define ADC_MUX_M_MASK      (0x3 << ADC_MUX_M_OFFS)
//
#define ADC_GAIN2           0x0000
#define ADC_GAIN02          0x0100
//
#define ADC_NORM            0x0000
#define ADC_SINGLE          0x0200
//
#define ADC_AUCAL           0x0400
#define ADC_NO_AUCAL        0x0000
//
#define ADC_IRQ_EN          0x0800
#define ADC_IRQ_DIS         0x0000
//
#define ADC_SET_GAIN_DIS    0x1000
#define ADC_SET_AUTOZERO    0x2000
//===================================

	#define ADC0_TIME_REG 		((VME_CH0_BASE + 2) * 4)
	#define ADC1_TIME_REG 		((VME_CH1_BASE + 2) * 4)
    #define ADC_TIME_REG(n) 	((VME_CH_BASE[n] + 2) * 4)

	#define ADC0_ADC_AVGN		((VME_CH0_BASE + 3) * 4)
	#define ADC1_ADC_AVGN		((VME_CH1_BASE + 3) * 4)
    #define ADC_ADC_AVGN(n) 	((VME_CH_BASE[n] + 3) * 4)

//		#define AVG_NUM_MASK 31:0
	#define ADC0_ADC_READ		((VME_CH0_BASE + 4) * 4)
	#define ADC1_ADC_READ		((VME_CH1_BASE + 4) * 4)
    #define ADC_ADC_READ(n) 	((VME_CH_BASE[n] + 4) * 4)

	#define ADC0_ADC_WRITE		((VME_CH0_BASE + 5) * 4)
	#define ADC1_ADC_WRITE		((VME_CH1_BASE + 5) * 4)	//RO
    #define ADC_ADC_WRITE(n) 	((VME_CH_BASE[n] + 5) * 4)

//		#define MEM_RANGE	0x1FFF
		#define OSC_ADDR_MASK 	0x1FFF
	#define ADC0_ADC_DATA		((VME_CH0_BASE + 6) * 4)
	#define ADC1_ADC_DATA		((VME_CH1_BASE + 6) * 4)

    #define ADC_ADC_INT(n)      ((VME_CH_BASE[n] + 7) * 4)
    #define ADC0_ADC_INT		((VME_CH0_BASE + 7) * 4)
	#define ADC1_ADC_INT		((VME_CH1_BASE + 7) * 4)

    #define ADC_INT_CURR(n) 	((VME_CH_BASE[n] + 8) * 4)
    #define ADC0_INT_CURR		((VME_CH0_BASE + 8) * 4)
	#define ADC1_INT_CURR		((VME_CH1_BASE + 8) * 4)
	//not for everybody
    #define ADC_PR_TIME_REG(n) 	((VME_CH_BASE[n] + 9) * 4)
    #define ADC0_PR_TIME_REG	((VME_CH0_BASE + 9) * 4)
	#define ADC1_PR_TIME_REG	((VME_CH1_BASE + 9) * 4)

    #define ADC0_CAL_TEMP       ((VME_CH0_BASE + 10) * 4)
    #define ADC1_CAL_TEMP       ((VME_CH1_BASE + 10) * 4)

//    #define ADC0_DAC_VOLT       ((VME_CH0_BASE + 11) * 4)
//    #define ADC1_DAC_VOLT       ((VME_CH1_BASE + 11) * 4)
    #define ADC_BP_SYNC_MUX(n)  ((VME_CH_BASE[n] + 11) * 4)
#define BP_START_SEL_OFFS	(0)
#define BP_START_SEL_MASK 	(0x7 << BP_START_SEL_OFFS)
#define BP_STOP_SEL_OFFS	(4)
#define BP_STOP_SEL_MASK 	(0x7 << BP_STOP_SEL_OFFS)

	#define ADC0_INT_STATUS		((VME_CH0_BASE + 12) * 4)
	#define ADC1_INT_STATUS		((VME_CH1_BASE + 12) * 4)
    #define ADC_INT_STATUS(n) 	((VME_CH_BASE[n] + 12) * 4)
    #define ADC_INT_STATUS_MASK	(0xFF)
	//quant adc
	#define ADC0_SWOFF			((VME_CH0_BASE + 13) * 4)
	#define ADC1_SWOFF			((VME_CH1_BASE + 13) * 4)
    #define ADC_SWOFF(n)        ((VME_CH_BASE[n] + 13) * 4)

    #define ADC0_OFFS			((VME_CH0_BASE + 14) * 4)
	#define ADC1_OFFS			((VME_CH1_BASE + 14) * 4)
    #define ADC_OFFS(n)         ((VME_CH_BASE[n] + 14) * 4)

    #define ADC0_QUAN			((VME_CH0_BASE + 15) * 4)
	#define ADC1_QUAN			((VME_CH1_BASE + 15) * 4)
    #define ADC_QUAN(n)         ((VME_CH_BASE[n] + 15) * 4)

#endif// __VSDC3_REG_DEFS_H_

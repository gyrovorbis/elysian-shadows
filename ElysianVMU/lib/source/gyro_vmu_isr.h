#ifndef GYRO_VMU_ISR_H
#define GYRO_VMU_ISR_H

#ifdef __cplusplus
extern "C" {
#endif

//Interrupt Service Routine Addresses (guess there is no vector table)
#define ISR_ADDR_RESET              0x00    //Reset Interrupt (not really an interrupt? The fuck?
#define ISR_ADDR_EXT_INT0           0x03    //INT0 interrupt (external)
#define ISR_ADDR_EXT_INT1           0x0b    //INT1 interrupt (external)
#define ISR_ADDR_EXT_INT2_T0L       0x13    //INT2 interrupt (external) or T0L overflow
#define ISR_ADDR_EXT_INT3_TBASE     0x1b    //INT3 interrupt (external) or Base Timer overflow
#define ISR_ADDR_T0H                0x23    //T0H overflow
#define ISR_ADDR_T1                 0x2b    //T1H or T1L overflow
#define ISR_ADDR_SIO0               0x33    //SIO0 interrupt
#define ISR_ADDR_SIO1               0x3b    //SI01 interrupt
#define ISR_ADDR_RFB                0x43    //RFB interrupt (VMU<->VMU receive/detect)
#define ISR_P3_ADDR                 0x4b    //P3 interrupt

typedef enum VMU_INT {
    VMU_INT_EXT_INT0,
    VMU_INT_EXT_INT1,
    VMU_INT_EXT_INT2_T0L,
    VMU_INT_EXT_INT3_TBASE,
    VMU_INT_T0H,
    VMU_INT_T1,
    VMU_INT_SIO0,
    VMU_INT_SI01,
    VMU_INT_RFB,
    VMU_INT_P3,
    VMU_INT_COUNT
} VMU_INT;

inline static int gyVmuIsrAddr(VMU_INT intType) {
    const static unsigned char lut[VMU_INT_COUNT] = {
        ISR_ADDR_EXT_INT0,
        ISR_ADDR_EXT_INT1,
        ISR_ADDR_EXT_INT2_T0L,
        ISR_ADDR_EXT_INT3_TBASE,
        ISR_ADDR_T0H,
        ISR_ADDR_T1,
        ISR_ADDR_SIO0,
        ISR_ADDR_SIO1,
        ISR_ADDR_RFB,
        ISR_P3_ADDR
    };
    return lut[intType];
}



#ifdef __cplusplus
}
#endif

#endif // GYRO_VMU_ISR_H


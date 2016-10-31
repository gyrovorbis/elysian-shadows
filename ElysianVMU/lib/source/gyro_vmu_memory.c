#include <assert.h>
#include <gyro_system_api.h>
#include "gyro_vmu_memory.h"
#include "gyro_vmu_device.h"
#include "gyro_vmu_sfr.h"
#include "gyro_vmu_buzzer.h"

extern void lcdrefresh(void);

inline static int getParity(unsigned char n) {
    int p = 0;
    while(n) {
        p = !p;
        n &= (n-1);
    }
    return p;
}

int gyVmuMemRead(VMUDevice* dev, int addr) {

    /*
    if(addr >= SFR_ADDR_XRAM_BASE && addr <= SFR_ADDR_XRAM_END) {
        return dev->xram[dev->sfr[SFR_OFFSET(SFR_ADDR_XBNK)]][XRAM_OFFSET(addr)];
    }*/

    switch(addr) {
#if 0
    case 0x166:
        r = wram[0x1ff&((sfr[0x65]<<8)|sfr[0x64])];
        if(sfr[0x63]&0x10)
          if(!++sfr[0x64])
        sfr[0x65]^=1;
        return r;
#endif
    case SFR_ADDR_VTRBF: { //Reading from separate working memory
        int r = dev->wram[0x1ff&((dev->sfr[SFR_OFFSET(SFR_ADDR_VRMAD2)]<<8)|dev->sfr[SFR_OFFSET(SFR_ADDR_VRMAD1)])];
        //must auto-increment pointer if VSEL_INCE is set
        if(dev->sfr[SFR_OFFSET(SFR_ADDR_VSEL)]&SFR_VSEL_INCE_MASK)
            //check for 8-bit overflow after incrementing VRMAD1
            if(!++dev->sfr[SFR_OFFSET(SFR_ADDR_VRMAD1)])
                //carry 9th bit to VRMAD2
                dev->sfr[SFR_OFFSET(SFR_ADDR_VRMAD2)]^=1;
        return r;
    }
    //case SFR_ADDR_P7: return SFR_P7_P71_MASK;
    case 0x112:
      return dev->timer0._base.tl;
    case 0x114:
      return dev->timer0._base.th;
    case 0x11b:
      return dev->timer1._base.tl;
    case 0x11d:
      return dev->timer1._base.th;
    case 0x165:
      return 0xfe|(dev->sfr[0x65]&1);
    default: {
        int val = dev->memMap[addr/VMU_MEM_SEG_SIZE][addr%VMU_MEM_SEG_SIZE];
#ifdef VMU_DEBUG
        _gyLog(GY_DEBUG_VERBOSE, "READMEM[%x] = %d", addr, val);
#endif
        return val;
    }
    }


}


//PLAY SOUND
//HANDLE TIMERS
//CLOCK FREQUENCY
//HALT MODE
//LCD ENABLE/DISABLE
void gyVmuMemWrite(VMUDevice* dev, int addr, int val) {
    val &= 0xff; //clamp to single byte
#ifdef VMU_DEBUG
      _gyLog(GY_DEBUG_VERBOSE, "WRITEMEM[%x] = %d", addr, val);
#endif

    //if(addr >= SFR_ADDR_XRAM_BASE && addr <= SFR_ADDR_XRAM_END) {
        //dev->xram[dev->sfr[SFR_OFFSET(SFR_ADDR_XBNK)]][XRAM_OFFSET(addr)] = val;
        //lcdrefresh();
        //return;
    //}

    //Check for SFRs with side-effects
    switch(addr) {
    case SFR_ADDR_ACC: {
        if(dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)] != val) {
            //dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)] &= ~SFR_PSW_P_MASK|getParity(val);
            dev->sfr[0x01] = (dev->sfr[0x01]&0xfe)|getParity(val);
        }
        break;
    }
    case SFR_ADDR_VTRBF: //Writing to separate working memory
        dev->wram[0x1ff&((dev->sfr[SFR_OFFSET(SFR_ADDR_VRMAD2)]<<8)|dev->sfr[SFR_OFFSET(SFR_ADDR_VRMAD1)])] = val;
        //must auto-increment pointer if VSEL_INCE is set
        if(dev->sfr[SFR_OFFSET(SFR_ADDR_VSEL)]&SFR_VSEL_INCE_MASK)
            //check for 8-bit overflow after incrementing VRMAD1
            if(!++dev->sfr[SFR_OFFSET(SFR_ADDR_VRMAD1)])
                //carry 9th bit to VRMAD2
                dev->sfr[SFR_OFFSET(SFR_ADDR_VRMAD2)] ^= 1;
        return;
    case SFR_ADDR_EXT: {
        //changing CPU mode (change imem between BIOS in rom and APP in flash)
        unsigned mode = val&SFR_EXT_MASK;
        if((dev->sfr[SFR_OFFSET(SFR_ADDR_EXT)]&SFR_EXT_MASK) != mode) {
            //next instr must be JMPF, do it now, since imem is changing
            if(dev->pc > 0xfffd || dev->imem[dev->pc] != OPCODE_JMPF) {
                //assert(0);
                //EXT 0 changed without a proceeding JMPF!
            } else {
                dev->pc = (dev->imem[dev->pc+1]<<8) | dev->imem[dev->pc+2];
                //dev->imem = mode? dev->flash : dev->rom;
            }
        }
        break;
    }
    case SFR_ADDR_XBNK:{ //changing XRAM bank
        if(dev->sfr[SFR_OFFSET(SFR_ADDR_XBNK)] != val) {
            assert(val <= 2);
            dev->memMap[VMU_MEM_SEG_XRAM] = dev->xram[val];
          //  lcdrefresh();
        }
        break;
    }
    case SFR_ADDR_PSW: {
        unsigned char psw = dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)];
        //Check if changing RAM bank
        if((psw&SFR_PSW_RAMBK0_MASK) != (val&SFR_PSW_RAMBK0_MASK)) {
            unsigned newIndex = (val&SFR_PSW_RAMBK0_MASK)>>SFR_PSW_RAMBKO_POS;
            assert(newIndex == 0 || newIndex == 1);
            dev->memMap[VMU_MEM_SEG_GP1] = dev->ram[newIndex];
            dev->memMap[VMU_MEM_SEG_GP2] = &dev->ram[newIndex][VMU_MEM_SEG_SIZE];
        }
        break;
    }
    case SFR_ADDR_T0PRR:
        dev->timer0.tscale = 256-val;
        dev->timer0.tbase = 0;
        break;
    case SFR_ADDR_T0CNT:
        if(!(val&SFR_T0CNT_P0LRUN_MASK))
            dev->timer0._base.tl = dev->sfr[SFR_OFFSET(SFR_ADDR_T0LR)];
        if(!(val&SFR_T0CNT_P0HRUN_MASK))
            dev->timer0._base.th = dev->sfr[SFR_OFFSET(SFR_ADDR_T0HR)];
        break;
    case SFR_ADDR_T0LR:
        if(!(dev->sfr[SFR_OFFSET(SFR_ADDR_T0CNT)]&SFR_T0CNT_P0LRUN_MASK))
            dev->timer0._base.tl = val;
        break;
    case SFR_ADDR_T0HR:
        if(!(dev->sfr[SFR_OFFSET(SFR_ADDR_T0CNT)]&SFR_T0CNT_P0HRUN_MASK))
            dev->timer0._base.th = val;
        break;
    case SFR_ADDR_T1CNT:
        if(!(val&SFR_T1CNT_T1LRUN_MASK))
            dev->timer1._base.tl = dev->sfr[SFR_OFFSET(SFR_ADDR_T1LR)];
        if(!(val&SFR_T1CNT_T1HRUN_MASK))
            dev->timer1._base.th = dev->sfr[SFR_OFFSET(SFR_ADDR_T1HR)];
        break;
    case SFR_ADDR_T1LR:
        if(!(dev->sfr[SFR_OFFSET(SFR_ADDR_T1CNT)]&SFR_T1CNT_T1LRUN_MASK))
            dev->timer1._base.tl = val;
        break;
    case SFR_ADDR_T1HR:
        if(!(dev->sfr[SFR_OFFSET(SFR_ADDR_T1CNT)]&SFR_T1CNT_T1HRUN_MASK))
            dev->timer1._base.th = val;
        break;
    case SFR_ADDR_SCON0:
        dev->sfr[SFR_OFFSET(SFR_ADDR_SCON0)] = val;
        if(val & SFR_SCON0_CTRL_MASK) { //transfer started
            if((dev->sfr[SFR_OFFSET(SFR_ADDR_P1DDR)]&(SFR_P1DDR_P10DDR_MASK|SFR_P1DDR_P13DDR_MASK)) && //P1.SO0 and P1.SCK0 set as OUT pins
               (dev->sfr[SFR_OFFSET(SFR_ADDR_P1FCR)]&(SFR_P1FCR_P10FCR_MASK|SFR_P1FCR_P13FCR_MASK))) //SO0 and SCK0 sourced from serial interface
            {
                gyVmuSerialTcpSend(dev, dev->sfr[SFR_OFFSET(SFR_ADDR_SBUF0)]);
            }
        }
        return;
        //return; Let SBUF0 be written with the byte?
    case SFR_ADDR_SCON1:
        break;

    case SFR_ADDR_VCCR:
        if(val&SFR_VCCR_VCCR7_MASK) {
#ifdef VMU_DEBUG
            _gyLog(GY_DEBUG_VERBOSE, "LCD TURNED ON!!!");
#endif
        } else {
 #ifdef VMU_DEBUG
            _gyLog(GY_DEBUG_VERBOSE, "LCD TURNED OFF!!!!");
#endif
        }
       // lcdrefresh();
    default: break;
    }
    //do actual memory write

#if 0
    dev->memMap[(addr&VMU_MEM_SEG_MASK)>>VMU_MEM_SEG_POS][addr&VMU_MEM_OFFSET_MASK] = val;
#else
    dev->memMap[addr/VMU_MEM_SEG_SIZE][addr%VMU_MEM_SEG_SIZE] = val;
#endif
    if((addr >= SFR_ADDR_XRAM_BASE && addr <= SFR_ADDR_XRAM_END) || addr == SFR_ADDR_STAD || addr == SFR_ADDR_XBNK) {
       // lcdrefresh();
    }

    //Check for buzzer state change
    if(addr == SFR_ADDR_T1CNT || addr == SFR_ADDR_T1LR || addr == SFR_ADDR_T1LC) {
        if(dev->sfr[SFR_OFFSET(SFR_ADDR_T1CNT)]&SFR_T1CNT_T1LRUN_MASK) {
            if(dev->sfr[SFR_OFFSET(SFR_ADDR_T1CNT)]&SFR_T1CNT_ELDT1C_MASK) { //This bit must be set for buzzer output changes to be applied
                //if(mode == VMU_TIMER1_MODE_TIMER8_PULSE8) - Yes, this really should be correct. Must be in Mode #1.
                int pulseWidth = (dev->sfr[SFR_OFFSET(SFR_ADDR_T1LC)] - dev->sfr[SFR_OFFSET(SFR_ADDR_T1LR)]);
                int cycleLength = (256-dev->sfr[SFR_OFFSET(SFR_ADDR_T1LR)]);
                //183us with 32khz clock
                gyVmuBuzzerSoundPlay(dev, cycleLength, pulseWidth, 0);
            }
        } else {
            gyVmuBuzzerSoundStop(dev); //Stop sound output
        }
    }
}


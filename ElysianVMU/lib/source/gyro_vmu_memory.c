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

    case SFR_ADDR_P7: return SFR_P7_P71_MASK;
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
        int val;
#ifdef VMU_DEBUG
        _gyLog(GY_DEBUG_VERBOSE, "READMEM[%x] = %d", addr, dev->memMap[(addr&VMU_MEM_SEG_MASK)>>VMU_MEM_SEG_POS][addr&VMU_MEM_OFFSET_MASK]);
#endif

        //return dev->memMap[(addr>>7)&0x3][addr&0x7f];
#if 0
         val = dev->memMap[(addr&VMU_MEM_SEG_MASK)>>VMU_MEM_SEG_POS][addr&VMU_MEM_OFFSET_MASK];
#else
        val = dev->memMap[addr/VMU_MEM_SEG_SIZE][addr%VMU_MEM_SEG_SIZE];
#endif
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
    if(addr == SFR_ADDR_T1CNT || addr == SFR_ADDR_T1LR || addr == SFR_ADDR_T1LC) {
      if(dev->sfr[SFR_OFFSET(SFR_ADDR_T1CNT)]&SFR_T1CNT_T1LRUN_MASK) {
          //int mode = gyVmuTimer1ModeGet(dev);
          //if(mode == VMU_TIMER1_MODE_TIMER8_PULSE8) {
                int pulseWidth = (dev->sfr[SFR_OFFSET(SFR_ADDR_T1LC)] - dev->sfr[SFR_OFFSET(SFR_ADDR_T1LR)]);
                int cycleLength = (256-dev->sfr[SFR_OFFSET(SFR_ADDR_T1LR)]);
                //float dutyCycle = (float)pulseWidth/(float)cycleLength;
                //183us with 32khz clock
                gyVmuBuzzerPlaySound(cycleLength, pulseWidth, 0);
                //gyVmuBuzzerPlaySound(32768/((256-dev->sfr[SFR_OFFSET(SFR_ADDR_T1L)])*6));
         //}
      }
      else {
        gyVmuBuzzerPlaySound(-1, 0.0f, 0);
      }
    }
}


#if 0


void writemem(int addr, int value)
{
  value &= 0xff;
  if(addr<0x100) {
    ram[(sfr[0x01]&2)>>1][addr] = value;
    return;
  }
  if(addr>=0x180) {
    int b = sfr[0x25];
    if(b>2 || (b==2 && addr>=0x186))
      return;
    xram[b][addr-0x180]=value;
    if(lcdon)
      lcd_updated = 1;
  } else switch(addr) {
   case 0x100:
     sfr[0x01] = (sfr[0x01]&0xfe)|parity[value]; // the FUCK is parity for!?
     break;
   case 0x10d:
     if((value&1) != (sfr[0x0d]&1)) {
       if(pc>0xfffd || rom[pc]!=0x21)
     error_msg("EXT 0 changed without following JMPF.  pc = %04x",
           pc&0xffff);
       else
     pc = (rom[pc+1]<<8)|rom[pc+2];
       if(hasbios)
     rom = ((value&1)? flash : bios);
     }
     break;
   case 0x10e:
     switch(value&0xa0) {
      case 0x00: spd = 3000; break;
      case 0x20: spd = 164; break;
      case 0x80: spd = 6000; break;
      case 0xa0: spd = 328; break;
     }
     break;
   case 0x110:
     if(!(value&0x40))
       t0l = sfr[0x13];
     if(!(value&0x80))
       t0h = sfr[0x15];
     break;
   case 0x111:
     t0scale = 256-value;
     t0base = 0;
     break;
   case 0x113:
     if(!(sfr[0x10]&0x40))
       t0l = value;
     break;
   case 0x115:
     if(!(sfr[0x10]&0x80))
       t0h = value;
     break;
   case 0x118:
     if(!(value&0x40))
       t1l = sfr[0x1b];
     if(!(value&0x80))
       t1h = sfr[0x1d];
     break;
   case 0x11b:
     if(!(sfr[0x18]&0x40))
       t1l = value;
     break;
   case 0x11d:
     if(!(sfr[0x18]&0x80))
       t1h = value;
     break;
   case 0x122:
     if(lcdon)
       lcd_updated = 1;
     break;
   case 0x127:
     if((!!(value&0x80)) != lcdon) {
       lcdon = !!(value&0x80);
       //lcdrefresh();
     }
     break;
   case 0x166:
     wram[0x1ff&((sfr[0x65]<<8)|sfr[0x64])] = value;
     if(sfr[0x63]&0x10)
       if(!++sfr[0x64])
     sfr[0x65]^=1;
     return;
  }
  /*
    if(addr>0x10e && addr<0x120 && addr != 0x118)
      fprintf(stderr, "%04x: Write to %03x: %02x\n", pc, addr, value);
  */
  sfr[addr&0xff] = value;
  if(addr == 0x118 || addr == 0x11b) {
    /* Check for sound... */
    if(sfr[0x18]&0x40)
      sound(32768/((256-sfr[0x1b])*6));
    else
      sound(-1);
  }
}
#endif

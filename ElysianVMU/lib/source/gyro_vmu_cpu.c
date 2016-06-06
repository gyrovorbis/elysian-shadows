#include "gyro_vmu_instr.h"
#include "gyro_vmu_memory.h"
#include "gyro_vmu_sfr.h"
#include "gyro_vmu_display.h"
#include "gyro_vmu_device.h"
#include "gyro_vmu_isr.h"
#include "gyro_vmu_osc.h"
#include "gyro_vmu_timers.h"
#include "gyro_vmu_bios.h"
#include <gyro_system_api.h>

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <sys/time.h>
#include <time.h>



extern void lcdrefresh(void);

//writemem has to check for writing to SFRs and do special shit if so
// - changing oscillator speed, changing EXT bit, etc
//readmem has a side-effect for reading to wram in a certain config
//setting external control EXT register or whatever then JMP swaps from BIOS to USER
//USER PC points to Flash, BIOS PC points to ROM
//SGNEXT - applies an offset depending on address space (flash vs ROM)?
#define SGNEXT(n) ((n)&0x80? (n)-0x100:(n))

#define UCHAR_SUB_OV(a, b) \
    ((b < 1)?((UCHAR_MAX + b >= a)?0:0):((b<=a)?0:0))
//    ((b < 1)?((UCHAR_MAX + b >= a)?1:0):((b<=a)?1:0))

inline static int tobcd(int n) {
  return ((n/10)<<4)|(n%10);
}

inline static unsigned char rotl(unsigned char c) {
    return (c << 1) | (c >> 7);
}

inline static unsigned char rotr(unsigned char c) {
    return (c >> 1) | (c << 7);
}


inline static void _push(VMUDevice* dev, unsigned val) {
    //check stack overflo
    //gyVmuMemWrite(dev, ++dev->sfr[SFR_OFFSET(SFR_ADDR_SP)], val);
    //assert(dev->sfr[SFR_OFFSET(SFR_ADDR_SP)]+1 < RAM_STACK_ADDR_END); //STACK OVERFLOW!!!
#ifdef VMU_DEBUG
    _gyLog(GY_DEBUG_VERBOSE, "push[%x] = %d", dev->sfr[SFR_OFFSET(SFR_ADDR_SP)]+1, val);
#endif
    dev->ram[0][++dev->sfr[SFR_OFFSET(SFR_ADDR_SP)]] = val;
}

inline static int _pop(VMUDevice* dev) {
    //check stack underflow
   // assert(dev->sfr[SFR_OFFSET(SFR_ADDR_SP)] >= RAM_STACK_ADDR_BASE);
#ifdef VMU_DEBUG
    _gyLog(GY_DEBUG_VERBOSE, "pop[%x] = %d", dev->sfr[SFR_OFFSET(SFR_ADDR_SP)], dev->ram[0][dev->sfr[SFR_OFFSET(SFR_ADDR_SP)]]);
#endif
    return dev->ram[0][dev->sfr[SFR_OFFSET(SFR_ADDR_SP)]--];
}


static int _serviceInterrupts(VMUDevice* dev) {
    //Check for Port 3 interrupt
#if 0
    if((sfr[0x4e]&3)==3 && !imask)
      intreq |= 1<<9;
#endif
    if(!dev->intMask && (dev->sfr[SFR_OFFSET(SFR_ADDR_P3INT)]&(SFR_P3INT_P31INT_MASK|SFR_P3INT_P30INT_MASK)) ==
            (SFR_P3INT_P31INT_MASK|SFR_P3INT_P30INT_MASK))
        dev->intReq |= 1<<VMU_INT_P3;

    uint8_t ie = dev->sfr[SFR_OFFSET(SFR_ADDR_IE)];

    //Check if there are no interrupts, we're already in an interrupt, or we've disabled them.
    if(!dev->intReq || dev->intMask || !(ie&SFR_IE_IE7_MASK)) return 0;

    for(uint16_t r = 0; r < VMU_INT_COUNT; ++r) {
        if(dev->intReq&(1<<r)) {                //pending interrupt request
            dev->intReq &= ~(1<<r);             //clear request
            ++dev->intMask;                     //increment mask/depth
            _push(dev, dev->pc&0xff);
            _push(dev, (dev->pc&0xff00)>>8);    //push return address
#ifdef VMU_DEBUG
            _gyLog(GY_DEBUG_VERBOSE, "INTERRUPT - %d", r);
#endif
            dev->pc = gyVmuIsrAddr(r);          //jump to ISR address
            return 1;
        }
    }

    return 0; //ain't no pending interrupts...
}

uint16_t _operands[ADDR_MODE_COUNT] = { 0 };
int _addrRel = 0;

static void _instrFetchOperands(VMUDevice* dev) {
    uint32_t instr = 0;
    //unsigned int types[3] = { 0 };
    const unsigned argc = instrArgsArgc(_instrMap[dev->instrBytes[INSTR_BYTE_OPCODE]].args);

    if(argc == 0) return; //No additional operands, don't fucking bother

    //concatenate instruction bytes into single integer
    switch(_instrMap[dev->instrBytes[INSTR_BYTE_OPCODE]].bytes) {
    case 1:
        instr = dev->instrBytes[INSTR_BYTE_OPCODE];
        break;
    case 2:
        instr = dev->instrBytes[INSTR_BYTE_2] | (dev->instrBytes[INSTR_BYTE_OPCODE]<<8);
        break;
    case 3:
        instr = dev->instrBytes[INSTR_BYTE_3] | (dev->instrBytes[INSTR_BYTE_2]<<8) | (dev->instrBytes[INSTR_BYTE_OPCODE]<<16);
        break;
    default: assert(0);
    }

    //Have to specifically check for the two fucktardedly encoded instruction types that make no fucking sense...
    switch(_instrMap[dev->instrBytes[INSTR_BYTE_OPCODE]].args) {
    case INSTR_ARG_TYPES_PACK(INSTR_ARG_TYPE_B3, INSTR_ARG_TYPE_D9, INSTR_ARG_TYPE_R8):
        _operands[ADDR_MODE_REL] = (uint8_t)(instr&0xff);
        _addrRel = (instr&0xff);
        instr >>= 8;
    case INSTR_ARG_TYPES_PACK(INSTR_ARG_TYPE_B3, INSTR_ARG_TYPE_D9):
        _operands[ADDR_MODE_DIR] = (uint8_t)(instr&0xff);
        instr >>= 8;
        _operands[ADDR_MODE_BIT] = (uint8_t)(instr&0x7);
        instr >>= 3;
        _operands[ADDR_MODE_DIR] |= (instr&0x2)<<7; //MSB is not fucking contiguous here... YOU ASSHOLES, SANYO.
        break;
    default: //instructions that have been encoded without cocks in their mouths
        for(int i = (int)argc-1; i >= 0; --i) {
            unsigned argType = INSTR_ARG_TYPES_UNPACK(_instrMap[dev->instrBytes[INSTR_BYTE_OPCODE]].args, (unsigned)i);
            switch(argType) {
            default:
            case INSTR_ARG_TYPE_NONE:
                goto doneloop;
                break;
            case INSTR_ARG_TYPE_R8:
                _operands[ADDR_MODE_REL] = (int8_t)(instr&0xff);
                _addrRel = (instr&0xff);
                instr >>= 8;
                break;
            case INSTR_ARG_TYPE_R16:
                _operands[ADDR_MODE_REL] = (uint16_t)((instr>>8)&0xff);
                _operands[ADDR_MODE_REL] |= (uint16_t)((instr&0xff)<<8);
                _addrRel = (uint16_t)((instr>>8)&0xff);
                _addrRel |= (uint16_t)((instr&0xff)<<8);
                instr >>= 16;
                break;
            case INSTR_ARG_TYPE_I8:
                _operands[ADDR_MODE_IMM] = (uint8_t)(instr&0xff);
                instr >>= 8;
                break;
            case INSTR_ARG_TYPE_D9:
                _operands[ADDR_MODE_DIR] = (uint16_t)(instr&0x1ff);
                instr >>= 9;
                break;
            case INSTR_ARG_TYPE_N2:
                _operands[ADDR_MODE_IND]    = (gyVmuMemRead(dev,
                                                            (instr & 0x3) | //Bits 0-1 come from instruction
                                                            ((dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)]&(SFR_PSW_IRBK0_MASK|SFR_PSW_IRBK1_MASK))>>0x1u)) //Bits 2-3 come from PSW
                                               | (instr&0x2)<<0x7u); //MSB of pointer is bit 1 from instruction
                instr >>= 2;
                break;
            case INSTR_ARG_TYPE_A12: //So fucking stupid... Not contiguous.
                _operands[ADDR_MODE_ABS] = (uint16_t)(instr&0x7ff);
                _operands[ADDR_MODE_ABS] |= (uint16_t)((instr&0x1000)>>1);
                instr >>= 11;
                break;
            case INSTR_ARG_TYPE_A16:
                _operands[ADDR_MODE_ABS] = (uint16_t)(instr&0xffff);
                instr >>= 16;
                break;
            case INSTR_ARG_TYPE_B3:
                _operands[ADDR_MODE_BIT] = (instr&0x7);
                instr >>= 3;
                break;
            }
        }
        break;
    }
    unsigned char opcode = _instrMap[dev->instrBytes[INSTR_BYTE_OPCODE]].opcode;
    assert(instr < 256);
    unsigned char instrcode = _instrMap[instr<<(8-_instrMap[opcode].opBits)].opcode;
    if(opcode != instrcode) {
#ifdef VMU_DEBUG
        _gyLog(GY_DEBUG_VERBOSE, "instr - %x, opcode - %x\n", instrcode, opcode);
#endif
    }
    assert(instrcode == opcode);

doneloop:
    return;
}

static int _instrExecute(VMUDevice* dev) {

    _instrFetchOperands(dev);

    //execute instruction
    switch(_instrMap[dev->instrBytes[INSTR_BYTE_OPCODE]].opcode) {

    case OPCODE_NOP:
    default:
        break;
    case OPCODE_BR:
        //dev->pc += (signed char)_operands[ADDR_MODE_REL];
        dev->pc += SGNEXT(_addrRel);
        break;
    case OPCODE_LD:
        gyVmuMemWrite(dev,
                      SFR_ADDR_ACC,
                      gyVmuMemRead(dev, _operands[ADDR_MODE_DIR]));
            break;
    case OPCODE_LD_IND:
        gyVmuMemWrite(dev,
                      SFR_ADDR_ACC,
                      gyVmuMemRead(dev, _operands[ADDR_MODE_IND]));
        break;
    case OPCODE_CALL:
        //gyVmuMemWrite(dev, ++dev->sfr[SFR_OFFSET(SFR_ADDR_SP)], (dev->pc&0xff));
        _push(dev, (dev->pc&0xff));
        //gyVmuMemWrite(dev, ++dev->sfr[SFR_OFFSET(SFR_ADDR_SP)], ((dev->pc&0xff00)>>7u));
        _push(dev, (dev->pc&0xff00)>>8u);
        dev->pc &= ~0xfff;
        dev->pc |= _operands[ADDR_MODE_ABS];
        break;
    case OPCODE_CALLR:
        //gyVmuMemWrite(dev, ++dev->sfr[SFR_OFFSET(SFR_ADDR_SP)], (dev->pc&0xff));
        _push(dev, (dev->pc&0xff));
        //gyVmuMemWrite(dev, ++dev->sfr[SFR_OFFSET(SFR_ADDR_SP)], ((dev->pc&0xff00)>>7u));
        _push(dev, (dev->pc&0xff00)>>8u);
        dev->pc += (_operands[ADDR_MODE_REL]%65536)-1;
        break;
    case OPCODE_BRF:
        //pc = 0xffff&(pc+1+rom[pc]+((rom[(pc+1)&0xffff])<<8));
        dev->pc += (_operands[ADDR_MODE_REL]%65536)-1;
        break;
    case OPCODE_ST:
        gyVmuMemWrite(dev, _operands[ADDR_MODE_DIR], dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)]);
        break;
    case OPCODE_ST_IND:
        gyVmuMemWrite(dev, _operands[ADDR_MODE_IND], dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)]);
        break;
    case OPCODE_CALLF:
        _push(dev, (dev->pc&0xff));
        _push(dev, (dev->pc&0xff00)>>8u);
        dev->pc = _operands[ADDR_MODE_ABS];
        break;
    case OPCODE_JMPF:
        dev->pc = _operands[ADDR_MODE_ABS];
        break;
    case OPCODE_MOV:
        gyVmuMemWrite(dev, _operands[ADDR_MODE_DIR], _operands[ADDR_MODE_IMM]);
        break;
    case OPCODE_MOV_IND:
        gyVmuMemWrite(dev, _operands[ADDR_MODE_IND], _operands[ADDR_MODE_IMM]);
        break;
    case OPCODE_JMP:
        dev->pc &= ~0xfff;
        dev->pc |= _operands[ADDR_MODE_ABS];
        break;
    case OPCODE_MUL: {
            int temp    =   dev->sfr[SFR_OFFSET(SFR_ADDR_C)] | (dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)]<<8);
            temp        *=  dev->sfr[SFR_OFFSET(SFR_ADDR_B)];

            gyVmuMemWrite(dev, SFR_ADDR_C, (temp&0xff));
            gyVmuMemWrite(dev, SFR_ADDR_ACC, ((temp&0xff00)>>8));
            gyVmuMemWrite(dev, SFR_ADDR_B, ((temp&0xff0000)>>16));

            dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)]           &=  ~SFR_PSW_CY_MASK;
            if(temp>65535) {
                dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)]       |=  SFR_PSW_OV_MASK;
            } else {
                dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)]       &=  ~SFR_PSW_OV_MASK;
            }
        }
        break;
    case OPCODE_BEI: {
        int acc = gyVmuMemRead(dev, SFR_ADDR_ACC);
         gyVmuMemWrite(dev, 0x101, (gyVmuMemRead(dev, 0x101)&0x7f)|(acc<_operands[ADDR_MODE_IMM]? 0x80:0));
        if(acc == _operands[ADDR_MODE_IMM]) {
            //dev->pc += (signed char)_operands[ADDR_MODE_REL];
            dev->pc += SGNEXT(_addrRel);
        }
        break;
    }
    case OPCODE_BE: {
        int acc = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        int mem = gyVmuMemRead(dev, _operands[ADDR_MODE_DIR]);
        gyVmuMemWrite(dev, 0x101, (gyVmuMemRead(dev, 0x101)&0x7f)|(acc<mem? 0x80:0));
        if(acc == mem) {
            //dev->pc += (signed char)_operands[ADDR_MODE_REL];
            dev->pc += SGNEXT(_addrRel);
        }
        break;
    }
    case OPCODE_BE_IND: {
        int acc = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        int mem = gyVmuMemRead(dev, _operands[ADDR_MODE_IND]);
        gyVmuMemWrite(dev, 0x101, (gyVmuMemRead(dev, 0x101)&0x7f)|(acc<mem? 0x80:0));
        if(acc == mem) {
            //dev->pc += (signed char)_operands[ADDR_MODE_REL];
            dev->pc += SGNEXT(_addrRel);
        }
        break;
    }
    case OPCODE_DIV: {
        int r                     =   dev->sfr[SFR_OFFSET(SFR_ADDR_B)];
        int s;

        if(r) {
            int v = dev->sfr[SFR_OFFSET(SFR_ADDR_C)] | (dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)]<<8);
            s = v%r;
            r = v/r;
        } else {
            r = 0xff00 | dev->sfr[SFR_OFFSET(SFR_ADDR_C)];
            s = 0;
        }
        gyVmuMemWrite(dev, SFR_ADDR_B, s);
        gyVmuMemWrite(dev, SFR_ADDR_C, r&0xff);
        gyVmuMemWrite(dev, SFR_ADDR_ACC, (r&0xff00)>>8);
        dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)]           &=  ~SFR_PSW_CY_MASK;
        if(!s) {
            dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)]       |=  SFR_PSW_OV_MASK;
        } else {
            dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)]       &=  ~SFR_PSW_OV_MASK;
        }
    }
        break;
    case OPCODE_BNEI:{
       // writemem(0x101, (readmem(0x101)&0x7f)|(r<rom[pc]? 0x80:0));
        const int acc = gyVmuMemRead(dev, SFR_ADDR_ACC);
        dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)] = (dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)]&0x7f)|(acc<_operands[ADDR_MODE_IMM]? 0x80:0);
        if(acc != _operands[ADDR_MODE_IMM]) {
            //dev->pc += (signed char)_operands[ADDR_MODE_REL];
            dev->pc += SGNEXT(_addrRel);
        }
        break;
    }
    case OPCODE_BNE:{
        int acc = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        int mem = gyVmuMemRead(dev, _operands[ADDR_MODE_DIR]);
        dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)] = (dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)]&0x7f)|(acc<mem? 0x80:0);
        if(acc != mem) {
            //dev->pc += (signed char)_operands[ADDR_MODE_REL];
            dev->pc += SGNEXT(_addrRel);
        }
        break;
    }
    case OPCODE_BNE_IND: {
        //int acc = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        int mem = gyVmuMemRead(dev, _operands[ADDR_MODE_IND]);
        dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)] = (dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)]&0x7f)|(mem < _operands[ADDR_MODE_IMM]? 0x80:0);
        if(_operands[ADDR_MODE_IMM] != mem) {
            //dev->pc += (signed char)_operands[ADDR_MODE_REL];
            dev->pc += SGNEXT(_addrRel);
        }
        break;
    }
    case OPCODE_BPC: {
        int mem = gyVmuMemRead(dev, _operands[ADDR_MODE_DIR]);
        if(mem & (1<<_operands[ADDR_MODE_BIT])) {
            gyVmuMemWrite(dev, _operands[ADDR_MODE_DIR], (mem&~(1<<_operands[ADDR_MODE_BIT])));
            //dev->pc += (signed char)_operands[ADDR_MODE_REL];
            dev->pc += SGNEXT(_addrRel);
        }
        break;
    }
    case OPCODE_LDF:
        _gyLog(GY_DEBUG_ERROR, "Using unknown opcode LDF!");
        break;
    case OPCODE_STF:
        _gyLog(GY_DEBUG_ERROR, "Using unknown opcode STF!");
        break;
    case OPCODE_DBNZ: {
        int mem = gyVmuMemRead(dev, _operands[ADDR_MODE_DIR])-1;
        gyVmuMemWrite(dev, _operands[ADDR_MODE_DIR], mem);
        if(mem != 0) {
            //dev->pc += (signed char)_operands[ADDR_MODE_REL];
            dev->pc += SGNEXT(_addrRel);
        }
        break;
    }
    case OPCODE_DBNZ_IND:{
        int mem = gyVmuMemRead(dev, _operands[ADDR_MODE_IND])-1;
        gyVmuMemWrite(dev, _operands[ADDR_MODE_IND], mem);
        if(mem != 0) {
            //dev->pc += (signed char)_operands[ADDR_MODE_REL];
            dev->pc += SGNEXT(_addrRel);
        }
        break;
    }
    case OPCODE_PUSH:
        _push(dev, gyVmuMemRead(dev, _operands[ADDR_MODE_DIR]));
        break;
    case OPCODE_INC: {
        gyVmuMemWrite(dev, _operands[ADDR_MODE_DIR], gyVmuMemRead(dev, _operands[ADDR_MODE_DIR])+1);
        break;
    }
    case OPCODE_INC_IND: {
        gyVmuMemWrite(dev, _operands[ADDR_MODE_IND], gyVmuMemRead(dev, _operands[ADDR_MODE_IND])+1);
        break;
    }
    case OPCODE_BP:
        if(gyVmuMemRead(dev, _operands[ADDR_MODE_DIR])&(0x1<<_operands[ADDR_MODE_BIT])) {
            //dev->pc += (signed char)_operands[ADDR_MODE_REL];
            dev->pc += SGNEXT(_addrRel);
        }
        break;
    case OPCODE_POP:
        //gyVmuMemWrite(dev, _operands[ADDR_MODE_DIR], gyVmuMemRead(dev, dev->sfr[SFR_OFFSET(SFR_ADDR_SP)]--));
        gyVmuMemWrite(dev, _operands[ADDR_MODE_DIR], _pop(dev));
        break;
    case OPCODE_DEC:{
        gyVmuMemWrite(dev, _operands[ADDR_MODE_DIR],gyVmuMemRead(dev, _operands[ADDR_MODE_DIR])-1);
        break;
    }
    case OPCODE_DEC_IND:{
        gyVmuMemWrite(dev, _operands[ADDR_MODE_IND], gyVmuMemRead(dev, _operands[ADDR_MODE_IND])-1);
        break;
    }
    case OPCODE_BZ:
        if(!dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)]) {
            //dev->pc += (signed char)_operands[ADDR_MODE_REL];
            dev->pc += SGNEXT(_addrRel);
        }
        break;
    case OPCODE_ADDI: {
        int r = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        int s = _operands[ADDR_MODE_IMM];
        gyVmuMemWrite(dev, SFR_ADDR_ACC, r+s);
        gyVmuMemWrite(dev, 0x101, (gyVmuMemRead(dev, 0x101)&0x3b)|(r+s>255? 0x80:0)|
                 ((r&15)+(s&15)>15? 0x40:0)|((0x80&(~r^s)&(s^(r+s)))? 4:0));
        break;
    }
    case OPCODE_ADD: {
        int r = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        int s = gyVmuMemRead(dev, _operands[ADDR_MODE_DIR]);
        gyVmuMemWrite(dev, SFR_ADDR_ACC, r+s);
        gyVmuMemWrite(dev, 0x101, (gyVmuMemRead(dev, 0x101)&0x3b)|(r+s>255? 0x80:0)|
                 ((r&15)+(s&15)>15? 0x40:0)|((0x80&(~r^s)&(s^(r+s)))? 4:0));
        break;
    }
    case OPCODE_ADD_IND: {
        int r = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        int s = gyVmuMemRead(dev, _operands[ADDR_MODE_IND]);
        gyVmuMemWrite(dev, SFR_ADDR_ACC, r+s);
        gyVmuMemWrite(dev, 0x101, (gyVmuMemRead(dev, 0x101)&0x3b)|(r+s>255? 0x80:0)|
                 ((r&15)+(s&15)>15? 0x40:0)|((0x80&(~r^s)&(s^(r+s)))? 4:0));
        break;
    }
    case OPCODE_BN:
        if(!(gyVmuMemRead(dev, _operands[ADDR_MODE_DIR])&(0x1<<_operands[ADDR_MODE_BIT]))) {
            //dev->pc += (signed char)_operands[ADDR_MODE_REL];
            dev->pc += SGNEXT(_addrRel);
        }
        break;
    case OPCODE_BNZ: {
        int acc = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        if(acc) {
            //dev->pc += (signed char)_operands[ADDR_MODE_REL];
            dev->pc += SGNEXT(_addrRel);
            //dev->pc &= 0xfff;
        }
    }
        break;
    case OPCODE_ADDCI: {
        int r = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        int s = _operands[ADDR_MODE_IMM] + ((dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)]&SFR_PSW_CY_MASK)>>SFR_PSW_CY_POS);
        gyVmuMemWrite(dev, SFR_ADDR_ACC, r+s);
        gyVmuMemWrite(dev, 0x101, (gyVmuMemRead(dev, 0x101)&0x3b)|(r+s>255? 0x80:0)|
                 ((r&15)+(s&15)>15? 0x40:0)|((0x80&(~r^s)&(s^(r+s)))? 4:0));
        break;
    }
    case OPCODE_ADDC: {
        int r = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        int s = gyVmuMemRead(dev, _operands[ADDR_MODE_DIR]) + ((dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)]&SFR_PSW_CY_MASK)>>SFR_PSW_CY_POS);
        gyVmuMemWrite(dev, SFR_ADDR_ACC, r+s);
        gyVmuMemWrite(dev, 0x101, (gyVmuMemRead(dev, 0x101)&0x3b)|(r+s>255? 0x80:0)|
                 ((r&15)+(s&15)>15? 0x40:0)|((0x80&(~r^s)&(s^(r+s)))? 4:0));
        break;
    }
    case OPCODE_ADDC_IND: {
        int r = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        int s = gyVmuMemRead(dev, _operands[ADDR_MODE_IND]) + ((dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)]&SFR_PSW_CY_MASK)>>SFR_PSW_CY_POS);
        gyVmuMemWrite(dev, SFR_ADDR_ACC, r+s);
        gyVmuMemWrite(dev, 0x101, (gyVmuMemRead(dev, 0x101)&0x3b)|(r+s>255? 0x80:0)|
                 ((r&15)+(s&15)>15? 0x40:0)|((0x80&(~r^s)&(s^(r+s)))? 4:0));
        break;
    }
    case OPCODE_RET: {
        int r = _pop(dev)<<8u;//gyVmuMemRead(dev, dev->sfr[SFR_OFFSET(SFR_ADDR_SP)]--)<<8;
        r |= _pop(dev);//gyVmuMemRead(dev, dev->sfr[SFR_OFFSET(SFR_ADDR_SP)]--);
        dev->pc = r;
        break;
    }
    case OPCODE_SUBI: {
        int r = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        int s = _operands[ADDR_MODE_IMM];
        gyVmuMemWrite(dev, SFR_ADDR_ACC, r-s);
        gyVmuMemWrite(dev, 0x101, (gyVmuMemRead(dev, 0x101)&0x3b)|(r-s<0? 0x80:0)|
             ((r&15)-(s&15)<0? 0x40:0)|(UCHAR_SUB_OV(r,s)? 4:0));

        break;
    }
    case OPCODE_SUB:{
        int r = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        int s = gyVmuMemRead(dev, _operands[ADDR_MODE_DIR]);
        gyVmuMemWrite(dev, SFR_ADDR_ACC, r-s);
        gyVmuMemWrite(dev, 0x101, (gyVmuMemRead(dev, 0x101)&0x3b)|(r-s<0? 0x80:0)|
             ((r&15)-(s&15)<0? 0x40:0)|(UCHAR_SUB_OV(r,s)? 4:0));

        break;
    }
    case OPCODE_SUB_IND:{
        int r = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        int s = gyVmuMemRead(dev, _operands[ADDR_MODE_IND]);
        gyVmuMemWrite(dev, SFR_ADDR_ACC, r-s);
        gyVmuMemWrite(dev, 0x101, (gyVmuMemRead(dev, 0x101)&0x3b)|(r-s<0? 0x80:0)|
             ((r&15)-(s&15)<0? 0x40:0)|(UCHAR_SUB_OV(r,s)? 4:0));

        break;
    }
    case OPCODE_NOT1: {
        gyVmuMemWrite(dev, _operands[ADDR_MODE_DIR], gyVmuMemRead(dev, _operands[ADDR_MODE_DIR]) ^ (0x1u<<_operands[ADDR_MODE_BIT]));
        break;
    }
    case OPCODE_RETI: {
        int r = _pop(dev)<<8u;//gyVmuMemRead(dev, dev->sfr[SFR_OFFSET(SFR_ADDR_SP)]--)<<8;
        r |= _pop(dev);//gyVmuMemRead(dev, dev->sfr[SFR_OFFSET(SFR_ADDR_SP)]--);
        dev->pc = r;
        --dev->intMask;
        break;
    }
    case OPCODE_SUBCI: {
        int r = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        int s = _operands[ADDR_MODE_IMM];
        int c = ((dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)]&SFR_PSW_CY_MASK)>>SFR_PSW_CY_POS);
        gyVmuMemWrite(dev, SFR_ADDR_ACC, r-s-c);
        gyVmuMemWrite(dev, 0x101, (gyVmuMemRead(dev, 0x101)&0x3b)|(r-s-c<0? 0x80:0)|
             ((r&15)-(s&15)-c<0? 0x40:0)|(UCHAR_SUB_OV(r,s-c)? 4:0));
        break;
    }
    case OPCODE_SUBC: {
        int r = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        int s = gyVmuMemRead(dev, _operands[ADDR_MODE_DIR]);
        int c = ((dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)]&SFR_PSW_CY_MASK)>>SFR_PSW_CY_POS);
        gyVmuMemWrite(dev, SFR_ADDR_ACC, r-s-c);
        gyVmuMemWrite(dev, 0x101, (gyVmuMemRead(dev, 0x101)&0x3b)|(r-s-c<0? 0x80:0)|
             ((r&15)-(s&15)-c<0? 0x40:0)|(UCHAR_SUB_OV(r,s-c)? 4:0));

        break;
    }
    case OPCODE_SUBC_IND:{
        int r = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        int s = gyVmuMemRead(dev, _operands[ADDR_MODE_IND]);
        int c = ((dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)]&SFR_PSW_CY_MASK)>>SFR_PSW_CY_POS);
        gyVmuMemWrite(dev, SFR_ADDR_ACC, r-s-c);
        gyVmuMemWrite(dev, 0x101, (gyVmuMemRead(dev, 0x101)&0x3b)|(r-s-c<0? 0x80:0)|
             ((r&15)-(s&15)-c<0? 0x40:0)|(UCHAR_SUB_OV(r,s-c)? 4:0));

        break;
    }
    case OPCODE_ROR: {
        int r = gyVmuMemRead(dev, 0x100);
        gyVmuMemWrite(dev, 0x100, (r>>1)|((r&1)<<7));
        //gyVmuMemWrite(dev, SFR_ADDR_ACC, rotr(dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)]));
        break;
    }
    case OPCODE_LDC: //Load from IMEM (flash/rom) not ROM?
        gyVmuMemWrite(dev, SFR_ADDR_ACC, dev->imem[dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)]+(dev->sfr[SFR_OFFSET(SFR_ADDR_TRL)] | (dev->sfr[SFR_OFFSET(SFR_ADDR_TRH)]<<8u))]);
        break;
    case OPCODE_XCH: {
        int acc   = gyVmuMemRead(dev, SFR_ADDR_ACC);
        int mem   = gyVmuMemRead(dev, _operands[ADDR_MODE_DIR]);
        acc                 ^= mem;
        mem                 ^= acc;
        acc                 ^= mem;
        gyVmuMemWrite(dev, SFR_ADDR_ACC, acc);
        gyVmuMemWrite(dev, _operands[ADDR_MODE_DIR], mem);
        break;
    }
    case OPCODE_XCH_IND: {
        int acc   = gyVmuMemRead(dev, SFR_ADDR_ACC);
        int mem   = gyVmuMemRead(dev, _operands[ADDR_MODE_IND]);
        acc                 ^= mem;
        mem                 ^= acc;
        acc                 ^= mem;
        gyVmuMemWrite(dev, SFR_ADDR_ACC, acc);
        gyVmuMemWrite(dev, _operands[ADDR_MODE_IND], mem);
        break;
    }
    case OPCODE_CLR1: {
        gyVmuMemWrite(dev, _operands[ADDR_MODE_DIR], gyVmuMemRead(dev, _operands[ADDR_MODE_DIR])& ~(0x1u<<_operands[ADDR_MODE_BIT]));
        break;
    }
    case OPCODE_RORC: {
        int r = gyVmuMemRead(dev, 0x100);
        int s = gyVmuMemRead(dev, 0x101);
        gyVmuMemWrite(dev, 0x101, (s&0x7f)|((r&1)<<7));
        gyVmuMemWrite(dev, 0x100, (r>>1)|(s&0x80));
#if 0
        unsigned char acc   = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        unsigned char psw   = dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)];
        unsigned msb        = (psw&SFR_PSW_CY_MASK)>>SFR_PSW_CY_POS;
        unsigned cyb        = acc&0x1;
        acc >>= 1u;
        acc |= msb<<7u;
        gyVmuMemWrite(dev, SFR_ADDR_ACC, acc);
        if(cyb) {
            gyVmuMemWrite(dev, SFR_ADDR_PSW, psw|SFR_PSW_CY_MASK);
        } else {
            gyVmuMemWrite(dev, SFR_ADDR_PSW, psw&~SFR_PSW_CY_MASK);
        }
#endif
        break;
    }
    case OPCODE_ORI:
        gyVmuMemWrite(dev, SFR_ADDR_ACC, dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)]|_operands[ADDR_MODE_IMM]);
        break;
    case OPCODE_OR:
        gyVmuMemWrite(dev, SFR_ADDR_ACC, dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)]|gyVmuMemRead(dev, _operands[ADDR_MODE_DIR]));
        break;
    case OPCODE_OR_IND:
        gyVmuMemWrite(dev, SFR_ADDR_ACC, dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)]|gyVmuMemRead(dev, _operands[ADDR_MODE_IND]));
        break;
    case OPCODE_ROL: {
        int r = gyVmuMemRead(dev, 0x100);
        gyVmuMemWrite(dev, 0x100, (r<<1)|((r&0x80)>>7));
        //gyVmuMemWrite(dev, SFR_ADDR_ACC, rotl(dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)]));
        break;
    }
    case OPCODE_ANDI:
        gyVmuMemWrite(dev, SFR_ADDR_ACC, dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)]&_operands[ADDR_MODE_IMM]);
        break;
    case OPCODE_AND:
        gyVmuMemWrite(dev, SFR_ADDR_ACC, dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)]&gyVmuMemRead(dev, _operands[ADDR_MODE_DIR]));
        break;
    case OPCODE_AND_IND:
        gyVmuMemWrite(dev, SFR_ADDR_ACC, dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)]&gyVmuMemRead(dev, _operands[ADDR_MODE_IND]));
        break;
    case OPCODE_SET1: {
        gyVmuMemWrite(dev, _operands[ADDR_MODE_DIR], gyVmuMemRead(dev, _operands[ADDR_MODE_DIR])|(0x1u<<_operands[ADDR_MODE_BIT]));
        break;
    }
    case OPCODE_ROLC:  {
#if 0
        unsigned char acc   = dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)];
        unsigned char psw   = dev->sfr[SFR_OFFSET(SFR_ADDR_PSW)];
        unsigned lsb        = (psw&SFR_PSW_CY_MASK)>>SFR_PSW_CY_POS;
        unsigned cyb        = acc&0x8u;
        acc <<= 1u;
        acc |= lsb;
        gyVmuMemWrite(dev, SFR_ADDR_ACC, acc);
        if(cyb) {
            gyVmuMemWrite(dev, SFR_ADDR_PSW, psw|SFR_PSW_CY_MASK);
        } else {
            gyVmuMemWrite(dev, SFR_ADDR_PSW, psw&~SFR_PSW_CY_MASK);
        }
#else
        int r = gyVmuMemRead(dev, 0x100);
        int s = gyVmuMemRead(dev, 0x101);
        gyVmuMemWrite(dev, 0x101, (s&0x7f)|(r&0x80));
        gyVmuMemWrite(dev, 0x100, (r<<1)|((s&0x80)>>7));
#endif
        break;
    }
    case OPCODE_XORI:
        gyVmuMemWrite(dev, SFR_ADDR_ACC, dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)]^_operands[ADDR_MODE_IMM]);
        break;
    case OPCODE_XOR:
        gyVmuMemWrite(dev, SFR_ADDR_ACC, dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)]^gyVmuMemRead(dev, _operands[ADDR_MODE_DIR]));
        break;
    case OPCODE_XOR_IND:
        gyVmuMemWrite(dev, SFR_ADDR_ACC, dev->sfr[SFR_OFFSET(SFR_ADDR_ACC)]^gyVmuMemRead(dev, _operands[ADDR_MODE_IND]));
        break;
    }

    return 1;
}



static int _fetchNextInstruction(VMUDevice* device) {
    //clear current instruction cache
    device->instrBytesFetched  = 0;
    device->instr              = 0;

    do { //keep fetching bytes until entire instruction has been fetched
        device->instrBytes[device->instrBytesFetched++] = device->imem[device->pc++];
    } while(device->instrBytesFetched < _instrMap[device->instrBytes[INSTR_BYTE_OPCODE]].bytes);

    //Entire instruction has been loaded
    if(device->instrBytesFetched >= _instrMap[device->instrBytes[INSTR_BYTE_OPCODE]].bytes) {
        static int instrNum = 0;
#ifdef VMU_DEBUG
        _gyLog(GY_DEBUG_VERBOSE, "*************** [%d] PC - %x **************", ++instrNum, device->pc-_instrMap[device->instrBytes[INSTR_BYTE_OPCODE]].bytes+1);
        _gyPush();

        //_gyLog(GY_DEBUG_VERBOSE, "BYTE - %x", byte);


        _gyLog(GY_DEBUG_VERBOSE, "mnemonic - %s", _instrMap[device->instrBytes[INSTR_BYTE_OPCODE]].mnemonic);
#endif
        assert(_instrMap[device->instrBytes[INSTR_BYTE_OPCODE]].mnemonic);
        //if(_instrMap[device->instrBytes[INSTR_BYTE_OPCODE]].bytes >= 2) _gyLog(GY_DEBUG_VERBOSE, "byte[1] = %x", device->instrBytes[INSTR_BYTE_2]);
        //if(_instrMap[device->instrBytes[INSTR_BYTE_OPCODE]].bytes >= 3) _gyLog(GY_DEBUG_VERBOSE, "byte[2] = %x", device->instrBytes[INSTR_BYTE_3]);


        _instrExecute(device);
        gyVmuTimersUpdate(device);

        //check for firmware entry based
        if(!(device->sfr[0x0d]&1) /*&& !hasbios*/)
            if((device->pc = gyVmuBiosHandleCall(device)))
                //device->sfr[0x0d]|=1;
                gyVmuMemWrite(device, SFR_ADDR_EXT, gyVmuMemRead(device, SFR_ADDR_EXT)|1);


        //check for overflows and shit
        //if(_instructionMap[device->_instrBytes[INSTR_BYTE_OPCODE]].psw) {


        //}

    }

#ifdef VMU_DEBUG
    _gyPop(1);
#endif
    return 1; //keep fetching instruction in next clock-cycle
}


//API for polling registers
//API for memcpying to/from system memory
//Error callbacks
//Error logging
//API for setting clock frequency, display brightness, etc.
//API to dynamically execute instruction (lua_dostring equivalent)
//API for fetching clock cycles/performance
//LCD_Changed method so app code won't have to update VBOs and shit if not.
//Callback for VMU beep? Isn't that the only sound it can make?


/*Should be handled externally through direct iteration
void render(void) {

}*/

int reset(void) {


    //1 - Clear main RAM
   // memset(_ram, 0,

    //set program counter to 0x0000
    return 0;
}

int gyVmuCpuTick(VMUDevice* dev, float deltaTime) {

    //do timing in time domain, so when clock frequency changes, it's automatically handled
    float time = 0.0f;
    int cycle = 0;


    if(deltaTime >= gyVmuOscSecPerCycle(dev)) {
        while(time < deltaTime) {
            _fetchNextInstruction(dev);
            _serviceInterrupts(dev);
            time += gyVmuOscSecPerCycle(dev)*(float)_instrMap[dev->instrBytes[INSTR_BYTE_OPCODE]].cc;
            ++cycle;
        }
    }

    return 1;
}

int gyVmuCpuReset(VMUDevice* dev) {
    _gyLog(GY_DEBUG_VERBOSE, "Resetting VMU CPU.");
    //dev->imem = dev->rom;
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    dev->memMap[VMU_MEM_SEG_XRAM]   = dev->xram[VMU_XRAM_BANK_LCD_TOP];
    dev->memMap[VMU_MEM_SEG_SFR]    = dev->sfr;
    dev->memMap[VMU_MEM_SEG_GP1]    = dev->ram[VMU_RAM_BANK1];
    dev->memMap[VMU_MEM_SEG_GP2]    = &dev->ram[VMU_RAM_BANK1][VMU_MEM_SEG_SIZE];

    dev->sfr[0x06] = 0x7f;
    dev->sfr[0x4c] = 0xff;
    dev->sfr[0x01] = 0x02;

#if 0
    t0h = t0l = 0;
    t1h = t1l = 0;
    t0base = 0;
    t0scale = 256;
#else
    dev->timer0.tscale = 256;
#endif
    dev->ram[0][0x10] = tobcd(tm->tm_year/100+19);
    dev->ram[0][0x11] = tobcd(tm->tm_year%100);
    dev->ram[0][0x12] = tobcd(tm->tm_mon+1);
    dev->ram[0][0x13] = tobcd(tm->tm_mday);
    dev->ram[0][0x14] = tobcd(tm->tm_hour);
    dev->ram[0][0x15] = tobcd(tm->tm_min);
    dev->ram[0][0x17] = (tm->tm_year+1900)>>8;
    dev->ram[0][0x18] = (tm->tm_year+1900)&0xff;
    dev->ram[0][0x19] = tm->tm_mon+1;
    dev->ram[0][0x1a] = tm->tm_mday;
    dev->ram[0][0x1b] = tm->tm_hour;
    dev->ram[0][0x1c] = tm->tm_min;
    dev->ram[0][0x1d] = tm->tm_sec;
    dev->ram[0][0x31] = 0xff;

    if(dev->biosLoaded) {
        dev->sfr[0x0d] = 0;
        //dev->pc = 0x1f0; no fucking clue
    } else {
        dev->sfr[0x0d] = 1;
            dev->imem = dev->flash;
            dev->pc = 0;
            gyVmuMemWrite(dev, 0x125, 2);
            gyVmuMemWrite(dev, 0x182, 0x10);
            gyVmuMemWrite(dev, 0x125, 0);
            gyVmuMemWrite(dev, 0x127, 0x80);

    }
    dev->sfr[SFR_OFFSET(SFR_ADDR_IE)] = SFR_IE_IE7_MASK;
    gyVmuMemWrite(dev, 0x10e, 0x81);



    //BIOS DEFAULT VALUES
    gyVmuMemWrite(dev, SFR_ADDR_P1FCR,  0xbf);
    gyVmuMemWrite(dev, SFR_ADDR_P3INT,  0xfd);
    gyVmuMemWrite(dev, SFR_ADDR_ISL,    0xc0);
    gyVmuMemWrite(dev, SFR_ADDR_VSEL,   0xf4);
    gyVmuMemWrite(dev, SFR_ADDR_BTCR,   0x41);



    return 1;
}


#if 0
//interrupty shit
//imask seems to be an interrupt depth counter
//increment before calling interrupt
//reti decrements when returning

if((sfr[0x4e]&3)==3 && !imask)
  intreq |= 1<<9;

if(!intreq || imask || !(sfr[0x08]&0x80))   //don't bother checking interrupts
  continue;

for(r=0; r<10; r++)                         //10 different interrupts
  if(intreq & (1<<r))                       //check each one
break;                                      //run interrupt

intreq &= ~(1<<r);                          //clear interrupt bit
push(pc&0xff);                              //push current PC
push((pc&0xff00)>>8);
imask++;                                    //increment interrupt depth

//Jump to ISR?
pc = ((r&~1)<<3)+((r&1)?0xb:0x3);           //How the fuck does it know where to go?

}

#endif

//bool uninitialize(void);


//open bios
//ram dump
//ram reload
//tick update function
//tick render function
//use libgyro
//header of convenient extern pointers to locations in VMU memory for direct access/usage outside of ASM (registers, accumulators, etc)
//instructions need to respect # of cycles


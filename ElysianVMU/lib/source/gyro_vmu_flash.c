#include "gyro_vmu_flash.h"
#include "gyro_vmu_device.h"
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <gyro_system_api.h>
#include <gyro_file_api.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include "gyro_vmu_vms.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

int gyVmuFlashLoadImage(VMUDevice* dev, const char* path) {
    char tempPath[10000];
    int success = 0;
    strcpy(tempPath, path);
    const char*curTok  = strtok(tempPath, ".");
    const char*prevTok = NULL;
    char ext[100] = { 0 };

    _gyLog(GY_DEBUG_VERBOSE, "Loading Flash Image [%s]", path);
    _gyPush();

    while(curTok) {
        prevTok = curTok;
        curTok = strtok(NULL, ".");
    }

    if(prevTok) {
        for(unsigned i = 0; i < strlen(prevTok); ++i)
            ext[i] = tolower(prevTok[i]);

        if(!strcmp(ext, "vms")) {
            success = gyloadflash(dev, path);
        } else if(!strcmp(ext, "dci")) {
            success = gyVmuFlashLoadDCI(dev, path);
        } else if(!strcmp(ext, "vmi")) {
            VMIFileInfo info;
            if(gyVmuFlashLoadVMI(&info, path)) {
                gyVmuFlashPrintVMIFileInfo(&info);
            }

        } else {
            _gyLog(GY_DEBUG_ERROR, "Unknown extension! [%s]", prevTok);
        }


    } else {
        _gyLog(GY_DEBUG_ERROR, "No extension found on file!");

    }

    _gyPop(1);
    return success;

}

int gyVmuFlashLoadDCI(VMUDevice *dev, const char *path) {
    GYFile* file;

    _gyLog(GY_DEBUG_VERBOSE, "Loading DCM Flash image from file [%s].", path);
    _gyPush();

    if (!gyFileOpen(path, "rb", &file)) {
        _gyLog(GY_DEBUG_ERROR, "Could not open binary file for reading!");
        _gyPop(1);
        return 0;
    }

    //Clear ROM
    memset(dev->flash, 0, sizeof(dev->flash));

    size_t bytesRead   = 0;
    size_t bytesTotal  = 0;

    while(bytesTotal < sizeof(dev->flash)) {
        if(gyFileRead(file, dev->flash+bytesTotal, sizeof(dev->flash)-bytesTotal, &bytesRead)) {
            bytesTotal += bytesRead;
        } else break;
    }


    gyFileClose(&file);

    _gyLog(GY_DEBUG_VERBOSE, "Read %d bytes.", bytesTotal);
    assert(bytesTotal >= 0);
    assert(bytesTotal == sizeof(dev->flash));

    for(unsigned i = 0; i < bytesTotal; i += 4) {
        for(unsigned j = i; j < i + 4; ++j) {
            unsigned temp       = dev->flash[j];
            dev->flash[j]       = dev->flash[i+3-j];
            dev->flash[i+3-j]   = temp;
        }
    }

    _gyPop(0);
    return 1;
}

uint32_t gyVmuVMIChecksumGenerate(const VMIFileInfo *info) {
    uint32_t checksum = 0;
    unsigned char* byte = (void*)&checksum;

    byte[0] = info->vmsResourceName[0] & 'S';
    byte[1] = info->vmsResourceName[1] & 'E';
    byte[2] = info->vmsResourceName[2] & 'G';
    byte[3] = info->vmsResourceName[3] & 'A';

    return checksum;
}

void gyVmuFlashPrintVMIFileInfo(const VMIFileInfo* vmi) {
    char string[33]; //temporary buffer to null-terminate string fields

    _gyLog(GY_DEBUG_VERBOSE, "VMI File Attributes");
    _gyPush();
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u [%s]", "Checksum",        vmi->checksum,
           (vmi->checksum == gyVmuVMIChecksumGenerate(vmi))? "VALID" : "INVALID");
    memcpy(string, vmi->description, sizeof(vmi->description));
    string[sizeof(vmi->description)] = 0;
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Description",          string);
    memcpy(string, vmi->copyright, sizeof(vmi->copyright));
    string[sizeof(vmi->copyright)] = 0;
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "Copyright",            string);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "Year Created",         vmi->creationYear);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "Month Created",        vmi->creationMonth);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "Day Created",          vmi->creationDay);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "Hour Created",         vmi->creationHour);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "Minute Created",       vmi->creationMinute);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "Second Created",       vmi->creationSecond);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "Weekday Created",       vmi->creationWeekday);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "VMI Version",          vmi->vmiVersion);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "File Number",          vmi->fileNumber);
    memcpy(string, vmi->vmsResourceName, sizeof(vmi->vmsResourceName));
    string[sizeof(vmi->vmsResourceName)] = 0;
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "VMS Resource Name",    string);
    memcpy(string, vmi->fileNameOnVms, sizeof(vmi->fileNameOnVms));
    string[sizeof(vmi->fileNameOnVms)] = 0;
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40s", "File Name on VMS",     string);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u [%s|%s]", "File Mode",    vmi->fileMode,
           (vmi->fileMode&VMU_VMI_FILE_INFO_FILE_MODE_GAME_MASK)?       "GAME":"DATA",
           (vmi->fileMode&VMU_VMI_FILE_INFO_FILE_MODE_PROTECT_MASK)?    "COPY_PROTECTED":"COPY_OK");
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "Unknown Field",        vmi->unknown);
    _gyLog(GY_DEBUG_VERBOSE, "%-20s: %40u", "File Size",            vmi->fileSize);
    _gyPop(1);
}

int gyVmuFlashLoadVMI(VMIFileInfo* info, const char *path) {
    int success = 0;

    _gyLog(GY_DEBUG_VERBOSE, "Loading VMI Info [%s].", path);
    _gyPush();

    memset(info, 0, sizeof(VMIFileInfo));

    FILE* file = fopen(path, "rb");
    if(!file) {
        _gyLog(GY_DEBUG_ERROR, "Could not open binary file for reading!");

    } else {

        size_t elementsRead = fread(info, sizeof(VMIFileInfo), 1, file);
        fclose(file);

        //BETTER BE TRUE OR COMPILER PACKED OUR FUCKING STRUCT WRONG!!!!
        assert(sizeof(VMIFileInfo) == VMU_VMI_FILE_SIZE);

        if(!elementsRead) {
            _gyLog(GY_DEBUG_ERROR, "Could not read full %d bytes!", VMU_VMI_FILE_SIZE);
        } else {
            success = 1;
        }

    }

    _gyPop(1);

    return success;

}

void gyVmuVmiFileInfoResourceNameGet(const VMIFileInfo* info, char* string) {
    memcpy(string, info->vmsResourceName, sizeof(info->vmsResourceName));
    string[sizeof(info->vmsResourceName)] = 0;
}




//FAKE FLASH IMAGE
void gyfakeflash(VMUDevice* dev, char *filename, int sz)
{
  unsigned char *root, *fat, *dir;
  time_t t = time(NULL);
  struct tm *tm = localtime(&t);
  int i;
  char *fn2;

  if((fn2 = strrchr(filename, '/'))!=NULL)
    filename = fn2+1;
  else if((fn2 = strrchr(filename, '\\'))!=NULL)
    filename = fn2+1;
  memset(dev->flash+241*512, 0, sizeof(dev->flash)-241*512);
  root = dev->flash+255*512;
  fat = dev->flash+254*512;
  dir = dev->flash+253*512;
  sz = ((sz+511)>>9);
  for(i=0; i<256*2; i+=2) {
    fat[i] = 0xfc;
    fat[i+1] = 0xff;
  }
  for(i=0; i<sz; i++) {
    fat[2*i] = i+1;
    fat[2*i+1] = 0;
  }
  if((--i)>=0) {
    fat[2*i] = 0xfa;
    fat[2*i+1] = 0xff;
  }
  fat[254*2] = 0xfa;
  fat[254*2+1] = 0xff;
  fat[255*2] = 0xfa;
  fat[255*2+1] = 0xff;
  for(i=253; i>241; --i) {
    fat[2*i] = i-1;
    fat[2*i+1] = 0;
  }
  fat[241*2] = 0xfa;
  fat[241*2+1] = 0xff;
  dir[0] = 0xcc;
  strncpy(dir+4, filename, 12);
  for(i=strlen(filename); i<12; i++)
    dir[4+i]=' ';
  dir[0x10] = tobcd(tm->tm_year/100+19);
  dir[0x11] = tobcd(tm->tm_year%100);
  dir[0x12] = tobcd(tm->tm_mon+1);
  dir[0x13] = tobcd(tm->tm_mday);
  dir[0x14] = tobcd(tm->tm_hour);
  dir[0x15] = tobcd(tm->tm_min);
  dir[0x16] = tobcd(tm->tm_sec);
  dir[0x17] = tobcd(tm->tm_wday);
  dir[0x18] = sz&0xff;
  dir[0x19] = sz>>8;
  dir[0x1a] = 1;
  memset(root, 0x55, 16);
  root[0x10] = 1;
  memcpy(root+0x30, dir+0x10, 8);
  root[0x44] = 255;
  root[0x46] = 254;
  root[0x48] = 1;
  root[0x4a] = 253;
  root[0x4c] = 13;
  root[0x50] = 200;
}

void gycheck_gamesize(VMUDevice* dev)
{
  unsigned char *root, *fat, *dir;
  int i, fatblk, dirblk, dircnt;
  root = dev->flash+255*512;
  fatblk = (root[0x47]<<8)|root[0x46];
  dirblk = (root[0x4b]<<8)|root[0x4a];
  dircnt = (root[0x4d]<<8)|root[0x4c];
  if(fatblk>=256 || dircnt>=256 || !dircnt)
    return;
  fat = dev->flash+fatblk*512;
  while(dircnt-- && dirblk<256) {
    dir = dev->flash+dirblk*512;
    dirblk = (fat[2*dirblk+1]<<8)|fat[2*dirblk];
    for(i=0; i<16; i++)
      if(dir[i*32] == 0xcc) {
    int sz = (dir[i*32+0x19]<<8)|dir[i*32+0x18];
    if(sz>1 && sz<=200) {
      dev->gameSize = sz*512;
      return;
    }
      }
  }
}


int gyinitflash(VMUDevice* dev,int fd)
{
  int r=0, t=0;
  memset(dev->flash, 0, sizeof(dev->flash));
  while(t<sizeof(dev->flash) && (r=read(fd, dev->flash+t, sizeof(dev->flash)-t))>0)
    t += r;
  if(r<0 || t<0x480)
    return 0;
  return t;
}


int gyloadflash(VMUDevice* dev, const char *filename)
{
    _gyLog(GY_DEBUG_VERBOSE, "Loading VMS image into flash [%s].", filename);
    _gyPush();

  int r, fd;

  if((fd = open(filename, O_RDONLY|O_BINARY))>=0) {
    if(!(r=gyinitflash(dev, fd))) {
      close(fd);
        _gyLog(GY_DEBUG_ERROR, "Could not load ROM!");
        _gyPop(1);
      return 0;
    }
    close(fd);
    dev->gameSize = r;

    _gyLog(GY_DEBUG_VERBOSE, "Raw image size - %d", r);
    gyVmuPrintVMSFileInfo((VMSFileInfo*)&dev->flash[0x200]);

    if(r<241*512)
      gyfakeflash(dev, filename, r);
    gycheck_gamesize(dev);

#if 0
    //assert(sizeof(dev->flash) == FLASH_SIZE);

    for(unsigned i = 0; i < FLASH_SIZE; ++i) {
      if(dev->flash[i]) {
          _gyLog(GY_DEBUG_VERBOSE, "FLASH VALID [%d]", i);
      }

    }
#endif
    _gyPop(1);
    return 1;
  } else {
    _gyLog(GY_DEBUG_ERROR, "Could not open file handle!");
    _gyPop(1);
    return 0;
  }


}



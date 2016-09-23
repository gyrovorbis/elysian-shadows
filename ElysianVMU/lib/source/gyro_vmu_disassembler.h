#ifndef GYRO_VMU_DISASSEMBLER_H
#define GYRO_VMU_DISASSEMBLER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <string.h>

struct VMUDevice;

bool gyVmuDisassembleFlashGame(const struct VMUDevice* dev, const char* outputFile, bool showAddr);
bool gyVmuDisassemble(const char* buffer, size_t size, const char* outputFile, bool showAddr);
bool gyVmuDisassembleFile(const char* inFile, const char* outFile, bool showAddr);

#ifdef __cplusplus
}
#endif

#endif // GYRO_VMU_DISASSEMBLER_H


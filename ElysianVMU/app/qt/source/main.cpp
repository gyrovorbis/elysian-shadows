#include <QApplication>
#include <string>
#include <QCommandLineParser>
#include <QDebug>
#include "gyro_vmu_main_window.hpp"
#include "gyro_vmu_instr.h"
#include "gyro_vmu_version.h"
#include "gyro_vmu_disassembler.h"
#include <vulkan/gyro_video_vulkan.h>
#include <QESTkUtils/tk_debug_log_stack.h>
#include <libGyro/gyro_defines.h>

static struct VMUQESTkUtilsDebugAdapter:
        public QESTkUtils::Debug::LogAdapter
{
    virtual void push(void) override { _gyPush(); }
    virtual void pop(unsigned count) override { _gyPop(count); }
    virtual void log(int severity, const char* fmt, ...) override {
        char buffer[GYRO_PRINTF_BUFFER_SIZE];
        va_list argptr;
        va_start(argptr, fmt);
        vsnprintf(buffer, sizeof(buffer), fmt, argptr);
        va_end(argptr);
        _gyLog((GY_DEBUG_SEVERITY)severity, "%s", buffer);
    }
} _vmuQESTkUtilsLogAdapter;

int main(int argc, char* argv[]) {

    gyDebugInit();
    QESTkUtils::Debug::setLogAdapter(&_vmuQESTkUtilsLogAdapter);

    QString romPath;
    QString workingDir;

    QApplication app(argc, argv);
    QApplication::setApplicationDisplayName("ElysianVMU");
    QApplication::setApplicationName("ElysianVMU");
    QApplication::setApplicationVersion(GYRO_VMU_VERSION_STRING);
    app.setWindowIcon(QIcon(":/icons/images/vmu_icon.ico"));

    QCommandLineParser parser;
    parser.setApplicationDescription("Software Emulator for the Sega Dreamcast Visual Memory Unit Device.");
    auto help = parser.addHelpOption();
    auto version = parser.addVersionOption();
    parser.addOption({{"r", "rom"}, "Load and run <Rom Image>", "Rom Image"});
    parser.addOption({{"w", "working-dir"}, "Set working the <Working Directory>", "Working Directory"});
    parser.addOption({{"i", "instruction"}, "Decode first byte of <Instruction>", "Instruction"});
    parser.addOption({{"d", "disassemble"}, "Disassemble <Rom Image> and output assembly <Source File>", "Source File"});
    parser.addOption({{"l", "line-numbers"}, "Include <Line Numbers> in disassembly file", "Line Numbers"});
    //add options for instruction set
    //add options for decoding instructions
    //add options for encoding instructions
    //add option for credits
    //load state option

    parser.parse(QApplication::arguments());

    if(parser.isSet(help)) {
        parser.showHelp();
        return 1;
    }

    if(parser.isSet(version)) {
        parser.showVersion();
        return 1;
    }

    if(parser.isSet("rom")) {
        romPath = parser.value("rom");
    }

    if(parser.isSet("w")) {
        workingDir = parser.value("w");
    }

    if(parser.isSet("d")) {
        if(!parser.isSet("r")) {
            qCritical() << "-d flag provided without -r flag for Rom Image file!";
            return 1;
        }

        QString outFile = parser.value("d");
        bool lineNumbers = parser.isSet("l");
        gyVmuDisassembleFile(romPath.toStdString().c_str(), outFile.toStdString().c_str(), lineNumbers);
        return 0;
    }

    if(parser.isSet("i")) {
        QString instr = parser.value("i");
        int opcode = instr.toInt();
        auto* entry = &_instrMap[opcode];
        gyPrintf("Opcode [%x]\n", entry->opcode);
        gyPrintf("\t%-20s: %40s\n", "mnemonic", entry->mnemonic);
        gyPrintf("\t%-20s: %40s\n", "description", entry->desc);
        gyPrintf("\t%-20s: %40d\n", "opcodeBits", entry->opBits);
        gyPrintf("\t%-20s: %40d\n", "argTypes", entry->args);
        gyPrintf("\t%-20s: %40d\n", "instructionBytes", entry->bytes);
        gyPrintf("\t%-20s: %40d\n", "clockCycles", entry->cc);
        gyPrintf("\t%-20s: %40d\n", "statusBits", entry->psw);
        return 1;
    }

    VMUMainWindow win(nullptr, workingDir);

    GYVkVidInitParams params = {
        NULL,
        NULL,
        0
    };
#ifdef LIBGYRO_VULKAN
    vulkanInit(&params); //TESTZING!!!!
#endif
    win.show();
    if(!romPath.isEmpty() && !win.loadRom(romPath)) return 1;
    return app.exec();
}


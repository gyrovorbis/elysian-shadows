#include <QApplication>
#include <string>
#include <QCommandLineParser>
#include "gyro_vmu_main_window.hpp"
#include "../lib/source/gyro_vmu_instr.h"

int main(int argc, char* argv[]) {

    gyDebugInit();

    QString romPath;
    QString workingDir;

    QApplication app(argc, argv);
    QApplication::setApplicationDisplayName("ElysianVMU");
    QApplication::setApplicationName("ElysianVMU");
    QApplication::setApplicationVersion("0.0.1 Alpha");
    app.setWindowIcon(QIcon(":/icons/images/vmu_icon.ico"));

    QCommandLineParser parser;
    parser.setApplicationDescription("Software Emulator for the Sega Dreamcast Visual Memory Unit Device.");
    auto help = parser.addHelpOption();
    auto version = parser.addVersionOption();
    parser.addOption({{"r", "rom"}, "Load and run <Rom Image>", "Rom Image"});
    parser.addOption({{"w", "working-dir"}, "Set working the <Working Directory>", "Working Directory"});
    parser.addOption({{"i", "instruction"}, "Decode first byte of <Instruction>", "Instruction"});
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
    win.show();
    if(!romPath.isEmpty() && !win.loadRom(romPath)) return 1;
    return app.exec();
}


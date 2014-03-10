TEMPLATE = app
CONFIG += console
CONFIG -= qt

win32:LIBS += -lws2_32 \
    -lIphlpapi

INCLUDEPATH += ./

SOURCES += version.c \
    verdef_64_tom.c \
    verdef_64_tof.c \
    verdef_32_tom.c \
    verdef_32_tof.c \
    update.c \
    unlink.c \
    syscalls.c \
    swap64.c \
    strptr.c \
    rawfile.c \
    rawdata.c \
    rand.c \
    opt.delscn.c \
    nlist.c \
    nextscn.c \
    next.c \
    newscn.c \
    newdata.c \
    ndxscn.c \
    memset.c \
    kind.c \
    input.c \
    hash.c \
    getscn.c \
    getident.c \
    getdata.c \
    getbase.c \
    getarsym.c \
    getaroff.c \
    getarhdr.c \
    gelftrans.c \
    gelfshdr.c \
    gelfphdr.c \
    gelfehdr.c \
    flag.c \
    fill.c \
    errno.c \
    errmsg.c \
    end.c \
    dc-tool.c \
    data.c \
    cook.c \
    cntl.c \
    checksum.c \
    begin.c \
    assert.c \
    64.xlatetof.c \
    32.xlatetof.c \
    32.newphdr.c \
    32.newehdr.c \
    32.getshdr.c \
    32.getphdr.c \
    32.getehdr.c \
    32.fsize.c \
    x.remscn.c \
    x.movscn.c \
    x.elfext.c

HEADERS += \
    verneed.h \
    verdef.h \
    syscalls.h \
    private.h \
    nlist.h \
    libelf.h \
    gelf.h \
    ext_types.h \
    errors.h \
    elf_repl.h \
    dcload-types.h \
    dc-io.h \
    commands.h \
    byteswap.h


default: no_platform

no_platform:
	@echo "You need to supply a platform"
	@echo -e "They are:\n\tlinux\n\tpsp\n\tdc\n\tios"
	@echo -e "there's also ios_bundle to bundle everyting\ninto a .app folder"

linux:
	@echo Building libGyro for Linux
	@cd libGyro && make -f Makefile.linux all
	@echo Building ESGamma for Linux
	@make -f Makefile.linux all

dc:
	@echo Building libGyro for Dreamcast
	@cd ../libGyro && make -f Makefile.dc all
	@echo Building ESGamma for Dreamcast
	@make -f Makefile.dc all
psp:
	@echo Building libGyro for Dreamcast
	@cd libGyro && make -f Makefile.psp all
	@echo Building ESGamma for PSP
	@make -f Makefile.psp all
ios:
	@echo Building libGyro for iOS
	@cd libGyro && make -f Makefile.ios all
	@echo Building ESGamma for iOS
	make -f Makefile.ios all
ios_bundle:
	make -f Makefile.ios bundle

clean_linux:
	@cd libGyro && make -f Makefile.linux clean
	@make -f Makefile.linux clean
clean_dc:
	@cd libGyro && make -f Makefile.dc clean
	@make -f Makefile.dc clean
clean_psp:
	@cd libGyro && make -f Makefile.psp clean
	@make -f Makefile.psp clean
clean_ios:
	@cd libGyro && make -f Makefile.ios clean
	@make -f Makefile.ios clean

if(APPLE)
	add_executable(basic MACOSX_BUNDLE main.c)
	set_target_properties(basic PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Basic")
else()
	add_executable(basic WIN32 main.c)
endif()

if(MSVC)
	set_target_properties(basic PROPERTIES
		LINK_FLAGS "/ENTRY:mainCRTStartup"
	)
endif()

if(APPLE)
	set_target_properties(basic PROPERTIES
		MACOSX_BUNDLE_SHORT_VERSION_STRING ${gy_VERSION_FULL}
		MACOSX_BUNDLE_LONG_VERSION_STRING ${gy_VERSION_FULL}
	)
endif()

target_link_libraries(basic ${Gyro_LIBRARIES})

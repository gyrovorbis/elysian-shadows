add_library(ESGamma SHARED
    ${ESG_SOURCES}
    ${AssetIO_SOURCES}
    ${ESG_INCLUDES}
    ${AssetIO_INCLUDES}
)
target_link_libraries(ESGamma -llog -landroid -lm -lEGL -lGLESv2 -lOpenSLES)
target_link_libraries(ESGamma ${ESGamma_LIBRARIES})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(gradleTask assembleRelease)
else()
    set(gradleTask assembleDebug)
endif()

set(build_SRC ${CMAKE_CURRENT_BINARY_DIR}/android_src)

set(GRADLE_ARGS
    -PbuildDir=${build_SRC}
    -b ${build_SRC}/build.gradle
)

make_directory(${build_SRC})
add_custom_command(TARGET ESGamma
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${build_SRC}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${build_SRC}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${build_SRC}/src/main/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${ES_ASSETS_DIR} ${build_SRC}/src/main/assets
    COMMAND ${CMAKE_COMMAND} -E make_directory ${build_SRC}/src/main/jniLibs/armeabi-v7a
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:ESGamma> ${build_SRC}/src/main/jniLibs/armeabi-v7a/$<TARGET_FILE_NAME:ESGamma>
    COMMAND ./gradlew ${GRADLE_ARGS} ${gradleTask}
    WORKING_DIRECTORY ${build_SRC}
    COMMENT "Executing Gradle to build APK..."
)

install(
    TARGETS ESGamma
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(
    DIRECTORY ${build_SRC}/apk
    DESTINATION bin
)

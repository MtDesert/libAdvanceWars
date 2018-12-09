cmake_minimum_required(VERSION 2.8)

foreach(name Structs Scenes)
	aux_source_directory(${name} SRC_LIST)
	include_directories(${name})
endforeach()

include(../libGamesEngines/CMakeLists.txt)

if(${TARGET_SYSTEM_NAME} STREQUAL Linux)
#add_executable(AdvanceWarsGLUT ../libGamesEngines/executable/GamesGLUT.cpp)
#add_executable(AdvanceWarsGLFW ../libGamesEngines/executable/GamesGLFW.cpp)
#add_executable(ToFreeWars executable/ToFreeWars.cpp executable/AwbwCategory.cpp)
#add_executable(lua2csv executable/lua2csv.cpp)
#add_executable(csv2lua executable/csv2lua.cpp)
#add_executable(DownloadCategory executable/DownloadCategory.cpp executable/AwbwCategory.cpp)
#add_executable(DownloadMap executable/DownloadMap.cpp executable/AwbwCategory.cpp executable/AwbwMap.cpp)
#add_executable(main executable/main.cpp)

#target_link_libraries(AdvanceWarsGLUT AdvanceWars GamesEngines Network Renderer Font Charset Cpp11 tolua lua glut GL curl)
#target_link_libraries(AdvanceWarsGLFW AdvanceWars GamesEngines Network Renderer Font Charset Cpp11 tolua lua glfw GL curl)
#target_link_libraries(ToFreeWars AdvanceWars Network curl tolua lua GL)
#target_link_libraries(lua2csv AdvanceWars tolua lua GL)
#target_link_libraries(csv2lua AdvanceWars tolua lua GL)
#target_link_libraries(DownloadCategory Network curl)
#target_link_libraries(DownloadMap Network curl)
#target_link_libraries(main AdvanceWars tolua lua GL)

#elseif(${TARGET_SYSTEM_NAME} STREQUAL Android)
#add_library(AdvanceWarsAndroid SHARED
#../libGamesEngines/executable/GamesNativeActivity.cpp
#${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
endif()

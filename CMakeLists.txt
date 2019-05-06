cmake_minimum_required(VERSION 2.8)
include(../CompileScripts/CMakeLists.txt)

set(INCLUDE_LIST
	. #当前目录
)

if(INCLUDE_DIR_ONLY)#仅包含目录
	includeDirAll(../libAdvanceWars/)#仅包含目录
else()
	srcIncludeAll()#包含目录和源文件
	set(COMPILE_GAME_LIB ON)
endif()
include(../libGamesEngines/CMakeLists.txt)#使用引擎的编译规则

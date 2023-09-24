cmake_minimum_required(VERSION 3.1)

set(PROJECT NoodlesPlate)

enable_language(C CXX)

project(${PROJECT} CXX)

# need openssl : https://slproweb.com/products/Win32OpenSSL.html

option(USE_SDL "Enale SDL instead of GLFW" OFF)
option(USE_PROFILER_TRACY "Enale Tracy Profiler" OFF)
option(USE_GPU_METRIC "Enable Gpu Metrics if available (NVIDIA and WINDOWS only)" OFF)
option(USE_NETWORK "Enable NETWORK (Shader Inport, Versions, Changelog)" OFF)
option(USE_STD_FILESYSTEM "Enable std::filesystem use for path and ImGuiFileDialog" ON)
option(USE_PACKAGE_TO_NSIS "Enable packaging to nsis isntaller or zip if false" ON)
option(USE_VR "Enable VR Backend via Openxr" OFF)

if (USE_NETWORK)
	add_definitions(-DUSE_NETWORK)
endif()

if (USE_SDL)
	add_definitions(-DUSE_SDL2)
else()
	add_definitions(-DUSE_GLFW3)
endif()

if (USE_VR)
	add_definitions(-DUSE_VR)
	add_definitions(-DXR_USE_GRAPHICS_API_OPENGL)
	if(WIN32)
		add_definitions(-DXR_USE_PLATFORM_WIN32)
	endif()
endif()

## some defines for debug mode (before 3rdparty.cmake)
if(MSVC)
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG /Zi /MTd")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} /MT")
else()
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -g")
endif()

set(LLVM_USE_CRT_DEBUG MTd CACHE STRING "" FORCE)
set(LLVM_USE_CRT_MINSIZEREL MT CACHE STRING "" FORCE)
set(LLVM_USE_CRT_RELEASE MT CACHE STRING "" FORCE)
set(LLVM_USE_CRT_RELWITHDEBINFO MT CACHE STRING "" FORCE)
set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "" FORCE)

## for group smake targets in the dir cmakeTargets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmakeTargets")

if (USE_STD_FILESYSTEM)
	add_definitions(-DUSE_STD_FILESYSTEM)
	set(CMAKE_CXX_STANDARD 17)
else()
	set(CMAKE_CXX_STANDARD 14)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DGLAD)
add_definitions(-DGLM)
add_definitions(-DUSE_OPENGL)
add_definitions(-DUSE_XML_CONFIG)
add_definitions(-DIMGUI_ENABLE_FREETYPE)
add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS)
add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)
add_definitions(-DCUSTOM_SOGLSL_CONFIG="${CMAKE_SOURCE_DIR}/src/Headers/CustomSoGLSLConfig.h")
add_definitions(-DCUSTOM_IMWIDGETS_CONFIG="${CMAKE_SOURCE_DIR}/src/Headers/CustomImWidgetsConfig.h")
#add_definitions(-DCUSTOM_IN_APP_GPU_PROFILER_CONFIG="${CMAKE_SOURCE_DIR}/src/Headers/CustomInAppGpuProfiler.h")
add_definitions(-DCUSTOM_IMGUIFILEDIALOG_CONFIG="${CMAKE_SOURCE_DIR}/src/Headers/CustomImGuiFileDialogConfig.h")
add_definitions(-DUSE_IMGUI="${CMAKE_SOURCE_DIR}/3rdparty/SoGLSL/3rdparty/ImGuiPack/3rdparty/imgui/imgui.h")

include(cmake/3rdparty.cmake)

if (USE_PROFILER_TRACY)
	option(USE_CALIBRATED_CONTEXT "Enable Calibrated Context in Tracy for sync cpu/gpu timestamps" OFF)
if (USE_CALIBRATED_CONTEXT)
	add_definitions(-DENABLE_CALIBRATED_CONTEXT)
endif()
	add_definitions(-DTRACY_ENABLE)
	add_definitions(-DTRACY_ON_DEMAND)
	add_definitions(-DTRACY_NO_BROADCAST)
	add_definitions(-DTRACY_ONLY_LOCALHOST=1)
endif()

if(UNIX)
    if(APPLE)
        add_definitions(-DAPPLE)
    else()
        add_definitions(-DLINUX)
    endif()
    add_definitions(-DUNIX)
	add_definitions(-DMINGW32)	
elseif(WIN32)
		add_definitions(-DWIN32)
	if(MINGW)
		add_definitions(-DMINGW32)
	endif()
    set(DIRENT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/dirent/include)
    include_directories(${DIRENT_INCLUDE_DIR})
endif()

if(MSVC)
	# Ignore 4055 for glad
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /IGNORE:4055")
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-DMSVC)
	add_definitions(-DNOMINMAX) ## for avoid issue regarding std::max and std::min
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -Wall -Wextra -Wpedantic -Wdeprecated-declarations -Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-parameter")
endif()

if (TARGET uninstall)
set_target_properties(uninstall PROPERTIES FOLDER "cmakeTargets")
endif()

add_definitions(${GLFW_DEFINITIONS})

file(GLOB PROJECT_MAIN ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/.clang-format)
source_group(main FILES ${PROJECT_MAIN})
file(GLOB_RECURSE PROJECT_SRC_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/src/*.*)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX src FILES ${PROJECT_SRC_RECURSE})

if (USE_SDL)
file(GLOB IMGUI_IMPL_SRC 
	${CMAKE_SOURCE_DIR}/3rdparty/SoGLSL/3rdparty/ImGuiPack/3rdparty/imgui/backends/imgui_impl_opengl3.cpp
	${CMAKE_SOURCE_DIR}/3rdparty/SoGLSL/3rdparty/ImGuiPack/3rdparty/imgui/backends/imgui_impl_opengl3.h
	${CMAKE_SOURCE_DIR}/3rdparty/SoGLSL/3rdparty/ImGuiPack/3rdparty/imgui/backends/imgui_impl_sdl.cpp
	${CMAKE_SOURCE_DIR}/3rdparty/SoGLSL/3rdparty/ImGuiPack/3rdparty/imgui/backends/imgui_impl_sdl.h)
else()
file(GLOB IMGUI_IMPL_SRC 
	${CMAKE_SOURCE_DIR}/3rdparty/SoGLSL/3rdparty/ImGuiPack/3rdparty/imgui/backends/imgui_impl_opengl3.cpp
	${CMAKE_SOURCE_DIR}/3rdparty/SoGLSL/3rdparty/ImGuiPack/3rdparty/imgui/backends/imgui_impl_opengl3.h
	${CMAKE_SOURCE_DIR}/3rdparty/SoGLSL/3rdparty/ImGuiPack/3rdparty/imgui/backends/imgui_impl_glfw.cpp
	${CMAKE_SOURCE_DIR}/3rdparty/SoGLSL/3rdparty/ImGuiPack/3rdparty/imgui/backends/imgui_impl_glfw.h)
endif()
source_group(src/Impl FILES ${IMGUI_IMPL_SRC})

file(GLOB_RECURSE CMAKE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/*.cmake)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/cmake PREFIX cmake FILES ${CMAKE_RECURSE})

if (MSVC)
	set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_SOURCE_DIR}/res/appicon.rc")
endif()

add_executable(
	${PROJECT}
	${PROJECT_MAIN}
	${CMAKE_RECURSE}
	${IMGUI_IMPL_SRC}
	${IMGUI_MISC_SRC}
	${PROJECT_SRC_RECURSE}
	${APP_ICON_RESOURCE_WINDOWS})

## will put dependencies in output dir
##add_custom_command(TARGET ${PROJECT} POST_BUILD
##  COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT}> $<TARGET_FILE_DIR:${PROJECT}>
##  COMMAND_EXPAND_LISTS
##)

if ("${CMAKE_EXE_LINKER_FLAGS}" STREQUAL "/machine:x64")
	set(ARCH x64)
	add_definitions(-DVERSION_X64)
	message("${PROJECT} Build X64")
else()
	set(ARCH x32)
	add_definitions(-DVERSION_X32)
	message("${PROJECT} Build X32")
endif()

#############################################################
### VERSION #################################################
#############################################################

file(READ ${CMAKE_SOURCE_DIR}/src/Headers/NoodlesPlateBuild.h BuildIdFile)
string(REGEX MATCH "#define NoodlesPlate_BuildNumber ([0-9]*)" _ ${BuildIdFile})
set(BuildNumber ${CMAKE_MATCH_1})
string(REGEX MATCH "#define NoodlesPlate_MinorNumber ([0-9]*)" _ ${BuildIdFile})
set(MinorNumber ${CMAKE_MATCH_1})
string(REGEX MATCH "#define NoodlesPlate_MajorNumber ([0-9]*)" _ ${BuildIdFile})
set(MajorNumber ${CMAKE_MATCH_1})
set(${PROJECT}_Version "v${MajorNumber}.${MinorNumber}.${BuildNumber}")
message("${PROJECT} Version to install : ${${PROJECT}_Version}")

#############################################################
#############################################################
#############################################################

if (APPLE)
	set(${PROJECT}_Output_Name "${PROJECT}_Osx_${ARCH}")
	set_target_properties(${PROJECT} PROPERTIES
		OUTPUT_NAME "${PROJECT}_Osx_${ARCH}"
		MACOSX_BUNDLE_EXECUTABLE_NAME "${PROJECT}_Osx_${ARCH}"
		MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT}_Osx_${ARCH}"
		FOLDER "${PROJECT}_Osx_${ARCH}"
		##RESOURCE res/icon.icns
		##MACOSX_BUNDLE_SHORT_VERSION_STRING "0.5"
		##MACOSX_BUNDLE_LONG_VERSION_STRING "Beta 0.5"
		##MACOSX_BUNDLE_ICON_FILE res/icon.icns
		MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/res/MacOSXBundleInfo.plist.in")
elseif(WIN32)
	if (MINGW)
		set(${PROJECT}_Output_Name "${PROJECT}_Mingw32_${ARCH}")
	elseif (CYGWIN)
		set(${PROJECT}_Output_Name "${PROJECT}_Cygwin_${ARCH}")
	elseif(MSVC)
		set(${PROJECT}_Output_Name "${PROJECT}_Msvc_${ARCH}")
	endif()
	set_target_properties(${PROJECT} PROPERTIES	OUTPUT_NAME "${${PROJECT}_Output_Name}")
elseif(UNIX)
	set(${PROJECT}_Output_Name "${PROJECT}_Linux_${ARCH}")
	set_target_properties(${PROJECT} PROPERTIES	OUTPUT_NAME "${${PROJECT}_Output_Name}")
endif()

#message("SOGSL_INCLUDE_DIRS :")
#foreach( LINE ${SOGSL_INCLUDE_DIRS})
#	message(" - ${LINE}")
#endforeach()

include_directories(
	src
	include
	3rdparty
	3rdparty/SoGLSL
	${NVAPI_INCLUDE_DIR}
	${SOGSL_INCLUDE_DIRS}
	${FREETYPE_INCLUDE_DIR}
)

target_link_libraries(${PROJECT} PRIVATE 
	${ZLIB_LIBRARIES}
	${NVAPI_LIBRARIES}
	${SOGLSL_LIBRARIES}
	${FREETYPE_LIBRARIES}
)

#############################################################
### INSTALL #################################################
#############################################################

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/bin")
install(TARGETS ${PROJECT} 
	RUNTIME 
	DESTINATION /
	COMPONENT APP_BINARY)
install(FILES 
	${CMAKE_SOURCE_DIR}/LICENSE
	${CMAKE_SOURCE_DIR}/VERSION
    DESTINATION /
	COMPONENT APP_FILES)
##install(TARGETS ${ZLIB_LIBRARIES} RUNTIME DESTINATION / COMPONENT APP_DLL)

#############################################################
### PACK ####################################################
## https://gitlab.kitware.com/cmake/community/-/wikis/doc/cpack/Component-Install-With-CPack
#############################################################

if (USE_PACKAGE_TO_NSIS)
	set(CPACK_GENERATOR "NSIS")
else()
	set(CPACK_GENERATOR "ZIP")
endif()
set(CPACK_MONOLITHIC_INSTALL 1)
set(CPACK_ZIP_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
set(CPACK_PACKAGE_FILE_NAME "${${PROJECT}_Output_Name}_v${MajorNumber}.${MinorNumber}.${BuildNumber}")
set(CPACK_PACKAGE_VENDOR "FunParadigm")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${${PROJECT}_Output_Name} - offline shader editor")
set(CPACK_PACKAGE_VERSION "${MajorNumber}.${MinorNumber}.${BuildNumber}")
set(CPACK_PACKAGE_VERSION_MAJOR "${MajorNumber}")
set(CPACK_PACKAGE_VERSION_MINOR "${MinorNumber}")
set(CPACK_PACKAGE_VERSION_PATCH "${BuildNumber}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "${CMAKE_PROJECT_NAME}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/VERSION")
SET(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_INSTALL_PREFIX}")
include(CPack)
	
#############################################################
################# BUILD INCREMENT TOOL ######################
#############################################################

## only for Debug
add_subdirectory(3rdparty/BuildInc)
set_target_properties(BuildInc PROPERTIES FOLDER 3rdparty/aiekick)

add_custom_command(
	TARGET ${PROJECT} PRE_BUILD
	COMMAND BuildInc -prefix=${PROJECT} 1000000:10 $<$<CONFIG:Debug>:${CMAKE_SOURCE_DIR}/src/Headers/${PROJECT}Build.h>
	DEPENDS ${PROJECT}
)

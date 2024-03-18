
set(SOGSL_INCLUDE_DIRS)
set(SOGLSL_LIBRARIES SoGLSL)

add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/SoGLSL)

if(USE_SHARED_LIBS)
	set_target_properties(SoGLSL PROPERTIES FOLDER 3rdparty/aiekick/Shared)
else()
	set_target_properties(SoGLSL PROPERTIES FOLDER 3rdparty/aiekick/Static)
endif()

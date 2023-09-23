set(SOGSL_INCLUDE_DIRS)
set(SOGLSL_LIBRARIES SoGLSL)
add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/SoGLSL)
set_target_properties(SoGLSL PROPERTIES FOLDER Libs/Static)


set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

##EXCLUDE_FROM_ALL reject install for this target
add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/zlib EXCLUDE_FROM_ALL)

set_target_properties(zlibstatic PROPERTIES FOLDER 3rdparty)

set(ZLIB_INCLUDE_DIRS 
	${CMAKE_SOURCE_DIR}/3rdparty/zlib 
	${CMAKE_CURRENT_BINARY_DIR}/3rdparty/zlib)
set(ZLIB_INCLUDE_DIR ${ZLIB_INCLUDE_DIRS})
set(ZLIB_LIBRARIES zlibstatic)
set(ZLIB::ZLIB zlibstatic)
set(ZLIB_FOUND TRUE)

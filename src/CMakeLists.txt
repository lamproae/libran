CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

project(libran)

file(GLOB source_files "*.c")
file(GLOB header_files "*.h")

add_library(${PROJECT_NAME} SHARED ${source_files})
add_library(${PROJECT_NAME}_static STATIC ${source_files})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
SET_TARGET_PROPERTIES(libran PROPERTIES OUTPUT_NAME ran)
SET_TARGET_PROPERTIES(libran_static PROPERTIES OUTPUT_NAME ran_static)
include_directories("${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/")

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${header_files}")
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME})

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
  CACHE INTERNAL "${PROJECT_NAME}: Include directories" FORCE)

install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_static
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  PUBLIC_HEADER DESTINATION include/${PROJECT_NAME}
  )

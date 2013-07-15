project(OPENGC_ENC_LIB CXX C)
include(${CMAKE_COMMON_INC})

set(LIB_TYPE "STATIC" CACHE STRING "STATIC, SHARED or MODULE?")
message("[opengc] \t LIB_TYPE " ${LIB_TYPE})
#set(CMAKE_CXX_FLAGS "-g -Wall")
add_library(opengc_enc_lib ${PROJECT_CPP_FILES} ${PROJECT_C_FILES} ${PROJECT_INC_FILES} ${PROJECT_INL_FILES})
include_directories("${${PROJECT_NAME}_SOURCE_DIR}/../opengc_common_lib/inc")


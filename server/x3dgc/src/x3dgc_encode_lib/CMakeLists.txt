project(X3DGC_ENC_LIB CXX C)
include(${CMAKE_COMMON_INC})

set(LIB_TYPE "STATIC" CACHE STRING "STATIC, SHARED or MODULE?")
message("[x3dgc] \t LIB_TYPE " ${LIB_TYPE})
#set(CMAKE_CXX_FLAGS "-g -Wall")
add_library(x3dgc_enc_lib ${PROJECT_CPP_FILES} ${PROJECT_C_FILES} ${PROJECT_INC_FILES} ${PROJECT_INL_FILES})
include_directories("${CMAKE_SOURCE_DIR}/x3dgc_common_lib/inc")


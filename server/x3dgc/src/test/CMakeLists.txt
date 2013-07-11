project(X3DGC_TEST)
include(${CMAKE_COMMON_INC})
add_executable(test_x3dgc ${PROJECT_CPP_FILES} ${PROJECT_C_FILES} ${PROJECT_INC_FILES} ${PROJECT_INL_FILES})
#set(CMAKE_CXX_FLAGS "-g -Wall")
target_link_libraries(test_x3dgc x3dgc_dec_lib x3dgc_enc_lib x3dgc_common_lib)
include_directories("${${PROJECT_NAME}_SOURCE_DIR}/../x3dgc_decode_lib/inc" "${${PROJECT_NAME}_SOURCE_DIR}/../x3dgc_encode_lib/inc" "${${PROJECT_NAME}_SOURCE_DIR}/../x3dgc_common_lib/inc")


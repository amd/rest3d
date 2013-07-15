project(OPENGC_TEST)
include(${CMAKE_COMMON_INC})
add_executable(test_opengc ${PROJECT_CPP_FILES} ${PROJECT_C_FILES} ${PROJECT_INC_FILES} ${PROJECT_INL_FILES})
#set(CMAKE_CXX_FLAGS "-g -Wall")
target_link_libraries(test_opengc opengc_dec_lib opengc_enc_lib opengc_common_lib)
include_directories("${${PROJECT_NAME}_SOURCE_DIR}/../opengc_decode_lib/inc" "${${PROJECT_NAME}_SOURCE_DIR}/../opengc_encode_lib/inc" "${${PROJECT_NAME}_SOURCE_DIR}/../opengc_common_lib/inc")


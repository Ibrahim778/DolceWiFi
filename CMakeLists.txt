cmake_minimum_required(VERSION 3.12)
set(CMAKE_TOOLCHAIN_FILE $ENV{SCE_PSP2_SDK_DIR}/host_tools/build/cmake/psp2-snc-toolchain.cmake)

project(DolceWiFi)
include(VitaDevelopmentSuite)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
enable_language(EMD)

add_library(${PROJECT_NAME} MODULE
    src/main.cpp

    ${PROJECT_NAME}.emd
)

target_compile_options(${PROJECT_NAME} PRIVATE
    -D_SCE_TARGET_OS_PSP2=1 -Xdiag=0 -Xquit=2 -D__SNC__=1
)

target_link_options(${PROJECT_NAME} PRIVATE
  --prx-no-runtime-support
)

target_link_directories(${PROJECT_NAME} PRIVATE
  ${VDSUITE_LIBRARY_DIRECTORIES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${VDSUITE_USER_INCLUDE_DIRECTORIES}
  ${VDSUITE_STANDARD_INCLUDE_DIRECTORIES}
  $ENV{SCE_PSP2_SDK_DIR}/target/include
  $ENV{SCE_PSP2_SDK_DIR}/target/include_common
)

target_link_libraries(${PROJECT_NAME}
    SceLibKernel_stub
    SceThreadmgr_stub
    SceNet_stub_weak
    SceNetCtl_stub_weak
    SceAppMgrUser_stub
    SceRegMgr_stub
	ScePafStdc_stub_weak
    ScePafThread_stub_weak
    ScePafTopLevel_stub_weak
    ScePafWidget_stub_weak
    ScePafCommon_stub_weak
    ScePafGraphics_stub_weak
    ScePafMisc_stub_weak
    ScePafResource_stub_weak
    QuickMenuReborn_stub_weak
    ScePower_stub
    SceWlan_stub
    SceAppMgr_stub
)

VDSuiteSignElf(${PROJECT_NAME}.suprx ${PROJECT_NAME})

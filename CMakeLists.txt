cmake_minimum_required(VERSION 3.16)

project(NetSIPCore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SIPCore
    src/main.cpp
    src/SIPCore.cpp
    src/sip/Account.cpp
    src/sip/Call.cpp
    src/sip/InfoModule.cpp
    src/tcp/TcpServer.cpp
    src/utils/Json.cpp
    src/utils/RTP.cpp
)

set_target_properties(SIPCore PROPERTIES
    OUTPUT_NAME "NetSIPCore"
)

target_include_directories(SIPCore PRIVATE
    src
    third_party/pjproject/pjsip/include
    third_party/pjproject/pjlib/include
    third_party/pjproject/pjlib-util/include
    third_party/pjproject/pjmedia/include
    third_party/pjproject/pjnath/include
)

target_link_directories(SIPCore PRIVATE
    ${CMAKE_SOURCE_DIR}/third_party/pjproject/pjsip/lib
    ${CMAKE_SOURCE_DIR}/third_party/pjproject/pjlib/lib
    ${CMAKE_SOURCE_DIR}/third_party/pjproject/pjlib-util/lib
    ${CMAKE_SOURCE_DIR}/third_party/pjproject/pjmedia/lib
    ${CMAKE_SOURCE_DIR}/third_party/pjproject/pjnath/lib
    ${CMAKE_SOURCE_DIR}/third_party/pjproject/lib
)

if(WIN32)
    target_sources(SIPCore PRIVATE resources/NetSIPCore.rc)

    target_link_libraries(SIPCore PRIVATE
        libpjproject-x86_64-x64-vc14-Release

        ws2_32
        winmm
        ole32
        oleaut32
        uuid
        secur32
        crypt32
        iphlpapi
    )

else()

    set(PJPROJECT_DIR ${CMAKE_SOURCE_DIR}/third_party/pjproject)

    target_link_libraries(SIPCore PRIVATE

        # pjsip
        ${PJPROJECT_DIR}/pjsip/lib/libpjsua2-x86_64-pc-linux-gnu.a
        ${PJPROJECT_DIR}/pjsip/lib/libpjsua-x86_64-pc-linux-gnu.a
        ${PJPROJECT_DIR}/pjsip/lib/libpjsip-ua-x86_64-pc-linux-gnu.a
        ${PJPROJECT_DIR}/pjsip/lib/libpjsip-simple-x86_64-pc-linux-gnu.a
        ${PJPROJECT_DIR}/pjsip/lib/libpjsip-x86_64-pc-linux-gnu.a

        # pjmedia
        ${PJPROJECT_DIR}/pjmedia/lib/libpjmedia-codec-x86_64-pc-linux-gnu.a
        ${PJPROJECT_DIR}/pjmedia/lib/libpjmedia-x86_64-pc-linux-gnu.a
        ${PJPROJECT_DIR}/pjmedia/lib/libpjmedia-audiodev-x86_64-pc-linux-gnu.a
        ${PJPROJECT_DIR}/pjmedia/lib/libpjmedia-videodev-x86_64-pc-linux-gnu.a

        # pjnath
        ${PJPROJECT_DIR}/pjnath/lib/libpjnath-x86_64-pc-linux-gnu.a

        # pjlib
        ${PJPROJECT_DIR}/pjlib-util/lib/libpjlib-util-x86_64-pc-linux-gnu.a
        ${PJPROJECT_DIR}/pjlib/lib/libpj-x86_64-pc-linux-gnu.a

        # third party
        ${PJPROJECT_DIR}/third_party/lib/libsrtp-x86_64-pc-linux-gnu.a
        ${PJPROJECT_DIR}/third_party/lib/libresample-x86_64-pc-linux-gnu.a
        ${PJPROJECT_DIR}/third_party/lib/libgsmcodec-x86_64-pc-linux-gnu.a
        ${PJPROJECT_DIR}/third_party/lib/libilbccodec-x86_64-pc-linux-gnu.a
        ${PJPROJECT_DIR}/third_party/lib/libg7221codec-x86_64-pc-linux-gnu.a
        ${PJPROJECT_DIR}/third_party/lib/libspeex-x86_64-pc-linux-gnu.a
        ${PJPROJECT_DIR}/third_party/lib/libwebrtc-x86_64-pc-linux-gnu.a
        ${PJPROJECT_DIR}/third_party/lib/libyuv-x86_64-pc-linux-gnu.a

        # system
        asound
        uuid
        pthread
        dl
        m
    )

endif()
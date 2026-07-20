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
    src/tcp/TCPServer.cpp
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

    target_link_libraries(SIPCore PRIVATE
        pj

        pthread
        dl
        m
        rt
    )

endif()
# SPDX-FileCopyrightText: 2006-2026 Felix Hammer
# SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#
# Qupil for students: the practice book ("Übeheft") the teacher's Qupil writes.
# A project of its own, next to the teacher app; it shares only the booklet format
# (src/shared) and the encryption (src/core/Crypto, Monocypher) with it.
#
#   cmake -S student -B build-student && cmake --build build-student

cmake_minimum_required(VERSION 3.21)

project(qupil-student
    VERSION 1.0.0
    DESCRIPTION "The practice book of Qupil, for students"
    HOMEPAGE_URL "https://qupil.de"
    LANGUAGES C CXX)

if(IOS)
    # Objective-C++ for the QR code scanner (QrScanner_ios.mm)
    enable_language(OBJCXX)
    set(CMAKE_XCODE_ATTRIBUTE_SKIP_INSTALL YES)
endif()

set(QUPIL_BUILD_NUMBER "" CACHE STRING "Build number (CFBundleVersion, Android version code) for store uploads")
if(QUPIL_BUILD_NUMBER)
    set(QUPIL_BUNDLE_VERSION ${QUPIL_BUILD_NUMBER})
else()
    set(QUPIL_BUNDLE_VERSION ${PROJECT_VERSION})
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The stores build with Qt 6.8; 6.4 is enough to try it out on a Linux desktop
find_package(Qt6 6.4 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Network)
if(Qt6_VERSION VERSION_GREATER_EQUAL 6.5)
    qt_standard_project_setup(REQUIRES 6.5)
else()
    qt_standard_project_setup()
endif()

set(QUPIL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

# ---- booklet format and encryption (shared with the teacher app; the teacher's side
#      of it – inbox, links in Nextcloud – is built here too, for its tests)
qt_add_library(qupilbooklet STATIC
    ${QUPIL_ROOT}/src/shared/Booklet.h
    ${QUPIL_ROOT}/src/shared/Booklet.cpp
    ${QUPIL_ROOT}/src/shared/BookletInbox.h
    ${QUPIL_ROOT}/src/shared/BookletInbox.cpp
    ${QUPIL_ROOT}/src/shared/NextcloudShares.h
    ${QUPIL_ROOT}/src/shared/NextcloudShares.cpp
    ${QUPIL_ROOT}/src/shared/QrCode.h
    ${QUPIL_ROOT}/src/shared/QrCode.cpp
    ${QUPIL_ROOT}/3rdparty/qrcodegen/qrcodegen.hpp
    ${QUPIL_ROOT}/3rdparty/qrcodegen/qrcodegen.cpp
    ${QUPIL_ROOT}/src/core/Crypto.h
    ${QUPIL_ROOT}/src/core/Crypto.cpp
    ${QUPIL_ROOT}/3rdparty/monocypher/monocypher.c
    ${QUPIL_ROOT}/3rdparty/monocypher/monocypher.h)
target_include_directories(qupilbooklet
    PUBLIC ${QUPIL_ROOT}/src/shared ${QUPIL_ROOT}/src/core
    PRIVATE ${QUPIL_ROOT}/3rdparty/monocypher ${QUPIL_ROOT}/3rdparty/qrcodegen)
target_link_libraries(qupilbooklet PUBLIC Qt6::Core Qt6::Gui Qt6::Network)

# ---- interface and state (a library, so the tests drive the same QML as the app)
qt_add_library(qupilstudentui STATIC)
# Singletons: must be set before qt_add_qml_module()
set_source_files_properties(qml/Theme.qml qml/Fmt.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
qt_add_qml_module(qupilstudentui
    URI QupilStudent
    VERSION 1.0
    RESOURCE_PREFIX /qt/qml
    OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/QupilStudent
    SOURCES
        src/StudentApp.h
        src/StudentApp.cpp
        src/Translator.h
        src/Translator.cpp
        src/QrScanner.h
        src/QrScanner.cpp
        src/IncomingLinks.h
    QML_FILES
        qml/Main.qml
        qml/Theme.qml
        qml/Fmt.qml
        qml/Icon.qml
        qml/WelcomePage.qml
        qml/HomePage.qml
        qml/SettingsPage.qml
        qml/components/T.qml
        qml/components/SectionLabel.qml
        qml/components/Card.qml
        qml/components/Pill.qml
        qml/components/ChoiceRow.qml
        qml/components/TaskCard.qml
        qml/components/AppButton.qml
        qml/components/IconButton.qml
        qml/components/AddTeacher.qml
        qml/components/Logo.qml
        qml/components/PageFlickable.qml)
qt_add_resources(qupilstudentui "student_assets"
    PREFIX /
    BASE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES i18n/strings.json)
qt_add_resources(qupilstudentui "student_fonts"
    PREFIX /fonts
    BASE ${QUPIL_ROOT}/assets/fonts
    FILES
        ${QUPIL_ROOT}/assets/fonts/Figtree-Regular.ttf
        ${QUPIL_ROOT}/assets/fonts/Figtree-SemiBold.ttf
        ${QUPIL_ROOT}/assets/fonts/Figtree-Bold.ttf
        ${QUPIL_ROOT}/assets/fonts/Figtree-ExtraBold.ttf)
target_include_directories(qupilstudentui PUBLIC src)
target_compile_definitions(qupilstudentui PUBLIC QUPIL_STUDENT_VERSION="${PROJECT_VERSION}")
# Which build this is (shown beside the version in the settings, as in Qupil): the commit
# on CI, "dev" otherwise
if(DEFINED ENV{GITHUB_SHA})
    string(SUBSTRING "$ENV{GITHUB_SHA}" 0 7 QUPIL_STUDENT_BUILD)
else()
    set(QUPIL_STUDENT_BUILD "dev")
endif()
target_compile_definitions(qupilstudentui PRIVATE QUPIL_STUDENT_BUILD="${QUPIL_STUDENT_BUILD}")
target_link_libraries(qupilstudentui PUBLIC qupilbooklet
    Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickControls2 Qt6::Network)
# The QR code scanner: the phone's camera (iOS: AVFoundation; Android: StudentActivity
# with zxing-android-embedded, see platform/android/build.gradle); none on the desktop
if(IOS)
    target_sources(qupilstudentui PRIVATE src/QrScanner_ios.mm src/IncomingLinks_ios.mm src/KeyboardHeight_none.cpp
        src/SafeArea_ios.mm)
    set_source_files_properties(src/QrScanner_ios.mm src/IncomingLinks_ios.mm src/SafeArea_ios.mm
        PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
    target_link_libraries(qupilstudentui PUBLIC "-framework AVFoundation" "-framework UIKit")
elseif(ANDROID)
    # The links that open the app come through StudentActivity as well (IncomingLinks.h)
    # ... and the height of the on-screen keyboard (KeyboardHeight.h)
    target_sources(qupilstudentui PRIVATE src/QrScanner_android.cpp src/IncomingLinks_android.cpp
        src/KeyboardHeight_android.cpp src/SafeArea_none.cpp)
else()
    target_sources(qupilstudentui PRIVATE src/QrScanner_none.cpp src/IncomingLinks_qt.cpp src/KeyboardHeight_none.cpp
        src/SafeArea_none.cpp)
endif()

# ---- the app
qt_add_executable(qupil-student src/main.cpp)
target_link_libraries(qupil-student PRIVATE qupilstudentui qupilstudentuiplugin)

set_target_properties(qupil-student PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "Qupil Practice"
    MACOSX_BUNDLE_GUI_IDENTIFIER de.qupil.student
    MACOSX_BUNDLE_BUNDLE_VERSION ${QUPIL_BUNDLE_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    MACOSX_BUNDLE_COPYRIGHT "© 2006–2026 Felix Hammer"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER de.qupil.student)

if(IOS)
    set_target_properties(qupil-student PROPERTIES
        OUTPUT_NAME QupilStudent
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
        XCODE_ATTRIBUTE_SKIP_INSTALL NO
        XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)")
    set(_assets ${CMAKE_CURRENT_SOURCE_DIR}/platform/ios/Assets.xcassets)
    if(EXISTS ${_assets})
        target_sources(qupil-student PRIVATE ${_assets})
        set_source_files_properties(${_assets} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        set_target_properties(qupil-student PROPERTIES XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon)
    endif()
    foreach(_lang en de fr es)
        set(_strings ${CMAKE_CURRENT_SOURCE_DIR}/platform/ios/${_lang}.lproj/InfoPlist.strings)
        target_sources(qupil-student PRIVATE ${_strings})
        set_source_files_properties(${_strings} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    endforeach()
    set(_privacy ${QUPIL_ROOT}/platform/ios/PrivacyInfo.xcprivacy)
    target_sources(qupil-student PRIVATE ${_privacy})
    set_source_files_properties(${_privacy} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    # Universal links (https://qupil.de/u#…): the Associated Domains entitlement. The CI builds
    # the archive unsigned and puts the entitlement in before the export (student.yml); this
    # option is for a build signed by Xcode itself.
    option(QUPIL_STUDENT_ASSOCIATED_DOMAINS "iOS: open https://qupil.de/u links in the app" OFF)
    if(QUPIL_STUDENT_ASSOCIATED_DOMAINS)
        set_target_properties(qupil-student PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS
            ${CMAKE_CURRENT_SOURCE_DIR}/platform/ios/QupilStudent.entitlements)
    endif()
    add_custom_command(TARGET qupil-student POST_BUILD
        COMMAND /bin/sh ${CMAKE_CURRENT_SOURCE_DIR}/platform/ios/patch-info-plist.sh
        VERBATIM)
endif()

if(ANDROID)
    set_target_properties(qupil-student PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/platform/android
        QT_ANDROID_PACKAGE_NAME de.qupil.student
        QT_ANDROID_VERSION_NAME ${PROJECT_VERSION})
    if(QUPIL_BUILD_NUMBER)
        set_target_properties(qupil-student PROPERTIES QT_ANDROID_VERSION_CODE ${QUPIL_BUILD_NUMBER})
    endif()
    set(QUPIL_ANDROID_OPENSSL "" CACHE PATH "Checkout of KDAB/android_openssl (HTTPS on Android)")
    set(_ssl "${QUPIL_ANDROID_OPENSSL}/ssl_3/${CMAKE_ANDROID_ARCH_ABI}")
    if(QUPIL_ANDROID_OPENSSL AND EXISTS "${_ssl}/libssl_3.so" AND EXISTS "${_ssl}/libcrypto_3.so")
        set_property(TARGET qupil-student APPEND PROPERTY QT_ANDROID_EXTRA_LIBS "${_ssl}/libcrypto_3.so" "${_ssl}/libssl_3.so")
    else()
        message(WARNING "No OpenSSL for Android (QUPIL_ANDROID_OPENSSL): the app cannot load booklets (HTTPS)")
    endif()
endif()

# ---- tests
option(QUPIL_STUDENT_TESTS "Build the student app's tests" ON)
if(QUPIL_STUDENT_TESTS AND NOT ANDROID AND NOT IOS)
    enable_testing()
    find_package(Qt6 REQUIRED COMPONENTS Test)
    qt_add_executable(tst_booklet tests/tst_booklet.cpp)
    target_link_libraries(tst_booklet PRIVATE qupilbooklet Qt6::Test)
    qt_add_executable(tst_teacherside tests/tst_teacherside.cpp tests/FakeNextcloud.h tests/FakeNextcloud.cpp)
    target_link_libraries(tst_teacherside PRIVATE qupilbooklet Qt6::Test)
    add_test(NAME tst_teacherside COMMAND tst_teacherside -platform offscreen)
    qt_add_executable(tst_studentapp tests/tst_studentapp.cpp tests/FakeNextcloud.h tests/FakeNextcloud.cpp)
    target_link_libraries(tst_studentapp PRIVATE qupilstudentui qupilstudentuiplugin Qt6::Test)
    set_target_properties(tst_booklet tst_teacherside tst_studentapp PROPERTIES MACOSX_BUNDLE FALSE WIN32_EXECUTABLE FALSE)
    add_test(NAME tst_booklet COMMAND tst_booklet)
    add_test(NAME tst_studentapp COMMAND tst_studentapp -platform offscreen)
endif()

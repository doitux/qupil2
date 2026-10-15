# SPDX-FileCopyrightText: 2006-2026 Felix Hammer
# SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

cmake_minimum_required(VERSION 3.21)

project(qupil
    VERSION 2.0.0
    DESCRIPTION "Lesson management for music teachers"
    HOMEPAGE_URL "https://qupil.de"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sources contain UTF-8 text (language names, note names)
if(MSVC)
    add_compile_options(/utf-8)
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 LinguistTools)
qt_standard_project_setup(REQUIRES 6.5)

# ---------------------------------------------------------------------------- app

qt_add_executable(qupil src/main.cpp)

target_compile_definitions(qupil PRIVATE QUPIL_VERSION="${PROJECT_VERSION}")
target_include_directories(qupil PRIVATE src src/core)
target_link_libraries(qupil PRIVATE Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickControls2)

if(APPLE OR WIN32)
    set_target_properties(qupil PROPERTIES OUTPUT_NAME Qupil)
endif()

set_target_properties(qupil PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME Qupil
    MACOSX_BUNDLE_GUI_IDENTIFIER de.qupil.app
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
    MACOSX_BUNDLE_COPYRIGHT "© 2006–2026 Felix Hammer"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER de.qupil.app)

if(WIN32)
    target_sources(qupil PRIVATE platform/windows/qupil.rc)
elseif(APPLE AND NOT IOS)
    set(QUPIL_ICNS ${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/qupil.icns)
    set_source_files_properties(${QUPIL_ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    target_sources(qupil PRIVATE ${QUPIL_ICNS})
    set_target_properties(qupil PROPERTIES MACOSX_BUNDLE_ICON_FILE qupil.icns)
endif()

set(QUPIL_QML_FILES
    qml/Main.qml
    qml/Theme.qml
    qml/Icon.qml
    qml/NavButton.qml
    qml/AppearanceSettings.qml
    qml/pages/TodayPage.qml
    qml/pages/PlaceholderPage.qml
    qml/pages/MorePage.qml)

# All QML files sit next to the module's qmldir in the resources, so every file
# sees all types of the module (and the Theme singleton) without an import.
foreach(_qml IN LISTS QUPIL_QML_FILES)
    get_filename_component(_name "${_qml}" NAME)
    set_source_files_properties(${_qml} PROPERTIES QT_RESOURCE_ALIAS ${_name})
endforeach()

# Theme.qml is a singleton – must be set before qt_add_qml_module()
set_source_files_properties(qml/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(qupil
    URI Qupil
    VERSION 1.0
    QML_FILES ${QUPIL_QML_FILES}
    RESOURCES
        assets/fonts/Figtree-Regular.ttf
        assets/fonts/Figtree-SemiBold.ttf
        assets/fonts/Figtree-Bold.ttf
        assets/fonts/Figtree-ExtraBold.ttf
        assets/icons/qupil.png
    SOURCES
        src/core/LanguageManager.h
        src/core/LanguageManager.cpp
)

# ---------------------------------------------------------------------------- translations
# i18n/*.ts are generated from tools/i18n/catalog.py (see docs/i18n.md).
# Do not run the "update_translations" target: lupdate would rewrite them.

set(QUPIL_TS_FILES
    i18n/qupil_en.ts
    i18n/qupil_de.ts
    i18n/qupil_fr.ts
    i18n/qupil_es.ts)

set(_qupil_tr_extra "")
if(Qt6_VERSION VERSION_GREATER_EQUAL 6.7)
    list(APPEND _qupil_tr_extra IMMEDIATE_CALL)
endif()

qt_add_translations(qupil
    TS_FILES ${QUPIL_TS_FILES}
    LUPDATE_OPTIONS -locations none
    LRELEASE_OPTIONS -idbased
    QM_FILES_OUTPUT_VARIABLE QUPIL_QM_FILES
    ${_qupil_tr_extra})

# LanguageManager loads :/i18n/qupil_<lang>.qm
qt_add_resources(qupil "qupil_translations"
    PREFIX "/i18n"
    BASE "${CMAKE_CURRENT_BINARY_DIR}"
    FILES ${QUPIL_QM_FILES})

# Qt's own strings (dialog buttons etc.) are not part of mobile installs
if(ANDROID OR IOS)
    set(_qt_tr_dir "${QT6_INSTALL_PREFIX}/${QT6_INSTALL_TRANSLATIONS}")
    set(_qt_qm "")
    foreach(_lang de fr es)
        if(EXISTS "${_qt_tr_dir}/qtbase_${_lang}.qm")
            list(APPEND _qt_qm "${_qt_tr_dir}/qtbase_${_lang}.qm")
        endif()
    endforeach()
    if(_qt_qm)
        qt_add_resources(qupil "qt_translations" PREFIX "/i18n" BASE "${_qt_tr_dir}" FILES ${_qt_qm})
    endif()
endif()

# ---------------------------------------------------------------------------- install / deploy

include(GNUInstallDirs)
install(TARGETS qupil
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(NOT ANDROID AND NOT IOS)
    qt_generate_deploy_qml_app_script(
        TARGET qupil
        OUTPUT_SCRIPT qupil_deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR)
    install(SCRIPT ${qupil_deploy_script})
endif()

# ---------------------------------------------------------------------------- tests

option(QUPIL_BUILD_TESTS "Build the unit tests" ON)

if(QUPIL_BUILD_TESTS AND NOT ANDROID AND NOT IOS)
    enable_testing()
    find_package(Qt6 REQUIRED COMPONENTS Test)

    qt_add_executable(tst_languagemanager
        tests/tst_languagemanager.cpp
        src/core/LanguageManager.h
        src/core/LanguageManager.cpp)
    target_include_directories(tst_languagemanager PRIVATE src/core)
    target_link_libraries(tst_languagemanager PRIVATE Qt6::Core Qt6::Qml Qt6::Test)
    set_target_properties(tst_languagemanager PROPERTIES MACOSX_BUNDLE FALSE WIN32_EXECUTABLE FALSE)
    qt_add_resources(tst_languagemanager "tst_translations"
        PREFIX "/i18n"
        BASE "${CMAKE_CURRENT_BINARY_DIR}"
        FILES ${QUPIL_QM_FILES})
    add_test(NAME languagemanager COMMAND tst_languagemanager)

    # Loads the whole QML UI once and fails on any QML warning
    add_test(NAME qml_smoke COMMAND qupil --smoke-test)
    set_tests_properties(languagemanager qml_smoke PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endif()

# SPDX-FileCopyrightText: 2006-2026 Felix Hammer
# SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

cmake_minimum_required(VERSION 3.21)

project(qupil
    VERSION 2.0.0
    DESCRIPTION "Lesson management for music teachers"
    HOMEPAGE_URL "https://qupil.de"
    LANGUAGES C CXX)

# Objective-C++ for the system notifications on iOS
if(IOS)
    enable_language(OBJCXX)
    # Xcode archives (TestFlight, App Store) may only contain the app itself:
    # no static libraries or Qt plugins (the app target switches this back on below)
    set(CMAKE_XCODE_ATTRIBUTE_SKIP_INSTALL YES)
endif()

# CFBundleVersion: every upload to TestFlight/App Store needs a higher one, so the
# TestFlight workflow passes its run number; otherwise it is the project version
set(QUPIL_BUILD_NUMBER "" CACHE STRING "Build number (CFBundleVersion) for App Store uploads")
if(QUPIL_BUILD_NUMBER)
    set(QUPIL_BUNDLE_VERSION ${QUPIL_BUILD_NUMBER})
else()
    set(QUPIL_BUNDLE_VERSION ${PROJECT_VERSION})
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sources contain UTF-8 text (language names, note names)
if(MSVC)
    add_compile_options(/utf-8)
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Sql Qml Quick QuickControls2 Multimedia Network LinguistTools)
qt_standard_project_setup(REQUIRES 6.5)

# ---------------------------------------------------------------------------- data core
# Database, change log for sync, import of Qupil 1.x – no GUI, unit tested

qt_add_library(qupilcore STATIC
    src/core/Ids.h
    src/core/Ids.cpp
    src/core/Clock.h
    src/core/Clock.cpp
    src/core/Schema.h
    src/core/Schema.cpp
    src/core/Database.h
    src/core/Database.cpp
    src/core/Store.h
    src/core/Store.cpp
    src/core/LegacyImporter.h
    src/core/LegacyImporter.cpp
    src/core/TakeOver.h
    src/core/TakeOver.cpp
    src/core/DemoData.h
    src/core/DemoData.cpp
    src/core/WifiSession.h
    src/core/WifiSession.cpp
    src/core/Mdns.h
    src/core/Mdns.cpp
    src/core/Csv.h
    src/core/Csv.cpp
    src/core/Wav.h
    src/core/Wav.cpp
    src/core/LessonAlerts.h
    src/core/LessonAlerts.cpp
    src/core/Crypto.h
    src/core/Crypto.cpp
    src/core/FolderSync.h
    src/core/FolderSync.cpp
    src/core/TextPack.h
    src/core/TextPack.cpp
    3rdparty/monocypher/monocypher.c
    3rdparty/monocypher/monocypher.h)
target_include_directories(qupilcore PUBLIC src/core PRIVATE 3rdparty/monocypher)
target_link_libraries(qupilcore PUBLIC Qt6::Core Qt6::Sql)

# ---------------------------------------------------------------------------- app

qt_add_executable(qupil src/main.cpp src/app/ScreenshotTour.h src/app/ScreenshotTour.cpp)

target_compile_definitions(qupil PRIVATE QUPIL_VERSION="${PROJECT_VERSION}")
# Which build this is (shown under "About"): the commit on CI, "dev" otherwise
if(DEFINED ENV{GITHUB_SHA})
    string(SUBSTRING "$ENV{GITHUB_SHA}" 0 7 QUPIL_BUILD)
else()
    set(QUPIL_BUILD "dev")
endif()
target_compile_definitions(qupil PRIVATE QUPIL_BUILD="${QUPIL_BUILD}")
target_include_directories(qupil PRIVATE src src/core src/app)
# Finding devices in the network: Bonjour on Apple systems, our own mDNS elsewhere
if(APPLE)
    target_sources(qupil PRIVATE src/app/BonjourDiscovery.h src/app/BonjourDiscovery.cpp)
else()
    target_sources(qupil PRIVATE src/app/MdnsDiscovery.h src/app/MdnsDiscovery.cpp)
endif()
# Lesson-end alerts: system notifications on iOS and Android (also in the background),
# Qupil plays them itself on desktop systems
if(IOS)
    target_sources(qupil PRIVATE src/app/SystemAlerts_ios.mm)
    set_source_files_properties(src/app/SystemAlerts_ios.mm PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
    target_link_libraries(qupil PRIVATE "-framework UserNotifications" "-framework UIKit")
elseif(ANDROID)
    target_sources(qupil PRIVATE src/app/SystemAlerts_android.cpp)
else()
    target_sources(qupil PRIVATE src/app/SystemAlerts_none.cpp)
endif()
# Linux: alert sounds through the system's player, in a process of its own
if(UNIX AND NOT APPLE AND NOT ANDROID)
    target_sources(qupil PRIVATE src/app/AlertPlayer.h src/app/AlertPlayer.cpp)
endif()
# Where a backup goes: iOS has its own "Save to Files" picker (Qt's dialog cannot save there)
target_sources(qupil PRIVATE src/app/FileExport.h)
if(IOS)
    target_sources(qupil PRIVATE src/app/FileExport_ios.mm)
    set_source_files_properties(src/app/FileExport_ios.mm PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
else()
    target_sources(qupil PRIVATE src/app/FileExport_none.cpp)
endif()
target_link_libraries(qupil PRIVATE qupilcore Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickControls2
    Qt6::Multimedia Qt6::Network)

if(APPLE OR WIN32)
    set_target_properties(qupil PROPERTIES OUTPUT_NAME Qupil)
endif()

set_target_properties(qupil PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME Qupil
    MACOSX_BUNDLE_GUI_IDENTIFIER de.qupil.app
    MACOSX_BUNDLE_BUNDLE_VERSION ${QUPIL_BUNDLE_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}
    MACOSX_BUNDLE_COPYRIGHT "© 2006–2026 Felix Hammer"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER de.qupil.app)

if(IOS)
    # iPhone and iPad
    set_target_properties(qupil PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2")
    # Home screen icon (single 1024 px image, Xcode renders all sizes)
    set(QUPIL_ASSET_CATALOG ${CMAKE_CURRENT_SOURCE_DIR}/platform/ios/Assets.xcassets)
    target_sources(qupil PRIVATE ${QUPIL_ASSET_CATALOG})
    set_source_files_properties(${QUPIL_ASSET_CATALOG} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_target_properties(qupil PROPERTIES XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon)
    # Translated permission texts
    foreach(_lang en de fr es)
        set(_strings ${CMAKE_CURRENT_SOURCE_DIR}/platform/ios/${_lang}.lproj/InfoPlist.strings)
        target_sources(qupil PRIVATE ${_strings})
        # Xcode keeps the <lang>.lproj folder of the source path itself
        # (Resources/<lang>.lproj would end up as <lang>.lproj/<lang>.lproj)
        set_source_files_properties(${_strings} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    endforeach()
    # Privacy manifest (required reason APIs of Qt and Qupil; no tracking, no data collected)
    set(_privacy ${CMAKE_CURRENT_SOURCE_DIR}/platform/ios/PrivacyInfo.xcprivacy)
    target_sources(qupil PRIVATE ${_privacy})
    set_source_files_properties(${_privacy} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    # Product › Archive: the app goes into the archive (Products/Applications)
    set_target_properties(qupil PROPERTIES
        XCODE_ATTRIBUTE_SKIP_INSTALL NO
        XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)")
    # Qupil only needs audio output (metronome): the native Darwin backend is enough
    # and keeps the FFmpeg libraries out of the app
    qt_import_plugins(qupil INCLUDE_BY_TYPE multimedia Qt6::QDarwinMediaPlugin)
    # Files app access, languages, Wi-Fi sync keys (added to Qt's generated Info.plist)
    # (the script takes the bundle path from Xcode's build settings)
    add_custom_command(TARGET qupil POST_BUILD
        COMMAND /bin/sh ${CMAKE_CURRENT_SOURCE_DIR}/platform/ios/patch-info-plist.sh
        VERBATIM)
endif()

if(APPLE AND NOT IOS)
    # macOS asks before an app may use the local network (Wi-Fi sync)
    set(_plist "$<TARGET_BUNDLE_CONTENT_DIR:qupil>/Info.plist")
    add_custom_command(TARGET qupil POST_BUILD
        COMMAND plutil -replace NSLocalNetworkUsageDescription -string
                "Qupil uses your local network to sync with your other devices over Wi-Fi." ${_plist}
        COMMAND plutil -replace NSBonjourServices -json "[\"_qupil-sync._tcp\"]" ${_plist}
        VERBATIM)
endif()

if(ANDROID)
    # Own manifest (Qt's template + Wi-Fi sync permissions, per-app language)
    set_target_properties(qupil PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/platform/android
        QT_ANDROID_PACKAGE_NAME de.qupil.app
        QT_ANDROID_VERSION_NAME ${PROJECT_VERSION})
    # Every CI build counts up, so Android installs it as an update; Android's app
    # info shows build number and commit (as in the file name of the test APK)
    if(DEFINED ENV{GITHUB_RUN_NUMBER})
        string(SUBSTRING "$ENV{GITHUB_SHA}" 0 7 _sha)
        set_target_properties(qupil PROPERTIES
            QT_ANDROID_VERSION_CODE $ENV{GITHUB_RUN_NUMBER}
            QT_ANDROID_VERSION_NAME "${PROJECT_VERSION} (build $ENV{GITHUB_RUN_NUMBER}, ${_sha})")
    endif()
    # HTTPS (the Nextcloud sync) needs OpenSSL, which Qt does not bring for Android:
    # the prebuilt libraries of https://github.com/KDAB/android_openssl go into the APK
    set(QUPIL_ANDROID_OPENSSL "" CACHE PATH "Checkout of KDAB/android_openssl (HTTPS on Android)")
    set(_ssl "${QUPIL_ANDROID_OPENSSL}/ssl_3/${CMAKE_ANDROID_ARCH_ABI}")
    if(QUPIL_ANDROID_OPENSSL AND EXISTS "${_ssl}/libssl_3.so" AND EXISTS "${_ssl}/libcrypto_3.so")
        set_property(TARGET qupil APPEND PROPERTY QT_ANDROID_EXTRA_LIBS "${_ssl}/libcrypto_3.so" "${_ssl}/libssl_3.so")
        message(STATUS "OpenSSL for Android: ${_ssl}")
    else()
        message(WARNING "No OpenSSL for Android (QUPIL_ANDROID_OPENSSL): the Nextcloud sync cannot connect (HTTPS)")
    endif()
endif()

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
    qml/Nav.qml
    qml/Labels.qml
    qml/Icon.qml
    qml/NavButton.qml
    qml/ActionButton.qml
    qml/Card.qml
    qml/AppDialog.qml
    qml/AppearanceSettings.qml
    qml/DataSettings.qml
    qml/LessonEndSettings.qml
    qml/TakeOverActions.qml
    qml/TakeOverSummary.qml
    qml/components/AppMenu.qml
    qml/components/AppMenuItem.qml
    qml/components/AppMenuSeparator.qml
    qml/components/Avatar.qml
    qml/components/BuildChip.qml
    qml/components/Chip.qml
    qml/components/ChoiceOption.qml
    qml/components/ChoiceField.qml
    qml/components/DateField.qml
    qml/components/EmptyHint.qml
    qml/components/Fab.qml
    qml/components/FieldLabel.qml
    qml/components/IconButton.qml
    qml/components/ListRow.qml
    qml/components/InputField.qml
    qml/components/ListEditor.qml
    qml/components/MenuGroup.qml
    qml/components/MenuRow.qml
    qml/components/PageHeader.qml
    qml/components/SectionLabel.qml
    qml/components/SoundChooser.qml
    qml/components/Segmented.qml
    qml/components/Sheet.qml
    qml/components/StatusBadge.qml
    qml/components/Stepper.qml
    qml/components/SubPage.qml
    qml/components/StudentPicker.qml
    qml/components/SwitchRow.qml
    qml/components/SyncCard.qml
    qml/components/TimeField.qml
    qml/components/Tip.qml
    qml/editors/ActivityEditor.qml
    qml/editors/PairingSheet.qml
    qml/editors/LibraryItemEditor.qml
    qml/editors/DayOverviewSheet.qml
    qml/editors/CsvImportSheet.qml
    qml/editors/EventEditor.qml
    qml/editors/ExternalEditor.qml
    qml/editors/LessonEditor.qml
    qml/editors/NewStudent.qml
    qml/editors/NoteEditor.qml
    qml/editors/PieceEditor.qml
    qml/editors/ReadyPiecesPicker.qml
    qml/editors/ReminderEditor.qml
    qml/pages/SetupPage.qml
    qml/pages/SchedulePage.qml
    qml/pages/ScheduleOverview.qml
    qml/pages/LessonDetail.qml
    qml/pages/StudentsPage.qml
    qml/pages/StudentDetail.qml
    qml/pages/StudentForm.qml
    qml/pages/EventsPage.qml
    qml/pages/EventDetail.qml
    qml/pages/MetronomePage.qml
    qml/pages/MorePage.qml
    qml/pages/RemindersPage.qml
    qml/pages/BirthdaysPage.qml
    qml/pages/CandidatesPage.qml
    qml/pages/SettingsPage.qml
    qml/pages/AppearancePage.qml
    qml/pages/AlertSettingsPage.qml
    qml/pages/LessonSettingsPage.qml
    qml/pages/ListsPage.qml
    qml/pages/DataPage.qml
    qml/pages/SyncPage.qml
    qml/pages/LibraryPage.qml
    qml/pages/InstrumentsPage.qml
    qml/pages/EnsembleCandidatesPage.qml
    qml/pages/AboutPage.qml
    qml/pages/ArchivePage.qml
    qml/pages/LicensePage.qml
    qml/pages/ArchiveEntryPage.qml
    qml/pages/PlaceholderPage.qml
    qml/pages/ReadyPiecesPage.qml)

# All QML files sit next to the module's qmldir in the resources, so every file
# sees all types of the module (and the Theme singleton) without an import.
foreach(_qml IN LISTS QUPIL_QML_FILES)
    get_filename_component(_name "${_qml}" NAME)
    set_source_files_properties(${_qml} PROPERTIES QT_RESOURCE_ALIAS ${_name})
endforeach()

# Theme.qml is a singleton – must be set before qt_add_qml_module()
set_source_files_properties(qml/Theme.qml qml/Nav.qml qml/Labels.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_qml_module(qupil
    URI Qupil
    VERSION 1.0
    QML_FILES ${QUPIL_QML_FILES}
    RESOURCES
        assets/fonts/Figtree-Regular.ttf
        assets/fonts/Figtree-SemiBold.ttf
        assets/fonts/Figtree-Bold.ttf
        assets/fonts/Figtree-ExtraBold.ttf
        assets/icons/qupil-16.png
        assets/icons/qupil-24.png
        assets/icons/qupil-32.png
        assets/icons/qupil-48.png
        assets/icons/qupil-64.png
        assets/icons/qupil-128.png
        assets/icons/qupil-256.png
        assets/sounds/lesson-end.wav
        assets/sounds/reminder.wav
        LICENSE
        LICENSES/LicenseRef-Qupil-AppStore-exception.txt
        LICENSES/OFL-1.1.txt
        LICENSES/Apache-2.0.txt
        3rdparty/monocypher/LICENCE.md
    SOURCES
        src/core/LanguageManager.h
        src/core/LanguageManager.cpp
        src/app/AppController.h
        src/app/AppController.cpp
        src/app/BackKey.h
        src/app/BackKey.cpp
        src/app/DataService.h
        src/app/DataService.cpp
        src/app/NoteFormat.h
        src/app/NoteFormat.cpp
        src/app/ArchiveFormat.h
        src/app/ArchiveFormat.cpp
        src/app/Metronome.h
        src/app/Metronome.cpp
        src/app/TextFormatter.h
        src/app/TextFormatter.cpp
        src/app/WebDavFolder.h
        src/app/WebDavFolder.cpp
        src/app/SyncController.h
        src/app/SyncController.cpp
        src/app/WifiController.h
        src/app/WifiController.cpp
        src/app/SocketChannel.h
        src/app/Discovery.h
        src/app/DocumentService.h
        src/app/DocumentService.cpp
        src/app/AlertSound.h
        src/app/AlertSound.cpp
        src/app/LessonEndAlert.h
        src/app/LessonEndAlert.cpp
        src/app/ReminderAlert.h
        src/app/ReminderAlert.cpp
        src/app/SystemAlerts.h
        src/app/SystemAlerts.cpp
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

if(UNIX AND NOT APPLE AND NOT ANDROID)
    # Desktop entry and icon theme files (menus, docks; Wayland takes the window icon from here)
    install(FILES platform/linux/de.qupil.app.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    foreach(_size 16 24 32 48 64 128 256 512)
        install(FILES assets/icons/qupil-${_size}.png RENAME de.qupil.app.png
            DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/${_size}x${_size}/apps)
    endforeach()
    install(FILES assets/icons/qupil.svg RENAME de.qupil.app.svg
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
endif()

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

    # Data core: ids, clock, change log, sync merge, 1.x import, backup
    qt_add_executable(tst_core tests/tst_core.cpp tests/LegacyFixture.h)
    target_link_libraries(tst_core PRIVATE qupilcore Qt6::Test)
    set_target_properties(tst_core PROPERTIES MACOSX_BUNDLE FALSE WIN32_EXECUTABLE FALSE)
    add_test(NAME core COMMAND tst_core)

    # Lesson notes and archive entries: the compact formats, old HTML of Qupil 1.x
    qt_add_executable(tst_notes tests/tst_notes.cpp src/app/NoteFormat.h src/app/NoteFormat.cpp
        src/app/ArchiveFormat.h src/app/ArchiveFormat.cpp src/core/TextPack.h src/core/TextPack.cpp)
    target_include_directories(tst_notes PRIVATE src/app src/core)
    target_link_libraries(tst_notes PRIVATE Qt6::Gui Qt6::Test)
    set_target_properties(tst_notes PROPERTIES MACOSX_BUNDLE FALSE WIN32_EXECUTABLE FALSE)
    add_test(NAME notes COMMAND tst_notes)
    set_tests_properties(notes PROPERTIES TIMEOUT 60)

    # Loads the whole QML UI once and fails on any QML warning
    add_test(NAME qml_smoke COMMAND qupil --smoke-test)
    set_tests_properties(languagemanager core notes qml_smoke PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endif()

cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# Release Service version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "21")
set (RELEASE_SERVICE_VERSION_MINOR "03")
set (RELEASE_SERVICE_VERSION_MICRO "70")
set (RELEASE_SERVICE_COMPACT_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}${RELEASE_SERVICE_VERSION_MINOR}${RELEASE_SERVICE_VERSION_MICRO}")

# Bump KREVERSI_BASE_VERSION once new features are added
set(KREVERSI_BASE_VERSION "2.1")
set(KREVERSI_VERSION "${KREVERSI_BASE_VERSION}.${RELEASE_SERVICE_COMPACT_VERSION}")

project(kreversi VERSION ${KREVERSI_VERSION})

set (QT_MIN_VERSION "5.12.0")
set (KF5_MIN_VERSION "5.71.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets Qml Quick QuickWidgets Svg Test)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Config
    ConfigWidgets
    CoreAddons
    Crash
    DBusAddons
    Declarative
    DocTools
    I18n
    IconThemes
    KIO
    XmlGui
    WidgetsAddons
)

find_package(KF5KDEGames 4.9.0 REQUIRED)

include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMSetupVersion)

set(CMAKE_CXX_STANDARD 14)
add_definitions(
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
#    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_USE_QSTRINGBUILDER
    -DQT_STRICT_ITERATORS
    -DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT
    -DQT_NO_KEYWORDS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050700
    -DQT_DEPRECATED_WARNINGS_SINCE=0x060000
    -DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x051E00
    -DKF_DEPRECATED_WARNINGS_SINCE=0x060000
)
if (${KF5Config_VERSION} STRGREATER "5.56.0")
        add_definitions(-DQT_NO_FOREACH)
        MESSAGE(STATUS "compile without foreach")
endif()

add_subdirectory(pics)
add_subdirectory(sounds)
add_subdirectory(icons)
add_subdirectory(doc)


########### next target ##############
set(kreversi_SRCS
    commondefs.cpp
    colorscheme.cpp
    kreversigame.cpp
    kreversiview.cpp
    kreversiplayer.cpp
    kreversihumanplayer.cpp
    kreversicomputerplayer.cpp
    startgamedialog.cpp
    Engine.cpp
    highscores.cpp
    kexthighscore.cpp
    kexthighscore_gui.cpp
    kexthighscore_internal.cpp
    kexthighscore_item.cpp
    kexthighscore_tab.cpp
    mainwindow.cpp
    main.cpp
)
ecm_setup_version(${KREVERSI_VERSION}
    VARIABLE_PREFIX KREVERSI
    VERSION_HEADER kreversi_version.h
)

qt5_add_resources(kreversi_SRCS kreversi.qrc)
ki18n_wrap_ui(kreversi_SRCS startgamedialog.ui)

kconfig_add_kcfg_files(kreversi_SRCS preferences.kcfgc)

file(GLOB ICON_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/hicolor/*-apps-kreversi.png")
ecm_add_app_icon(kreversi_SRCS ICONS ${ICON_SRCS})
add_executable(kreversi ${kreversi_SRCS})

target_link_libraries(kreversi
    Qt5::Svg
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::CoreAddons
    KF5::Crash
    KF5::DBusAddons
    KF5::Declarative
    KF5::IconThemes
    KF5::KIOCore
    KF5::KIOFileWidgets
    KF5::WidgetsAddons
    KF5::XmlGui
    KF5KDEGames
)

install(TARGETS kreversi  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(PROGRAMS org.kde.kreversi.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(DIRECTORY qml DESTINATION ${KDE_INSTALL_DATADIR}/kreversi)
install(FILES org.kde.kreversi.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

ki18n_install(po)
kdoctools_install(po)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

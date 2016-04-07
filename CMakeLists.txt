project(kreversi)
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.3.0")
set (KF5_MIN_VERSION "5.15.0")

find_package(ECM 1.7.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets Qml Quick QuickWidgets Svg Test)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Completion
    Config
    ConfigWidgets
    CoreAddons
    Crash
    DBusAddons
    DNSSD
    Declarative
    GuiAddons
    I18n
    IconThemes
    ItemModels
    ItemViews
    KDELibs4Support
    KIO
    NewStuff
    Service
    WidgetsAddons
    XmlGui
)

find_package(KF5KDEGames 4.9.0 REQUIRED)

include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

add_definitions(
    -DQT_USE_FAST_CONCATENATION
    -DQT_USE_FAST_OPERATOR_PLUS
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
#    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_USE_QSTRINGBUILDER
)

add_subdirectory(pics)
add_subdirectory(sounds)
add_subdirectory(icons)
add_subdirectory(doc)

include_directories(${CMAKE_SOURCE_DIR}/libKF5KDEGames/highscore)

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

ki18n_wrap_ui(kreversi_SRCS startgamedialog.ui)

kconfig_add_kcfg_files(kreversi_SRCS preferences.kcfgc)

add_executable(kreversi ${kreversi_SRCS})

target_link_libraries(kreversi
    Qt5::Svg
    KF5::Completion
    KF5::Crash
    KF5::DBusAddons
    KF5::Declarative
    KF5::KDELibs4Support
    KF5::TextWidgets
    KF5::XmlGui
    KF5KDEGames
)

install(TARGETS kreversi  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(PROGRAMS org.kde.kreversi.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
#install(FILES kreversi.kcfg  DESTINATION  ${KDE_INSTALL_KCFGDIR})
install(FILES kreversiui.rc  DESTINATION  ${KDE_INSTALL_KXMLGUI5DIR}/kreversi)
install(DIRECTORY qml DESTINATION ${KDE_INSTALL_DATADIR}/kreversi)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

project(kreversi)
cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets Qml Quick QuickWidgets Svg Test Declarative)
find_package(KF5 REQUIRED COMPONENTS
  CoreAddons 
  Config 
  ItemModels 
  WidgetsAddons 
  WindowSystem 
  Codecs 
  Archive
  Config 
  DBusAddons
  DNSSD
  Declarative
  I18n
  GuiAddons
  Service
  ConfigWidgets
  ItemViews
  IconThemes
  Completion
  JobWidgets
  TextWidgets
  GlobalAccel
  XmlGui
  Crash
  Bookmarks
  KIO
  NotifyConfig
  NewStuff
  KDELibs4Support)

find_package(KF5KDEGames 4.9.0 REQUIRED)

include(FeatureSummary)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)

include_directories(${KDEGAMES_INCLUDE_DIRS} ${KDEGAMES_INCLUDE_DIR}/highscore ${KDE4_INCLUDES} ${QDBUS_INCLUDE_DIRS})

add_subdirectory( pics )
add_subdirectory( sounds )
add_subdirectory( icons )
add_subdirectory( doc )

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames/highscore )

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
    mainwindow.cpp
    main.cpp )

qt5_wrap_ui(kreversi_SRCS startgamedialog.ui)

kconfig_add_kcfg_files(kreversi_SRCS preferences.kcfgc)

add_executable(kreversi ${kreversi_SRCS})

target_link_libraries(kreversi Qt5::Svg Qt5::Declarative KF5::Declarative KF5::XmlGui KF5::TextWidgets KF5KDEGames KF5::Completion KF5::KDELibs4Support)

install(TARGETS kreversi  ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ###############

install( PROGRAMS org.kde.kreversi.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
#install( FILES kreversi.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES kreversiui.rc  DESTINATION  ${KXMLGUI_INSTALL_DIR}/kreversi )	
install( DIRECTORY qml DESTINATION ${DATA_INSTALL_DIR}/kreversi )

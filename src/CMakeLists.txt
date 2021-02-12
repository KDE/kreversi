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
    kreversi.qrc
)
ecm_setup_version(${KREVERSI_VERSION}
    VARIABLE_PREFIX KREVERSI
    VERSION_HEADER kreversi_version.h
)

ki18n_wrap_ui(kreversi_SRCS startgamedialog.ui)

kconfig_add_kcfg_files(kreversi_SRCS preferences.kcfgc)

file(GLOB ICON_SRCS "${CMAKE_SOURCE_DIR}/icons/hicolor/*-apps-kreversi.png")
ecm_add_app_icon(kreversi_SRCS ICONS ${ICON_SRCS})
add_executable(kreversi ${kreversi_SRCS})

target_link_libraries(kreversi
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
    Qt5::Svg
)

install(TARGETS kreversi  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(DIRECTORY qml DESTINATION ${KDE_INSTALL_DATADIR}/kreversi)

install(PROGRAMS org.kde.kreversi.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES org.kde.kreversi.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

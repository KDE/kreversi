ecm_setup_version(${KREVERSI_VERSION}
    VARIABLE_PREFIX KREVERSI
    VERSION_HEADER kreversi_version.h
)

add_executable(kreversi)

target_sources(kreversi PRIVATE
    colorscheme.cpp
    colorscheme.h
    commondefs.cpp
    commondefs.h
    Engine.cpp
    Engine.h
    gamestartinformation.h
    highscores.cpp
    highscores.h
    kexthighscore.cpp
    kexthighscore_gui.cpp
    kexthighscore_gui.h
    kexthighscore.h
    kexthighscore_internal.cpp
    kexthighscore_internal.h
    kexthighscore_item.cpp
    kexthighscore_item.h
    kexthighscore_tab.cpp
    kexthighscore_tab.h
    kreversicomputerplayer.cpp
    kreversicomputerplayer.h
    kreversigame.cpp
    kreversigame.h
    kreversihumanplayer.cpp
    kreversihumanplayer.h
    kreversiplayer.cpp
    kreversiplayer.h
    kreversiview.cpp
    kreversiview.h
    main.cpp
    mainwindow.cpp
    mainwindow.h
    startgamedialog.cpp
    startgamedialog.h


    kreversi.qrc
)

ki18n_wrap_ui(kreversi startgamedialog.ui)

kconfig_add_kcfg_files(kreversi preferences.kcfgc)

file(GLOB ICON_SRCS "${CMAKE_SOURCE_DIR}/icons/hicolor/*-apps-kreversi.png")
ecm_add_app_icon(kreversi ICONS ${ICON_SRCS})

target_link_libraries(kreversi
    KF5KDEGames
    KF5::ConfigCore
    KF5::ConfigGui
    KF5::CoreAddons
    KF5::Crash
    KF5::DBusAddons
    KF5::KIOCore
    KF5::KIOFileWidgets
    KF5::WidgetsAddons
    KF5::XmlGui
    Qt${QT_MAJOR_VERSION}::Svg
)

install(TARGETS kreversi  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(DIRECTORY qml DESTINATION ${KDE_INSTALL_DATADIR}/kreversi)

install(PROGRAMS org.kde.kreversi.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES org.kde.kreversi.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

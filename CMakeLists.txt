project(kreversi)
add_subdirectory( pics ) 
add_subdirectory( sounds ) 
add_subdirectory( icons ) 

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames/highscore)

########### next target ###############

set(kreversi_SRCS 
    kreversichip.cpp
    kreversigame.cpp
    kreversirenderer.cpp
    kreversiscene.cpp
    kreversiview.cpp
    Engine.cpp
    highscores.cpp
    mainwindow.cpp
    main.cpp )

kde4_add_kcfg_files(kreversi_SRCS preferences.kcfgc)

set(kreversi_icons
	icons/hi16-app-kreversi.png
	icons/hi32-app-kreversi.png
	icons/hi48-app-kreversi.png
	icons/hi64-app-kreversi.png
	icons/hi128-app-kreversi.png
)
kde4_add_win32_app_icon(kreversi_SRCS ${kreversi_icons})
kde4_add_executable(kreversi ${kreversi_SRCS})

target_link_libraries(kreversi ${KDE4_KDEUI_LIBS} kdegames ${KGGZGAMES_LIBS})
install(TARGETS kreversi  DESTINATION ${BIN_INSTALL_DIR} )

register_ggz_module(module.dsc)

########### install files ###############

install( FILES kreversi.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
#install( FILES kreversi.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES kreversiui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kreversi )


project(kreversi)

if(NOT COMPILING_KDEGAMES)
	find_package(KDE4 REQUIRED)
	include(KDE4Defaults)
	include(MacroLibrary)
	find_package(KDEGames 4.9.0 REQUIRED)

	add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
	add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
	include_directories(${KDEGAMES_INCLUDE_DIRS} ${KDEGAMES_INCLUDE_DIR}/highscore ${KDE4_INCLUDES} ${QDBUS_INCLUDE_DIRS})
endif(NOT COMPILING_KDEGAMES)

add_subdirectory( pics ) 
add_subdirectory( sounds ) 
add_subdirectory( icons ) 
add_subdirectory( doc )

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames/highscore)

########### next target ###############

set(kreversi_SRCS 
    kreversichip.cpp
    kreversigame.cpp
    kreversiscene.cpp
    kreversiview.cpp
    Engine.cpp
    highscores.cpp
    mainwindow.cpp
    main.cpp )

kde4_add_kcfg_files(kreversi_SRCS preferences.kcfgc)

kde4_add_app_icon(kreversi_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/hi*-app-kreversi.png")
kde4_add_executable(kreversi ${kreversi_SRCS})

target_link_libraries(kreversi ${KDE4_KDEUI_LIBS} kdegames)
install(TARGETS kreversi  ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ###############

install( PROGRAMS kreversi.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
#install( FILES kreversi.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES kreversiui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kreversi )

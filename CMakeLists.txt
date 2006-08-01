project(kreversi)
add_subdirectory( pics ) 
add_subdirectory( sounds ) 
add_subdirectory( icons ) 

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames ${CMAKE_SOURCE_DIR}/libkdegames/highscore  )


########### next target ###############

set(kreversi_SRCS 
   Score.cpp 
   Move.cpp 
   Position.cpp 
   Game.cpp 
   qreversigame.cpp 
   qreversigameview.cpp 
   SuperEngine.cpp 
   Engine.cpp 
   board.cpp 
   highscores.cpp 
   kreversi.cpp 
   main.cpp )

kde4_automoc(${kreversi_SRCS})

kde4_add_ui3_files(kreversi_SRCS settings.ui )

kde4_add_kcfg_files(kreversi_SRCS prefs.kcfgc )

kde4_add_executable(kreversi ${kreversi_SRCS})

target_link_libraries(kreversi  ${KDE4_KIO_LIBS} kdegames knotifyconfig)

install(TARGETS kreversi  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES kreversi.desktop  DESTINATION  ${XDG_APPS_DIR} )
install( FILES kreversi.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES kreversiui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kreversi )


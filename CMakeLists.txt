kde4_header()
project(kreversi)
add_subdirectory( pics ) 
add_subdirectory( sounds ) 
add_subdirectory( icons ) 

include_directories( ${CMAKE_SOURCE_DIR}/libkdegames ${CMAKE_SOURCE_DIR}/libkdegames/highscore ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(kreversi_SRCS 
   kzoommainwindow.cpp 
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

target_link_libraries(kreversi  ${KDE4_KIO_LIBRARIES} kdegames )

install_targets(/bin kreversi )


########### install files ###############

install_files( ${XDG_APPS_DIR} FILES kreversi.desktop )
install_files( ${KCFG_INSTALL_DIR} FILES kreversi.kcfg )
install_files( ${DATA_INSTALL_DIR}/kreversi FILES kreversiui.rc )

kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = -I$(top_srcdir)/libkdegames -I$(top_srcdir)/libkdegames/highscore $(all_includes)
#
#bin_PROGRAMS = kreversi
#
#kreversi_SOURCES = \
#    kzoommainwindow.cpp \
#    Score.cpp \
#    Move.cpp \
#    Position.cpp \
#    Game.cpp \
#    qreversigame.cpp \
#    qreversigameview.cpp \
#    SuperEngine.cpp \
#    Engine.cpp \
#    board.cpp \
#    settings.ui \
#    highscores.cpp \
#    kreversi.cpp \
#    main.cpp \
#    prefs.kcfgc
#kreversi_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#kreversi_LDADD = $(LIB_KDEGAMES) $(LIB_KIO)
#kreversi_DEPENDENCIES = $(LIB_KDEGAMES_DEP)
#
#METASOURCES = AUTO
#
#rcdir = $(kde_datadir)/kreversi
#rc_DATA = kreversiui.rc
#
#noinst_HEADERS = \
#    kzoommainwindow.h \
#    Engine.h \
#    Game.h \
#    qreversigame.h \
#    qreversigameview.h \
#    kreversi.h \
#    Move.h \
#    board.h \
#    Position.h \
#    Score.h \
#    version.h \
#    SuperEngine.h \
#    highscores.h
#
#SUBDIRS = . pics sounds icons
#
#xdg_apps_DATA = kreversi.desktop
#kde_kcfg_DATA = kreversi.kcfg
#
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp -o $(podir)/kreversi.pot

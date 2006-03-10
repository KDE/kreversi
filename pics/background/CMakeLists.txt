kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kreversi/pics/background FILES Dark_Wood.png Granite.png Light_Wood.png Ocean.png Puzzle.png  Earth.png Hexagon.png  Mystique.png Pipes.png Stones.png )

kde4_footer()



#original Makefile.am contents follow:

#
#bg_DATA = Dark_Wood.png Granite.png Light_Wood.png Ocean.png Puzzle.png \
#Earth.png Hexagon.png  Mystique.png Pipes.png Stones.png
#
#bgdir = $(kde_datadir)/kreversi/pics/background
#
#EXTRA_DIST = $(bg_DATA)
#

kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kreversi FILES eventsrc )
install_files( ${DATA_INSTALL_DIR}/kreversi/sounds FILES reversi-click.wav reversi-won.wav )

kde4_footer()



#original Makefile.am contents follow:

#soundsdir = $(kde_datadir)/kreversi/sounds
#sounds_DATA = reversi-click.wav reversi-won.wav
#
#EXTRA_DIST = $(sounds_DATA)
#
#appdatadir = $(kde_datadir)/kreversi
#appdata_DATA = eventsrc
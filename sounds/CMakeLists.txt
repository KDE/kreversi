



########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kreversi FILES eventsrc )
install_files( ${DATA_INSTALL_DIR}/kreversi/sounds FILES reversi-click.wav reversi-won.wav )




#original Makefile.am contents follow:

#soundsdir = $(kde_datadir)/kreversi/sounds
#sounds_DATA = reversi-click.wav reversi-won.wav
#
#EXTRA_DIST = $(sounds_DATA)
#
#appdatadir = $(kde_datadir)/kreversi
#appdata_DATA = eventsrc
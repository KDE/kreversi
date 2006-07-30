



########### install files ###############

install( FILES kreversi.notifyrc  DESTINATION  ${DATA_INSTALL_DIR}/kreversi )
install( FILES reversi-click.wav reversi-won.wav  DESTINATION  ${DATA_INSTALL_DIR}/kreversi/sounds )




#original Makefile.am contents follow:

#soundsdir = $(kde_datadir)/kreversi/sounds
#sounds_DATA = reversi-click.wav reversi-won.wav
#
#EXTRA_DIST = $(sounds_DATA)
#
#appdatadir = $(kde_datadir)/kreversi
#appdata_DATA = eventsrc

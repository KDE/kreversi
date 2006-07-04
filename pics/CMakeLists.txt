
add_subdirectory( background ) 




########### install files ###############

install( FILES chips.png chips_mono.png  DESTINATION  ${DATA_INSTALL_DIR}/kreversi/pics )




#original Makefile.am contents follow:

#pics_DATA = chips.png chips_mono.png
#
#picsdir = $(kde_datadir)/kreversi/pics
#
#SUBDIRS = background
#
#EXTRA_DIST = $(pics_DATA)
#

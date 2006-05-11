kde4_header()

add_subdirectory( background ) 




########### install files ###############

install_files( ${DATA_INSTALL_DIR}/kreversi/pics FILES chips.png chips_mono.png )

kde4_footer()



#original Makefile.am contents follow:

#pics_DATA = chips.png chips_mono.png
#
#picsdir = $(kde_datadir)/kreversi/pics
#
#SUBDIRS = background
#
#EXTRA_DIST = $(pics_DATA)
#

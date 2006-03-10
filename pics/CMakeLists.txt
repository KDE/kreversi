kde4_header()

add_subdirectory( background ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


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

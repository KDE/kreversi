
INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD =   -lkimgio $(LIB_KDEUI) 

bin_PROGRAMS = kreversi

kreversi_SOURCES = main.cpp misc.cpp app.cpp board.cpp Game.cpp Score.cpp SuperEngine.cpp \
		Engine.cpp Position.cpp Move.cpp about.cpp playsound.cpp
METASOURCES =	app.moc board.moc about.moc

noinst_HEADERS = Engine.h about.h Game.h app.h Move.h board.h Position.h \
	misc.h playsound.h Score.h version.h SuperEngine.h           

SUBDIRS = . pics sounds icons

applnk_DATA = kreversi.desktop
applnkdir   = $(kde_appsdir)/Games

EXTRA_DIST = $(applnk_DATA)

messages:	
	$(XGETTEXT) $(kreversi_SOURCES) -o $(podir)/kreversi.pot



INCLUDES = -I$(top_srcdir)/libkdegames -I$(top_srcdir)/libkdegames/highscore $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD =   $(LIB_KDEGAMES) $(LIB_KSYCOCA) $(LIB_KDEUI) 

bin_PROGRAMS = kreversi

kreversi_SOURCES = main.cpp kreversi.cpp board.cpp Game.cpp \
		Score.cpp SuperEngine.cpp settings.ui \
		Engine.cpp Position.cpp Move.cpp playsound.cpp

METASOURCES =	AUTO

rcdir = $(kde_datadir)/kreversi
rc_DATA = kreversiui.rc

noinst_HEADERS = Engine.h Game.h kreversi.h Move.h board.h Position.h \
	playsound.h Score.h version.h SuperEngine.h           

SUBDIRS = . pics sounds icons

xdg_apps_DATA = kreversi.desktop

EXTRA_DIST = $(xdg_apps_DATA)

messages:	
	$(XGETTEXT) $(kreversi_SOURCES) -o $(podir)/kreversi.pot

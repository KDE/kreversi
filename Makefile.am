
INCLUDES = -I$(top_srcdir)/libkdegames -I$(top_srcdir)/libkdegames/highscore $(all_includes)

bin_PROGRAMS = kreversi

kreversi_SOURCES = main.cpp kreversi.cpp board.cpp Game.cpp \
		Score.cpp SuperEngine.cpp settings.ui \
		Engine.cpp Position.cpp Move.cpp prefs.kcfgc
kreversi_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kreversi_LDADD = $(LIB_KDEGAMES) $(LIB_KIO)
kreversi_DEPENDENCIES = $(LIB_KDEGAMES_DEP)

METASOURCES =	AUTO

rcdir = $(kde_datadir)/kreversi
rc_DATA = kreversiui.rc

noinst_HEADERS = Engine.h Game.h kreversi.h Move.h board.h Position.h \
	Score.h version.h SuperEngine.h           

SUBDIRS = . pics sounds icons

xdg_apps_DATA = kreversi.desktop
kde_kcfg_DATA = kreversi.kcfg


messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kreversi.pot

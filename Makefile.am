INCLUDES = -I$(top_srcdir)/libkdegames -I$(top_srcdir)/libkdegames/highscore $(all_includes)

bin_PROGRAMS = kreversi

kreversi_SOURCES = kzoommainwindow.cpp Score.cpp Move.cpp Position.cpp Game.cpp SuperEngine.cpp Engine.cpp board.cpp settings.ui highscores.cpp kreversi.cpp main.cpp prefs.kcfgc
kreversi_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kreversi_LDADD = $(LIB_KDEGAMES) $(LIB_KIO)

METASOURCES = AUTO

rcdir = $(kde_datadir)/kreversi
rc_DATA = kreversiui.rc

noinst_HEADERS = kzoommainwindow.h Engine.h Game.h kreversi.h Move.h board.h Position.h Score.h version.h SuperEngine.h highscores.h

SUBDIRS = . pics sounds icons

xdg_apps_DATA = kreversi.desktop
kde_kcfg_DATA = kreversi.kcfg


messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kreversi.pot


INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD =   $(LIB_KSYCOCA) $(LIB_KDEUI) 

bin_PROGRAMS = kreversi

kreversi_SOURCES = main.cpp misc.cpp app.cpp board.cpp Game.cpp Score.cpp SuperEngine.cpp \
		Engine.cpp Position.cpp Move.cpp playsound.cpp
METASOURCES =	app.moc board.moc

noinst_HEADERS = Engine.h Game.h app.h Move.h board.h Position.h \
	misc.h playsound.h Score.h version.h SuperEngine.h           

SUBDIRS = . pics sounds icons

xdg_apps_DATA = kreversi.desktop

EXTRA_DIST = $(xdg_apps_DATA)

messages:	
	$(XGETTEXT) $(kreversi_SOURCES) -o $(podir)/kreversi.pot

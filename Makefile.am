
INCLUDES = $(all_includes)

bin_PROGRAMS = kreversi

kreversi_SOURCES = main.cpp misc.cpp app.cpp board.cpp Game.cpp Score.cpp SuperEngine.cpp \
		Engine.cpp Position.cpp Move.cpp playsound.cpp

kreversi_LDFLAGS =  $(all_libraries) $(KDE_RPATH)
kreversi_LDADD =  $(LIB_KSYCOCA) $(LIB_KDEUI)

METASOURCES =	app.moc board.moc

noinst_HEADERS = Engine.h Game.h app.h Move.h board.h Position.h \
	misc.h playsound.h Score.h version.h SuperEngine.h           

SUBDIRS = . pics sounds icons

applnk_DATA = kreversi.desktop
applnkdir   = $(kde_appsdir)/Games/Board

EXTRA_DIST = $(applnk_DATA)

messages:	
	$(XGETTEXT) $(kreversi_SOURCES) -o $(podir)/kreversi.pot

install-data-local:
	rm -rf $(DESTDIR)$(kde_appsdir)/Games/kreversi.desktop

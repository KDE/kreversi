
INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD =   -lmediatool     $(LIB_KDEUI) $(LIBCOMPAT)

bin_PROGRAMS = kreversi

kreversi_SOURCES = main.cpp misc.cpp app.cpp board.cpp Game.cpp Score.cpp SuperEngine.cpp \
		Engine.cpp Position.cpp Move.cpp Random.cpp about.cpp playsound.cpp
METASOURCES =	app.moc board.moc about.moc

noinst_HEADERS = Engine.h about.h Game.h app.h Move.h board.h Position.h \
	misc.h Random.h playsound.h Score.h version.h SuperEngine.h           

SUBDIRS = html pics sounds icons

applnk_DATA = kreversi.kdelnk
applnkdir = $(kde_appsdir)/Games

EXTRA_DIST = $(applnk_DATA)

messages:	
	$(XGETTEXT) -C -ktranslate -ki18n -x $(includedir)/kde.pot $(kreversi_SOURCES) && mv messages.po ../po/kreversi.pot


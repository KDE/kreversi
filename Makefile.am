
INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = -lkdeui -lkdecore -lqt -lXext -lX11 -lmediatool $(LIBSOCKET) $(LIBCOMPAT)

bin_PROGRAMS = kreversi

kreversi_SOURCES = main.cpp misc.cpp app.cpp board.cpp Game.cpp Score.cpp SuperEngine.cpp \
		Engine.cpp Position.cpp Move.cpp Random.cpp about.cpp playsound.cpp
BUILTSOURCES =	app.moc board.moc about.moc

DISTCLEANFILES = $(BUILTSOURCES)

SUBDIRS = html pics sounds icons

applnk_DATA = kreversi.kdelnk
applnkdir = $(kde_appsdir)/Games

messages:	
	@XGETTEXT@ -C -ktranslate -x../../kdelibs/po/kde.pot *.cpp && mv messages.po ../po/kreversi.pot

# Dependencies
about.cpp:about.moc
about.moc: about.h
	$(MOC) about.h -o about.moc

app.cpp:app.moc
app.moc: app.h
	$(MOC) app.h -o app.moc

board.cpp:board.moc
board.moc: board.h
	$(MOC) board.h -o board.moc



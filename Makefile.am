
INCLUDES = $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
LDADD = -lkdeui -lkdecore -lqt -lXext -lX11 -lmediatool $(LIBSOCKET)

bin_PROGRAMS = kreversi

########### File Formats ###########

SUFFIXES = .moc

.h.moc:
	$(MOC) $< -o $@

########## Files #####################

kreversi_SOURCES = main.cpp misc.cpp app.cpp board.cpp Game.cpp Score.cpp SuperEngine.cpp \
		Engine.cpp Position.cpp Move.cpp Random.cpp about.cpp playsound.cpp
BUILTSOURCES =	app.moc board.moc about.moc

DISTCLEANFILES = $(BUILTSOURCES)

SUBDIRS = html pics sounds

applnk_DATA = Reversi.kdelnk
applnkdir = $(kde_appsdir)/Games

messages:	
	@XGETTEXT@ -C -ktranslate -x../../kdelibs/po/kde.pot *.cpp && mv messages.po ../po/kreversi.pot

# Dependencies
about.cpp: about.moc
app.cpp: app.moc
board.cpp: board.moc

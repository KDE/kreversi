INCLUDES = -I$(top_srcdir)/libkdegames -I$(top_srcdir)/libkdegames/highscore $(all_includes)

bin_PROGRAMS = kreversi

kreversi_SOURCES = \
    kzoommainwindow.cpp \
    Score.cpp \
    Move.cpp \
    Position.cpp \
    Game.cpp \
    qreversigame.cpp \
    qreversigameview.cpp \
    SuperEngine.cpp \
    Engine.cpp \
    board.cpp \
    settings.ui \
    highscores.cpp \
    kreversi.cpp \
    main.cpp \
    prefs.kcfgc
kreversi_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kreversi_LDADD = $(LIB_KDEGAMES) $(LIB_KIO)
kreversi_DEPENDENCIES = $(LIB_KDEGAMES_DEP)

METASOURCES = AUTO

rcdir = $(kde_datadir)/kreversi
rc_DATA = kreversiui.rc

noinst_HEADERS = \
    kzoommainwindow.h \
    Engine.h \
    Game.h \
    qreversigame.h \
    qreversigameview.h \
    kreversi.h \
    Move.h \
    board.h \
    Position.h \
    Score.h \
    version.h \
    SuperEngine.h \
    highscores.h

SUBDIRS = . pics sounds icons

xdg_apps_DATA = kreversi.desktop
kde_kcfg_DATA = kreversi.kcfg


messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kreversi.pot

# for system-wide highscore file
DESTBIN = $(DESTDIR)$(bindir)/$(bin_PROGRAMS)
DESTHIGHSCORES = $(DESTDIR)$(HIGHSCORE_DIRECTORY)
DESTSCORES = $(DESTDIR)$(HIGHSCORE_DIRECTORY)/$(bin_PROGRAMS).scores

install-data-local:
	@if test x$(HIGHSCORE_DIRECTORY) != x; then \
	  echo "********************************************************" ;\
	  echo "" ;\
	  echo "This game is installed sgid \"games\" to use the" ;\
	  echo "system-wide highscore file (in "$(HIGHSCORE_DIRECTORY)")." ;\
	  echo "" ;\
	  echo "If the system-wide highscore file does not exist, it is" ;\
          echo "created with the correct ownership and permissions. See the" ;\
          echo "INSTALL file in \"kdegames/libkdegames/highscore\" for details." ;\
	  echo "" ;\
	  echo "********************************************************" ;\
	fi 

install-exec-hook:
	@if test x$(HIGHSCORE_DIRECTORY) != x; then \
	  chown $(highscore_user):$(highscore_group) $(DESTBIN) \
	  || echo "Error: Could not install the game with correct permissions !!" ;\
	fi

	@if test x$(HIGHSCORE_DIRECTORY) != x; then \
		mkdir -p $(DESTHIGHSCORES) && \
		chown $(highscore_user):$(highscore_group) $(DESTHIGHSCORES) \
		&& chmod 750 $(DESTHIGHSCORES) \
	        || echo "Error: Could not create the highscore directory with correct permissions !!" ;\
	fi

	@if test x$(HIGHSCORE_DIRECTORY) != x; then \
	   chown $(highscore_user):$(highscore_group) $(DESTBIN) \
	   || echo "Error: Could not install the game with correct permissions !!" ;\
	fi

	@if test ${setgid} = true; then \
	   chmod 2755  $(DESTBIN) \
	   || echo "Error: Could not install the game with correct permissions !!" ;\
	fi

	@if test x$(HIGHSCORE_DIRECTORY) != x; then \
	   touch $(DESTSCORES) && chown $(highscore_user):$(highscore_group) $(DESTSCORES) \
	   && chmod 0660 $(DESTSCORES) \
	   || echo "Error: Could not create system-wide highscore file with correct permissions !!" ;\
	fi


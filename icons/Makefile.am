
kde_icon_DATA = kreversi.xpm

EXTRA_DIST = $(kde_icon_DATA) mini-kreversi.xpm

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) $(srcdir)/mini-kreversi.xpm $(kde_minidir)/kreversi.xpm
	$(mkinstalldirs) $(kde_minidir)/../large
	$(INSTALL_DATA) $(srcdir)/large-kreversi.xpm $(kde_minidir)/../large/kreversi.xpm

uninstall-local:
	rm -f $(kde_minidir)/kreversi.xpm
	rm -f $(kde_minidir)/../large/kreversi.xpm


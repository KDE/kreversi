
icon_DATA = kreversi.xpm

icondir = $(kde_icondir)

EXTRA_DIST = $(icon_DATA) mini-kreversi.xpm

install-data-local:
	$(mkinstalldirs) $(kde_minidir)
	$(INSTALL_DATA) mini-kreversi.xpm $(kde_minidir)/kreversi.xpm

uninstall-local:
	rm -f $(kde_minidir)/kreversi.xpm


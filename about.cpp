/* Yo Emacs, this -*- C++ -*-
 *******************************************************************
 *******************************************************************
 *
 *
 * KREVERSI
 *
 *
 *******************************************************************
 *
 * A Reversi (or sometimes called Othello) game
 *
 *******************************************************************
 *
 * created 1997 by Mario Weilguni <mweilguni@sime.com>
 *
 *******************************************************************
 *
 * This file is part of the KDE project "KREVERSI"
 *
 * KREVERSI is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * KREVERSI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KREVERSI; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *******************************************************************
 */

#include "about.h"
#include <qpushbt.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <klocale.h>
#include <kapp.h>

extern QString PICDIR;

About::About(QWidget *parent) : QDialog(parent, "About KReversi", TRUE) {
  KLocale *locale = kapp->getLocale();
  resize(450, 400);
  setFixedSize(size());
  setCaption(locale->translate("About KReversi"));

  QPixmap pm((PICDIR + "logo.xpm").data());
  QLabel *logo = new QLabel(this);
  logo->setPixmap(pm);
  logo->setGeometry(20, (height()-pm.height())/2, pm.width(), pm.height());

  QLabel *l;
  l = new QLabel("KReversi", this);
  l->setFont(QFont("Times", 25, QFont::Bold));
  l->setGeometry((width() - l->sizeHint().width())/2, 20,
		 l->sizeHint().width(), 
		 l->sizeHint().height());

  QString s;
  s = locale->translate("Version ");
  s += VERSION_STR;
  s += locale->translate("\n(c) 1997 Mario Weilguni <mweilguni@sime.com>\n\n" \
    "This program is free software published under the GNU General " \
    "Public License (take a look into help for details)\n\n" \
    "Thanks to:\n" \
    "\tMats Luthman for the game engine " \
    "(I've ported it from his JAVA applet)\n\n" \
    "\tStephan Kulow <coolo@itm.mu-luebeck.de> for comments "\
    "and bugfixes\n\n" \
    "\tArne Klaassen <klaassen@informatik.uni-rostock.de> "\
    "for the raytraced chips");
  l = new QLabel(s.data(), this);
  l->setGeometry(150, 70, 290, 260);
  l->setAlignment(AlignLeft|WordBreak|ExpandTabs);

  QPushButton *b_ok = new QPushButton(locale->translate("Close"), this);
  b_ok->setGeometry(width()/2-40, height() - 48, 80, 32);
  b_ok->setDefault(TRUE);
  b_ok->setAutoDefault(TRUE);
  connect(b_ok, SIGNAL(released()),
	  this, SLOT(accept()));
  b_ok->setFocus();
}

#include "about.moc"

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
#include "version.h"
#include <qpushbt.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <klocale.h>
#include <kapp.h>
#include <qlayout.h>

extern QString PICDIR;

About::About(QWidget *parent) : QDialog(parent, "About kreversi", TRUE) {
  KLocale *locale = kapp->getLocale();
  setCaption(locale->translate("About kreversi"));

  QPixmap pm((PICDIR + "logo.xpm").data());
  QLabel *logo = new QLabel(this);
  logo->setPixmap(pm);
  logo->setFixedSize(logo->sizeHint());  

  QLabel *l;
  l = new QLabel("kreversi", this);
  l->setFont(QFont("Times", 25, QFont::Bold));
  l->setFixedSize(l->sizeHint());

  QString s;
  s = locale->translate("Version ");
  s += KREVERSI_VERSION;
  s += locale->translate("\n(c) 1997 Mario Weilguni <mweilguni@sime.com>\n\n" \
    "This program is free software\npublished under the GNU General\n" \
    "Public License (take a look\ninto help for details)\n\n" \
    "Thanks to:\n" \
    "\tMats Luthman for the game engine\n" \
    "\t(I've ported it from his JAVA applet)\n\n" \
    "\tStephan Kulow\n\tfor comments "\
    "and bugfixes\n\n" \
    "\tArne Klaassen\n\t "\
    "for the raytraced chips");
  QLabel *l1 = new QLabel(s.data(), this);
  l1->setAlignment(AlignLeft|ExpandTabs);
  l1->setFixedSize(l1->sizeHint());
  l1->show();

  QPushButton *b_ok = new QPushButton(locale->translate("Close"), this);
  b_ok->setDefault(TRUE);
  if(style() == MotifStyle) 
    b_ok->setFixedSize(b_ok->sizeHint().width() + 10, // for 
		       b_ok->sizeHint().height() + 10);
  else
    b_ok->setFixedSize(b_ok->sizeHint());
  connect(b_ok, SIGNAL(clicked()),
	  this, SLOT(accept()));
  b_ok->setFocus();

  // create layout
  QVBoxLayout *tl = new QVBoxLayout(this, 10);
  tl->addWidget(l, 0);
  QHBoxLayout *tl1 = new QHBoxLayout(0);
  tl->addLayout(tl1);
  tl1->addWidget(logo);
  tl1->addSpacing(15);
  tl1->addWidget(l1);
  tl->addSpacing(10);
  tl->addWidget(b_ok);
  tl->activate();
  tl->freeze();
}

#include "about.moc"

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


#include "board.h"

#include <unistd.h>

#include <qpainter.h>

#include <kapplication.h>
#include <kstandarddirs.h>
#include <kconfig.h>
#include <knotifyclient.h>
#include <klocale.h>
#include <kexthighscore.h>
#include <kdebug.h>

#include "prefs.h"
#include "Engine.h"


#ifndef PICDATA
#define PICDATA(x) KGlobal::dirs()->findResource("appdata", QString("pics/")+ x)
#endif

const uint  HINT_BLINKRATE        = 250000;
const uint  ANIMATION_DELAY       = 3000;
const uint  CHIP_OFFSET[NbColors] = { 24, 1 };
const uint  CHIP_SIZE             = 36;


//-----------------------------------------------------------------


Board::Board(QWidget *parent, Game *game)
    : QWidget(parent, "board"),
      chiptype(Unloaded)
{
  m_game = game;
}


Board::~Board()
{
}


// ----------------------------------------------------------------


// Start it all up.
//

void Board::start()
{
#if 0
  // make sure a signal is emitted
  // FIXME: why?  /iw
  setStrength(strength());
#endif
  updateBoard(TRUE);
  adjustSize();
}


void Board::loadChips(ChipType type)
{
  QString  name("pics/");
  name += (type==Colored ? "chips.png" : "chips_mono.png");

  QString  s  = KGlobal::dirs()->findResource("appdata", name);
  bool     ok = allchips.load(s);

  Q_ASSERT( ok && allchips.width()==CHIP_SIZE*5
            && allchips.height()==CHIP_SIZE*5 );
  chiptype = type;
  update();
}


// Negative speed is allowed.  If speed is negative,
// no animations are displayed.
//

void Board::setAnimationSpeed(uint speed)
{
  if (speed <= 10)
    anim_speed = speed;
}


// Handle mouse clicks.
//

void Board::mousePressEvent(QMouseEvent *e)
{
  // Only handle left button.  No context menu.
  if ( e->button() != LeftButton ) {
    e->ignore();
    return;
  }

  int px = (e->pos().x()-1) / zoomedSize();
  int py = (e->pos().y()-1) / zoomedSize();
  emit signalSquareClicked(py, px);
}


// Calculate the final score.
//
// FIXME: Move this to kreversi.h?  Or to KReversiGame.

void Board::gameEnded()
{
  uint  black = m_game->score(Black);
  uint  white = m_game->score(White);

  if (black > white)
    emit gameWon(Black);
  else if (black < white)
    emit gameWon(White);
  else
    emit gameWon(Nobody);

  emit turn(Nobody);
}


void Board::showHint(Move move)
{
  // Only show a hint if there is a move to show.
  if (move.x() == -1)
    return;

  // Blink with a piece at the location where the hint move points.
  //
  // The isVisible condition has been added so that when the player
  // was viewing a hint and quits the game window, the game doesn't
  // still have to do all this looping and directly ends.
  m_hintShowing = true;
  for (int flash = 0;
       flash < 100 && m_hintShowing && isVisible(); 
       flash++)
  {
    if (flash & 1)
      drawPiece(move.y() - 1, move.x() - 1, Nobody);
    else
      drawPiece(move.y() - 1, move.x() - 1, m_game->toMove());

    // keep GUI alive while waiting
    for (int dummy = 0; dummy < 5; dummy++) {
      usleep(HINT_BLINKRATE / 5);
      qApp->processEvents();
    }
  }

  // Draw the empty square again.
  drawPiece(move.y() - 1, move.x() - 1, m_game->color(move.x(), move.y()));
}


// Set the member m_hintShowing to false.  This will make showHint()
// end, if it is running.
//

void Board::quitHint()
{
  m_hintShowing = false;
}


// ================================================================
//            Functions related to drawing/painting


// Flash all pieces which are turned.
//
// NOTE: This code is quite a hack.  Should make it better.
//

void Board::animateChanged(Move move)
{
  if (anim_speed == 0)
    return;

  // Draw the new piece.
  drawPiece(move.y() - 1, move.x() - 1, move.color());

  // Animate row by row in all directions.
  for (int dx = -1; dx < 2; dx++)
    for (int dy = -1; dy < 2; dy++)
      if ((dx != 0) || (dy != 0))
	animateChangedRow(move.y() - 1, move.x() - 1, dy, dx);
}


bool Board::isField(int row, int col) const
{
  return ((0 <= row) && (row < 8) && (0 <= col) && (col < 8));
}


void Board::animateChangedRow(int row, int col, int dy, int dx)
{
  row = row + dy;
  col = col + dx;
  while (isField(row, col)) {
    if (m_game->wasTurned(col+1, row+1)) {
      KNotifyClient::event(winId(), "click", i18n("Click"));
      rotateChip(row, col);
   } else
      return;

    col += dx;
    row += dy;
  }
}


void Board::rotateChip(uint row, uint col)
{
  // Check which direction the chip has to be rotated.  If the new
  // chip is white, the chip was black first, so lets begin at index
  // 1, otherwise it was white.
  Color  color = m_game->color(col+1, row+1);
  uint   from  = CHIP_OFFSET[opponent(color)];
  uint   end   = CHIP_OFFSET[color];
  int    delta = (color==White ? 1 : -1);

  from += delta;
  end -= delta;

  for (uint i = from; i != end; i += delta) {
    drawOnePiece(row, col, i);
    kapp->flushX(); // FIXME: use QCanvas to avoid flicker...
    usleep(ANIMATION_DELAY * anim_speed);
  }
}


// Redraw the board.  If 'force' is true, redraw everything, otherwise
// only redraw those squares that have changed (marked by
// m_game->squareModified(col, row)).
//

void Board::updateBoard(bool force)
{
  // Draw the squares of the board.
  for (uint row = 0; row < 8; row++)
    for (uint col = 0; col < 8; col++)
      if ( force || m_game->squareModified(col + 1, row + 1) ) {
	Color  color = m_game->color(col + 1, row + 1);
	drawPiece(row, col, color);
      }

  // Draw a border around the board.
  QPainter  p(this);
  p.setPen(black);
  p.drawRect(0, 0, 8 * zoomedSize() + 2, 8 * zoomedSize() + 2);

  // FIXME: It can't be right to have this signal here.
  emit score();
}


QPixmap Board::chipPixmap(Color color, uint size) const
{
  return chipPixmap(CHIP_OFFSET[color], size);
}


// Get a pixmap for the chip 'i' at size 'size'.
//

QPixmap Board::chipPixmap(uint i, uint size) const
{
  // Get the part of the 'allchips' pixmap that contains exactly that
  // chip that we want to use.
  QPixmap  pix(CHIP_SIZE, CHIP_SIZE);
  copyBlt(&pix, 0, 0, &allchips, (i%5) * CHIP_SIZE, (i/5) * CHIP_SIZE,
          CHIP_SIZE, CHIP_SIZE);

  // Resize (scale) the pixmap to the desired size.
  QWMatrix  wm3;
  wm3.scale(float(size)/CHIP_SIZE, float(size)/CHIP_SIZE);

  return pix.xForm(wm3);
}


uint Board::zoomedSize() const
{
  return qRound(float(CHIP_SIZE) * Prefs::zoom() / 100);
}


void Board::drawPiece(uint row, uint col, Color color)
{
  int  i = (color == Nobody ? -1 : int(CHIP_OFFSET[color]));
  drawOnePiece(row, col, i);
}


void Board::drawOnePiece(uint row, uint col, int i)
{
  int       px = col * zoomedSize() + 1;
  int       py = row * zoomedSize() + 1;
  QPainter  p(this);

  // Draw either a background pixmap or a background color to the square.
  if (bg.width())
    p.drawTiledPixmap(px, py, zoomedSize(), zoomedSize(), bg, px, py);
  else
    p.fillRect(px, py, zoomedSize(), zoomedSize(), bgColor);

  // Draw a black border around the square.
  p.setPen(black);
  p.drawRect(px, py, zoomedSize(), zoomedSize());

  // If no piece on the square, i.e. only the background, then return here...
  if ( i == -1 )
    return;

  // ...otherwise finally draw the piece on the square.
  p.drawPixmap(px, py, chipPixmap(i, zoomedSize()));
}


// We got a repaint event.  We make it easy for us and redraw the
// entire board.
//

void Board::paintEvent(QPaintEvent *)
{
  updateBoard(true);
}


void Board::adjustSize()
{
  int w = 8 * zoomedSize();
  setFixedSize(w + 2, w + 2);
}


void Board::setPixmap(QPixmap &pm)
{
  if ( pm.width() == 0 ) 
    return;

  bg = pm;
  update();
  setErasePixmap(pm);
}


void Board::setColor(const QColor &c)
{
  bgColor = c;
  bg = QPixmap();
  update();
  setEraseColor(c);
}


void Board::loadSettings()
{
  if ( Prefs::grayscale() ) {
    if (chiptype != Grayscale)
      loadChips(Grayscale);
  }
  else {
    if (chiptype != Colored)
      loadChips(Colored);
  }

  if ( !Prefs::animation() )
    setAnimationSpeed(0);
  else
    setAnimationSpeed(10 - Prefs::animationSpeed());

  if ( Prefs::backgroundImageChoice() ) {
    QPixmap pm( Prefs::backgroundImage() );
    if (!pm.isNull())
      setPixmap(pm);
  } else {
    setColor( Prefs::backgroundColor() );
  }

#if 0
  // This should be changed...
  setColor(paletteBackgroundColor());
  if (config->readNumEntry("Background", -1) != -1) {
    int i = config->readNumEntry("Background");
    if (i == 1) {
      QColor s = config->readColorEntry("BackgroundColor");
      setColor(s);
    } else if (i == 2) {
      QString s = locate("appdata", config->readEntry("BackgroundPixmap"));
      if (!s.isEmpty()) {
	QPixmap bg(s);
	if (bg.width())
	  setPixmap(bg);
      }
    }
  }
  else {
    QPixmap bg(locate("appdata", "pics/background/Light_Wood.png"));
    if (bg.width())
      setPixmap(bg);
  }
#endif
}

#include "board.moc"


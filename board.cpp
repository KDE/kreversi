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

#include "prefs.h"
#include "Engine.h"

#define PICDATA(x) KGlobal::dirs()->findResource("appdata", QString("pics/")+ x)

const uint HINT_BLINKRATE = 250000;
const uint ANIMATION_DELAY = 3000;
const uint CHIP_OFFSET[NbColors] = { 24, 1 };
const uint CHIP_SIZE       = 36;

//-----------------------------------------------------------------------------
Board::Board(QWidget *parent)
    : QWidget(parent, "board"),
  human(Black), nopaint(false), chiptype(Unloaded)
{
  engine = new Engine();
  game = new Game();
  setStrength(1);
}

Board::~Board() {
  delete engine;
  delete game;
}

uint Board::zoomedSize() const {
  return qRound(float(CHIP_SIZE) * Prefs::zoom() / 100);
}

void Board::start() {
  // make sure a signal is emitted
  setStrength(strength());
  newGame();
  adjustSize();
}

void Board::loadChips(ChipType type) {
  QString name("pics/");
  name += (type==Colored ? "chips.png" : "chips_mono.png");
  QString s = KGlobal::dirs()->findResource("appdata", name);
  bool ok = allchips.load(s);
  Q_ASSERT( ok && allchips.width()==CHIP_SIZE*5
            && allchips.height()==CHIP_SIZE*5 );
  chiptype = type;
  update();
}

// negative speed is allowed. if speed is negative,
// no animations are displayed
void Board::setAnimationSpeed(uint speed) {
  if(speed <= 10)
    anim_speed = speed;
}

/// takes back last set of moves
void Board::undo() {
  if(state() == Ready) {
    Color last_color = game->lastMove().color();
    while ((game->moveNumber() != 0) &&
           (last_color == game->lastMove().color()))
       game->TakeBackMove();
    game->TakeBackMove();
    update();
  }
}


/// interrupt thinking of game engine
void Board::interrupt() {
  engine->setInterrupt(TRUE);
}

bool Board::interrupted() const {
  return ((game->toMove() == computerColor()) && (state() == Ready));
}


/// continues a move if it was prior interrupted
void Board::doContinue() {
  if(interrupted())
    computerMakeMove();
}

/// starts a new game
void Board::newGame() {
  game->Reset();
  updateBoard(TRUE);
  setState(Ready);

  emit turn(Black);

  // computer makes first move
  if(human == White)
    computerMakeMove();
}

/// handles mouse clicks
void Board::mousePressEvent(QMouseEvent *e) {
  if ( e->button()!=LeftButton ) {
    e->ignore();
    return;
  }

  if ( interrupted() ) {
    illegalMove();
    return;
  }
  if(state() == Ready) {
    int px = (e->pos().x()-1) / zoomedSize();
    int py = (e->pos().y()-1) / zoomedSize();
    fieldClicked(py, px);
  } else if(state() == Hint)
    setState(Ready);
  else illegalMove();
}

void Board::illegalMove()
{
  KNotifyClient::event(winId(), "illegal_move", i18n("Illegal move"));
}

/// handles piece settings
void Board::fieldClicked(int row, int col) {
  if(state() == Ready) {
    Color color = game->toMove();

    /// makes a human move
    Move m(col + 1, row + 1, color);
    if(game->moveIsLegal(m)) {
      game->MakeMove(m);
      animateChanged(m);

      if(!game->moveIsAtAllPossible()) {
	updateBoard();
	setState(Ready);
	gameEnded();
	return;
      }

      updateBoard();

      if(color != game->toMove())
	computerMakeMove();
    } else
      illegalMove();
  }
}


/// makes a computer move
void Board::computerMakeMove() {
  // check if the computer can move
  Color color = game->toMove();
  Color opponent = ::opponent(color);

  emit turn(color);

  if(game->moveIsPossible(color)) {
    setState(Thinking);
    do {
      Move m;

      if(!game->moveIsAtAllPossible()) {
	setState(Ready);
	gameEnded();
	return;
      }

      m = engine->computeMove(*game);
      if(m.x() == -1) {
	setState(Ready);
	return;
      }
      usleep(300000); // Pretend we have to think hard.

      //playSound("click.wav");
      game->MakeMove(m);
      animateChanged(m);
      updateBoard();
    } while(!game->moveIsPossible(opponent));


    emit turn(opponent);
    setState(Ready);

    if(!game->moveIsAtAllPossible()) {
      gameEnded();
      return;
    }
  }
}


/// calculate final score
void Board::gameEnded() {
  uint b = score(Black);
  uint w = score(White);
  if(b > w)
    emit gameWon(Black);
  else if(b < w)
    emit gameWon(White);
  else
    emit gameWon(Nobody);
  emit turn(Nobody);
}


void Board::switchSides() {
  if(state() == Ready) {
    human = opponent(human);
    emit score();
    kapp->processEvents();
    computerMakeMove();
  }
}

void Board::setState(State nstatus) {
  _status = nstatus;
  emit statusChange(_status);
}

void Board::setStrength(uint st) {
  Q_ASSERT( st>=1 && st<=7 );
  st = QMAX(QMIN(st, 7), 1);
  engine->setStrength(st);
  KExtHighscore::setGameType(st-1);
}

uint Board::strength() const {
  return engine->strength();
}

uint Board::moveNumber() const {
  return game->moveNumber();
}

uint Board::score(Color color) const {
  return game->score(color);
}

Color Board::whoseTurn() const {
  return game->toMove();
}

void Board::hint() {
  if(state() == Ready) {
    setState(Thinking);
    Move m = engine->computeMove(*game);
    setState(Hint);
    if(m.x() != -1) {
      // the isVisible condition has been added so that when the player was viewing
      // a hint and quits the game window, the game doesn't still have to do all this looping
      // and directly ends
      for(int flash = 0; (flash < 100) && (state() != Ready) && isVisible(); flash++) {
	if(flash & 1)
	  drawPiece(m.y() - 1, m.x() - 1, Nobody);
	else
	  drawPiece(m.y() - 1, m.x() - 1, game->toMove());

	// keep GUI alive while waiting
	for(int dummy = 0; dummy < 5; dummy++) {
	  usleep(HINT_BLINKRATE/5);
	  qApp->processEvents();
	}
      }
      drawPiece(m.y() - 1, m.x() - 1, game->color(m.x(), m.y()));
    }
    setState(Ready);
  }
}

// ********************************************************
// ********************************************************
//
// functions related to drawing/painting
//
// ********************************************************
// ********************************************************

// flash all pieces which are turned
// NOTE: this code is quite a hack. Should make it better
void Board::animateChanged(Move m) {
  if(anim_speed == 0)
    return;

  // draw the new piece
  drawPiece(m.y()-1, m.x()-1, m.color());

  for(int dx = -1; dx < 2; dx++)
    for(int dy = -1; dy < 2; dy++)
      if((dx != 0) || (dy != 0))
	animateChangedRow(m.y()-1, m.x()-1, dy, dx);
}


bool Board::isField(int row, int col) const {
  return ((row > -1) && (row < 8) && (col > -1) && (col < 8));
}


void Board::animateChangedRow(int row, int col, int dy, int dx) {
  row = row + dy;
  col = col + dx;
  while(isField(row, col)) {
    if(game->wasTurned(col+1, row+1)) {
      KNotifyClient::event(winId(), "click", i18n("Click"));
      rotateChip(row, col);
   } else
      return;

    col += dx;
    row += dy;
  }
}

void Board::rotateChip(uint row, uint col) {
  // check which direction the chip has to be rotated
  // if the new chip is white, the chip was black first,
  // so lets begin at index 1, otherwise it was white
  Color color = game->color(col+1, row+1);
  uint from = CHIP_OFFSET[opponent(color)];
  uint end = CHIP_OFFSET[color];
  int delta = (color==White ? 1 : -1);
  from += delta;
  end -= delta;

  for(uint i = from; i != end; i += delta) {
    drawOnePiece(row, col, i);
    kapp->flushX(); // use QCanvas to avoid flicker...
    usleep(ANIMATION_DELAY * anim_speed);
  }
}


void Board::updateBoard(bool force) {
  for(uint row = 0; row < 8; row++)
    for(uint col = 0; col < 8; col++)
        if ( force || game->squareModified(col+1, row+1) ) {
            Color color = game->color(col + 1, row + 1);
            drawPiece(row, col, color);
        }
  QPainter p(this);
  p.setPen(black);
  p.drawRect(0, 0, 8 * zoomedSize() + 2, 8 * zoomedSize() + 2);

  emit score();
}

QPixmap Board::chipPixmap(Color color, uint size) const
{
  return chipPixmap(CHIP_OFFSET[color], size);
}

QPixmap Board::chipPixmap(uint i, uint size) const
{
  QPixmap pix(CHIP_SIZE, CHIP_SIZE);
  copyBlt(&pix, 0, 0, &allchips, (i%5) * CHIP_SIZE, (i/5) * CHIP_SIZE,
          CHIP_SIZE, CHIP_SIZE);
  QWMatrix wm3;
  wm3.scale(float(size)/CHIP_SIZE, float(size)/CHIP_SIZE);
  return pix.xForm(wm3);
}

void Board::drawOnePiece(uint row, uint col, int i) {
  int px = col * zoomedSize() + 1;
  int py = row * zoomedSize() + 1;

  QPainter p(this);
  if (bg.width())
      p.drawTiledPixmap(px, py, zoomedSize(), zoomedSize(), bg, px, py);
  else p.fillRect(px, py, zoomedSize(), zoomedSize(), bgColor);

  p.setPen(black);
  p.drawRect(px, py, zoomedSize(), zoomedSize());

  if ( i==-1 ) return;
  p.drawPixmap(px, py, chipPixmap(i, zoomedSize()));
}

void Board::drawPiece(uint row, uint col, Color color) {
  int i = (color==Nobody ? -1 : int(CHIP_OFFSET[color]));
  drawOnePiece(row, col, i);
}


void Board::paintEvent(QPaintEvent *) {
  updateBoard(true);
}

void Board::adjustSize() {
  int w = 8 * zoomedSize();
  setFixedSize(w+2, w+2);
}

void Board::setPixmap(QPixmap &pm) {
  if ( pm.width()==0 ) return;
  bg = pm;
  update();
  setErasePixmap(pm);
}

void Board::setColor(const QColor &c) {
  bgColor = c;
  bg = QPixmap();
  update();
  setEraseColor(c);
}

// saves the game. Only one game at a time can be saved
void Board::saveGame(KConfig *config) {
  interrupt(); // stop thinking
  config->writeEntry("NumberOfMoves", moveNumber());
  config->writeEntry("State", state());
  config->writeEntry("Strength", strength());
  for(uint i = moveNumber(); i > 0; i--) {
    Move m = game->lastMove();
    game->TakeBackMove();
    QString s, idx;
    s.sprintf("%d %d %d", m.x(), m.y(), (int)m.color());
    idx.sprintf("Move_%d", i);
    config->writeEntry(idx, s);
  }

  // save whose turn it is
  config->writeEntry("WhoseTurn", (int)human);
  config->sync();

  // all moves must be redone
  
  
  loadGame(config, TRUE);
  doContinue(); // continue possible move
}

// loads the game. Only one game at a time can be saved
bool Board::loadGame(KConfig *config, bool noupdate) {
  interrupt(); // stop thinking
  uint nmoves = config->readNumEntry("NumberOfMoves", 0);
  if(nmoves==0) return false;
  game->Reset();
  uint movenumber = 1;
  while(nmoves--) {
    // read one move
    QString idx;
    idx.sprintf("Move_%d", movenumber++);
    QStringList s = config->readListEntry(idx, ' ');
    uint x = (*s.at(0)).toUInt();
    uint y = (*s.at(1)).toUInt();
    Color color = (Color)(*s.at(2)).toInt();
    Move m(x, y, color);
    game->MakeMove(m);
  }

  if(noupdate)
    return true;

  human = (Color)config->readNumEntry("WhoseTurn");

  updateBoard(TRUE);
  setState(State(config->readNumEntry("State")));
  setStrength(config->readNumEntry("Strength", 1));
    
  if(interrupted())
    doContinue();
  else {
    emit turn(Black);
    // computer makes first move
    if(human == White)
      computerMakeMove();
  }
  return true;
}

void Board::loadSettings(){
  if( Prefs::grayscale() ) {
    if(chiptype != Grayscale)
      loadChips(Grayscale);
  }
  else {
    if(chiptype != Colored)
      loadChips(Colored);
  }

  if( !Prefs::animation() )
    setAnimationSpeed(0);
  else
    setAnimationSpeed(10 - Prefs::animationSpeed());
  setStrength(Prefs::skill());

  if ( Prefs::backgroundImageChoice() ) {
    QPixmap pm( Prefs::backgroundImage() );
    if(!pm.isNull())
      setPixmap(pm);
  } else {
    setColor( Prefs::backgroundColor() );
  }

  /*
  // This should be changed...
  setColor(paletteBackgroundColor());
  if(config->readNumEntry("Background", -1) != -1) {
    int i = config->readNumEntry("Background");
    if(i == 1) {
      QColor s = config->readColorEntry("BackgroundColor");
      setColor(s);
    } else if(i == 2) {
      QString s = locate("appdata", config->readEntry("BackgroundPixmap"));
      if(!s.isEmpty()) {
        QPixmap bg(s);
        if(bg.width())
          setPixmap(bg);
      }
    }
  }
  else
  {
    QPixmap bg(locate("appdata", "pics/background/Light_Wood.png"));
    if (bg.width())
      setPixmap(bg);
  }
  */
}

#include "board.moc"


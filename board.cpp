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

#include "misc.h"
#include "board.h"
#include <qpainter.h>
#include <qimage.h>
#include <qbitmap.h>

#include <kapplication.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kstandarddirs.h>

#include "Score.h"
#include "playsound.h"

#include <config.h>
#include <unistd.h>

#define APPDATA(x) KGlobal::dirs()->findResource("appdata", x)
#define PICDATA(x) KGlobal::dirs()->findResource("appdata", QString("pics/")+ x)

extern QString SOUNDDIR;

const int HINT_BLINKRATE = 250000;
const int DEFAULT_ANIMATION_DELAY = 4;
const int ANIMATION_DELAY = 3000;
const int CHIP_BLACK	  = 1;
const int CHIP_WHITE      = 24;
const int CHIP_MAX	  = 25;
const int CHIP_SIZE       = 36;

Board::Board(QWidget *parent) : QWidget(parent) {
  setAnimationSpeed(DEFAULT_ANIMATION_DELAY);
  setUpdatesEnabled(FALSE);

  // ensure that the first time adjustsize does
  // pixmap loading
  oldsizehint = -1;

  _size = 32;
  _zoom = 100;
  _zoomed_size = (_size * _zoom) / 100 + 2;

  nopaint = FALSE;
  human = Score::BLACK;
  setStrength(4);

  connect(this, SIGNAL(signalFieldClicked(int, int)),
	  this, SLOT(slotFieldClicked(int, int)));
}


Board::~Board() {
}

void Board::start() {
  // make sure a signal is emitted  
  setStrength(e.GetStrength());
  newGame();
  adjustSize();
  setUpdatesEnabled(TRUE);
}

void Board::loadChips(const char *filename) {
  allchips.load(PICDATA(filename));
  chipname = filename;
  if(scaled_allchips.width())
  {
    loadPixmaps(); // Reload
    update();
  }
}

QString Board::chipsName() {
  return chipname;
}


int Board::getMoveNumber() {
  return g.GetMoveNumber();
}

// negative speed is allowed. if speed is negative,
// no animations are displayed
void Board::setAnimationSpeed(int speed) {
  if(speed <= 10)
    anim_speed = speed;
}

int Board::animationSpeed() {
  return anim_speed;
}

/// takes back last set of moves
void Board::undo() {
  if((getState() == READY)) {
    int last_player = g.GetLastMove().GetPlayer();
    while ((g.GetMoveNumber() != 0) && 
           (last_player == g.GetLastMove().GetPlayer()))
       g.TakeBackMove();
    g.TakeBackMove();
    update();
  }
}


/// interrupt thinking of game engine
void Board::interrupt() {
  e.SetInterrupt(TRUE);
}

bool Board::interrupted() {
  return ((g.GetWhoseTurn() == computerIs()) && (getState() == THINKING));
}


/// continues a move if it was prior interrupted
void Board::doContinue() {
  if(interrupted())
    computerMakeMove();
}


/// adjusts the size
void Board::adjustSize() {
  // do only resize if size has really change to avoid a flickering display
  if(sizeHint().width() != oldsizehint) {
    loadPixmaps();
    resize(sizeHint());
    emit sizeChange();
    oldsizehint = sizeHint().width();
  }
}


/// returns which color the human plays
int  Board::humanIs() {
  return human;
}


/// returns which color the computer plays
int  Board::computerIs() {
  if(human == Score::BLACK)
    return Score::WHITE;
  else
    return Score::BLACK;
}


/// starts a new game
void Board::newGame() {  
  //  return;
  g.Reset();
  updateBoard(TRUE);
  setState(READY);

  emit turn(Score::BLACK);
  
  // computer makes first move
  if(human == Score::WHITE)
    computerMakeMove();
}


/// handles mouse clicks
void Board::mousePressEvent(QMouseEvent *e) {
  if(getState() == READY) {
    int px = (e->pos().x()-1) / _zoomed_size;
    int py = (e->pos().y()-1) / _zoomed_size;
    emit signalFieldClicked(py, px);
  } else if(getState() == HINT)
    setState(READY);
  else
    emit illegalMove();
}


/// handles piece settings
void Board::slotFieldClicked(int row, int col) {
  if(getState() == READY) {
    int color = g.GetWhoseTurn();

    /// makes a human move
    Move m(col + 1, row + 1, color);
    if(g.MoveIsLegal(m)) {
      //      playSound("click.wav");
      g.MakeMove(m);
      animateChanged(m);

      if(!g.MoveIsAtAllPossible()) {
	updateBoard();
	setState(READY);
	gameEnded();
	return;
      }

      updateBoard();

      if(color != g.GetWhoseTurn())
	computerMakeMove();
    } else 
      emit illegalMove();
  }
}


/// makes a computer move
void Board::computerMakeMove() {
  // check if the computer can move
  int color = g.GetWhoseTurn();
  int opponent = g.GetWhoseTurnOpponent();

  emit turn(color);

  if(g.MoveIsPossible(color)) {
    setState(THINKING);
    do {
      Move m;

      if(!g.MoveIsAtAllPossible()) {
	setState(READY);
	gameEnded();
	return;
      }

      m = e.ComputeMove(g);
      if(m.GetX() == -1) {
	setState(READY);
	return;
      }
      sleep(1); // Pretend we have to think hard.

      //playSound("click.wav");
      g.MakeMove(m);
      animateChanged(m);
      updateBoard();
    } while(!g.MoveIsPossible(opponent));
    emit turn(opponent);
    setState(READY);

    if(!g.MoveIsAtAllPossible()) {
      setState(READY);
      gameEnded();
      return;
    }
  }
}


/// returns the current score
void Board::getScore(int &black, int &white) {
  black = g.GetScore(Score::BLACK);
  white = g.GetScore(Score::WHITE);
} 


/// calculate final score
void Board::gameEnded() {
  int b, w;

  getScore(b, w);
  if(b > w)
    emit gameWon(Score::BLACK);
  else if(b < w)
    emit gameWon(Score::WHITE);
  else
    emit gameWon(Score::NOBODY);
  emit turn(Score::NOBODY);
}


void Board::switchSides() {
  if(getState() == READY) {
    if(human == Score::WHITE)
      human = Score::BLACK;
    else
      human = Score::WHITE;
    emit score();
    computerMakeMove();
  }
}


int  Board::getState() {
  return _status;
}

void Board::setState(int nstatus) {
  _status = nstatus;
  emit statusChange(_status);
}


void Board::setStrength(int st) {
  if((st > 1) && (st <= 10)) {
    e.SetStrength(st);
    emit strengthChanged(e.GetStrength());
  }
}


int Board::getStrength() {
  return e.GetStrength();
}


void Board::hint() {
  if(getState() == READY) {
    setState(THINKING);
    Move m = e.ComputeMove(g);
    setState(HINT);
    if(m.GetX() != -1) {
      for(int flash = 0; (flash < 100) && (getState() != READY); flash++) {
	if(flash & 1)
	  drawOnePiece(m.GetY() - 1, m.GetX() - 1, Score::NOBODY);
	else
	  drawOnePiece(m.GetY() - 1, m.GetX() - 1, g.GetWhoseTurn());

	// keep GUI alive while waiting
	for(int dummy = 0; dummy < 5; dummy++) {
	  usleep(HINT_BLINKRATE/5);
	  qApp->processEvents();
	}
      }
      
      drawOnePiece(m.GetY() - 1, m.GetX() - 1, 
		   g.GetSquare(m.GetX(), m.GetY()));
    }
    setState(READY);
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
// NOTE: this code is white a hack. Should make it better
void Board::animateChanged(Move m) {
  if(animationSpeed() <= 0)
    return;

  // draw the new piece

  drawOnePiece(m.GetY()-1, m.GetX()-1, m.GetPlayer());
  soundSync();

  for(int dx = -1; dx < 2; dx++)
    for(int dy = -1; dy < 2; dy++)
      if((dx != 0) || (dy != 0))
	animateChangedRow(m.GetY()-1, m.GetX()-1, dy, dx);
}


bool Board::isField(int row, int col) {
  return (bool)((row > -1) && (row < 8) && (col > -1) && (col < 8));
}


void Board::animateChangedRow(int row, int col, int dy, int dx) {
  row = row + dy;
  col = col + dx;
  while(isField(row, col)) {
    if(g.wasTurned(col+1, row+1)) {
      playSound("reversi-click.wav");
      rotateChip(row, col);
      soundSync();
   } else
      return;

    col += dx;
    row += dy;
  }
}

// NOTE: this code is S**T
void Board::rotateChip(int row, int col) {
  // check which direction the chip has to be rotated
  // if the new chip is white, the chip was black first,
  // so lets begin at index 1, otherwise it was white
  int color = g.GetSquare(col+1, row+1);
  int from, end, delta;
  if(color == Score::WHITE) {
    from = CHIP_BLACK + 2;
    end  = CHIP_WHITE - 2;
    delta = 1;
  } else if(color == Score::BLACK) {
    from = CHIP_WHITE -2;
    end  = CHIP_BLACK + 2;
    delta = -1;
  } else
    return;

  int px = col * _zoomed_size + 2;
  int py = row * _zoomed_size + 2;

  int w = scaled_allchips.width()/5;
  int h = scaled_allchips.height()/5;

  QPainter p;
  
  for(int i = from; i != end; i += delta) {
    QPixmap pix(_zoomed_size-2, _zoomed_size-2);

    p.begin(&pix);
    if (bg.width())
       p.drawTiledPixmap(0, 0, _zoomed_size-2, _zoomed_size-2, bg, px, py);
    else
       p.fillRect(0, 0, _zoomed_size-2, _zoomed_size-2, eraseColor());
    p.drawPixmap(0, 0, scaled_allchips, (i % 5) * w + 10, (i / 5) * h + 10,
	   _zoomed_size-2, _zoomed_size-2);
    p.flush();
    p.end();

    bitBlt(this, px, py, &pix, 0, 0, _zoomed_size-2, _zoomed_size-2, CopyROP);
    kapp->flushX();
    usleep(ANIMATION_DELAY * anim_speed);
  }
  
  drawOnePiece(row, col, color);
}

bool Board::canZoomIn() {
  return (bool)(width() < 640);
}


bool Board::canZoomOut() {
  return (bool)(width() > 200);
}


void Board::setZoom(int _new) {
  if(((_new < _zoom) && canZoomOut()) ||
     ((_new > _zoom) && canZoomIn())) {
    if(_new != _zoom) {
      _zoom = _new;
      _zoomed_size = (_size * _zoom) / 100 + 2;
      adjustSize();
      emit sizeChange();
    }
  }
}


int Board::getZoom() const {
  return _zoom;
}


void Board::zoomIn() {
  setZoom(_zoom + 20);
}


void Board::zoomOut() {
  setZoom(_zoom - 20);
}

void Board::loadPixmaps() {
  if (scaled_allchips.width() != (_zoomed_size*5))
  {
    scaled_allchips = QPixmap(_zoomed_size*5, _zoomed_size*5);
    QWMatrix wm3;
    wm3.scale((float)(_zoomed_size*5)/(CHIP_SIZE * 5),
              (float)(_zoomed_size*5)/(CHIP_SIZE * 5));
    scaled_allchips = allchips.xForm(wm3);
  }
  if (bg.width())
    setErasePixmap(bg);
}


void Board::updateBoard(bool force) {
  for(int row = 0; row < 8; row++)
    for(int col = 0; col < 8; col++) 
      drawPiece(row, col, force);

  emit score();
}


void Board::drawOnePiece(int row, int col, int color) {
  int px = col * _zoomed_size + 2;
  int py = row * _zoomed_size + 2;

  int w = scaled_allchips.width()/5;
  int h = scaled_allchips.height()/5;

  QPixmap tmp(_zoomed_size-2, _zoomed_size-2);

  QPainter p;
  p.begin(&tmp);
  if (bg.width())
    p.drawTiledPixmap(0, 0, _zoomed_size-2, _zoomed_size-2, bg, px, py);
  else
    p.fillRect(0, 0, _zoomed_size-2, _zoomed_size-2, eraseColor());
  
  if(color == Score::BLACK) {
    // easy to understand, isn't it :-)
    int i = CHIP_BLACK;
    p.drawPixmap(0, 0, scaled_allchips, (i % 5) * w + 10, (i / 5) * h + 10,
	   _zoomed_size-2, _zoomed_size-2);
  }
  else if(color == Score::WHITE) {
    // easy to understand, isn't it :-)
    int i = CHIP_WHITE;
    p.drawPixmap(0, 0, scaled_allchips, (i % 5) * w + 10, (i / 5) * h + 10,
	   _zoomed_size-2, _zoomed_size-2);
  }
  p.flush();
  p.end();  
  bitBlt(this, px, py, &tmp, 0, 0, _zoomed_size-2, _zoomed_size-2, CopyROP);
  kapp->flushX();
}


void Board::drawPiece(int row, int col, bool force) {
  if(g.squareModified(col+1, row+1) || force) {
    int color = g.GetSquare(col + 1, row + 1);
    drawOnePiece(row, col, color);
  }
}


void Board::paintEvent(QPaintEvent *) {
  int w = sizeHint().width() - 2;
  
  if(!nopaint) {
    QPainter p;
    
    p.begin(this);
    p.setPen(QPen(QColor("black"), 2));
    
    // draw vertical lines
    for(int i = 1; i < 8; i++) {
      int x = i * _zoomed_size;
      p.drawLine(x+1, 0, x+1, w);
    }
    
    // draw horizontal lines
    for(int i = 1; i < 8; i++) {
      int y = i * _zoomed_size;
      p.drawLine(0, y+1, w, y+1);
    }

    p.drawRect(1, 1, w+1, w+1);
    
    p.end();
    updateBoard(TRUE);
  }
}


QSize Board::sizeHint() const {
  int w = 8 * _zoomed_size;
  return QSize(w+2, w+2);
}

void Board::setPixmap(QPixmap &pm) {
  bg = pm;
  if(scaled_allchips.width())
  {
    loadPixmaps(); // Reload
    update();
  }
}

void Board::setColor(const QColor &c) {
  setBackgroundColor(c);
  bg.resize(0, 0);
  if(scaled_allchips.width())
  {
    loadPixmaps(); // Reload
    update();
  }
} 

// saves the game. Only one game at a time can be saved
void Board::saveGame(KConfig *config) {
  interrupt(); // stop thinking
  config->writeEntry("NumberOfMoves", g.GetMoveNumber());
  int nmoves = g.GetMoveNumber();
  config->writeEntry("State", getState());
  for(int i = nmoves; i > 0; i--) {    
    Move m = g.GetLastMove();
    g.TakeBackMove();
    QString s, idx;
    s.sprintf("%d %d %d", m.GetX(), m.GetY(), m.GetPlayer());
    idx.sprintf("Move_%d", i);
    config->writeEntry(idx, s);
  }
  
  // save whose turn it is
  config->writeEntry("WhoseTurn", human);

  // all moves must be redone
  loadGame(config, TRUE);
  doContinue(); // continue possible move
}

// loads the game. Only one game at a time can be saved
void Board::loadGame(KConfig *config, bool noupdate) {
  interrupt(); // stop thinking
  int nmoves = config->readNumEntry("NumberOfMoves", -1);
  if(nmoves > 0) {
    g.Reset();
    int movenumber = 1;
    while(nmoves--) {
      // read one move
      QString idx;
      idx.sprintf("Move_%d", movenumber++);
      QStringList s = config->readListEntry(idx, ' ');
      int x = (*s.at(0)).toInt();
      int y = (*s.at(1)).toInt();
      int pl = (*s.at(2)).toInt();
      Move m(x, y, pl);
      g.MakeMove(m);
    }

    if(noupdate)
      return;

    human = config->readNumEntry("WhoseTurn");
  
    updateBoard(TRUE);
    setState(config->readNumEntry("State"));

    if(interrupted())
      doContinue();
    else {      
      emit turn(Score::BLACK);
      // computer makes first move
      if(human == Score::WHITE)
	computerMakeMove();
    }
  }
}

bool Board::canLoad(KConfig *config) {
  int nmoves = config->readNumEntry("NumberOfMoves", -1);
  return (bool)(nmoves > 0);
}

#include "board.moc"

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
#include <qpainter.h>

#include <kapplication.h>
#include <kstandarddirs.h>
#include <kconfig.h>

#include "playsound.h"
#include "Engine.h"

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

Board::Board(QWidget *parent, const char *name) : QWidget(parent, name), 
  chipname(""),
  // ensure that the first time adjustsize does
  // pixmap loading
  oldsizehint(-1),
  _size(32),
  _zoom(100),
  _zoomed_size((_size * _zoom) / 100 + 2),

  human(Score::BLACK),
  nopaint(FALSE){
  
  setAnimationSpeed(DEFAULT_ANIMATION_DELAY);
  setUpdatesEnabled(FALSE);
  engine = new Engine();
  game = new Game();
  setStrength(1);

  connect(this, SIGNAL(signalFieldClicked(int, int)),
	  this, SLOT(slotFieldClicked(int, int)));
  loadSettings();
}


Board::~Board() {
  delete engine;
  delete game;
}

void Board::start() {
  // make sure a signal is emitted  
  setStrength(engine->GetStrength());
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
  return game->GetMoveNumber();
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
    int last_player = game->GetLastMove().GetPlayer();
    while ((game->GetMoveNumber() != 0) && 
           (last_player == game->GetLastMove().GetPlayer()))
       game->TakeBackMove();
    game->TakeBackMove();
    update();
  }
}


/// interrupt thinking of game engine
void Board::interrupt() {
  engine->SetInterrupt(TRUE);
}

bool Board::interrupted() {
  return ((game->GetWhoseTurn() == computerIs()) && (getState() == READY));
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
    oldsizehint = sizeHint().width();
    emit sizeChange();
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
  return Score::BLACK;
}

/// starts a new game
void Board::newGame() {  
  //  return;
  game->Reset();
  updateBoard(TRUE);
  setState(READY);

  emit turn(Score::BLACK);
  
  // computer makes first move
  if(human == Score::WHITE)
    computerMakeMove();
}

/// handles mouse clicks
void Board::mousePressEvent(QMouseEvent *e) {
  if(interrupted()){
    emit illegalMove();
    return;
  }
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
    int color = game->GetWhoseTurn();

    /// makes a human move
    Move m(col + 1, row + 1, color);
    if(game->MoveIsLegal(m)) {
      //      playSound("click.wav");
      game->MakeMove(m);
      animateChanged(m);

      if(!game->MoveIsAtAllPossible()) {
	updateBoard();
	setState(READY);
	gameEnded();
	return;
      }

      updateBoard();

      if(color != game->GetWhoseTurn())
	computerMakeMove();
    } else 
      emit illegalMove();
  }
}


/// makes a computer move
void Board::computerMakeMove() {
  // check if the computer can move
  int color = game->GetWhoseTurn();
  int opponent = game->GetWhoseTurnOpponent();

  emit turn(color);

  if(game->MoveIsPossible(color)) {
    setState(THINKING);
    do {
      Move m;

      if(!game->MoveIsAtAllPossible()) {
	setState(READY);
	gameEnded();
	return;
      }

      m = engine->ComputeMove(*game);
      if(m.GetX() == -1) {
	setState(READY);
	return;
      }
      usleep(300000); // Pretend we have to think hard.
      
      //playSound("click.wav");
      game->MakeMove(m);
      animateChanged(m);
      updateBoard();
    } while(!game->MoveIsPossible(opponent));
    
    
    emit turn(opponent);
    setState(READY);

    if(!game->MoveIsAtAllPossible()) {
      setState(READY);
      gameEnded();
      return;
    }
  }
}


/// returns the current score
void Board::getScore(int &black, int &white) {
  black = game->GetScore(Score::BLACK);
  white = game->GetScore(Score::WHITE);
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
    kapp->processEvents();
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
  if((st >= 1) && (st <= 8)) {
    engine->SetStrength(st);
    emit strengthChanged(engine->GetStrength());
  }
}

int Board::getStrength() {
  return engine->GetStrength();
}

void Board::hint() {
  if(getState() == READY) {
    setState(THINKING);
    Move m = engine->ComputeMove(*game);
    setState(HINT);
    if(m.GetX() != -1) {
      for(int flash = 0; (flash < 100) && (getState() != READY); flash++) {
	if(flash & 1)
	  drawOnePiece(m.GetY() - 1, m.GetX() - 1, Score::NOBODY);
	else
	  drawOnePiece(m.GetY() - 1, m.GetX() - 1, game->GetWhoseTurn());

	// keep GUI alive while waiting
	for(int dummy = 0; dummy < 5; dummy++) {
	  usleep(HINT_BLINKRATE/5);
	  qApp->processEvents();
	}
      }
      
      drawOnePiece(m.GetY() - 1, m.GetX() - 1, 
		   game->GetSquare(m.GetX(), m.GetY()));
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
// NOTE: this code is quite a hack. Should make it better
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
    if(game->wasTurned(col+1, row+1)) {
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
  int color = game->GetSquare(col+1, row+1);
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
  if(game->squareModified(col+1, row+1) || force) {
    int color = game->GetSquare(col + 1, row + 1);
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
  config->writeEntry("NumberOfMoves", game->GetMoveNumber());
  int nmoves = game->GetMoveNumber();
  config->writeEntry("State", getState());
  config->writeEntry("Strength", getStrength());
  for(int i = nmoves; i > 0; i--) {    
    Move m = game->GetLastMove();
    game->TakeBackMove();
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
    game->Reset();
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
      game->MakeMove(m);
    }

    if(noupdate)
      return;

    human = config->readNumEntry("WhoseTurn");
  
    updateBoard(TRUE);
    setState(config->readNumEntry("State"));
    setStrength(config->readNumEntry("Strength", 10));
    
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

void Board::loadSettings(){
  KConfig *config = kapp->config();
  config->setGroup("Game");
  if(config->readBoolEntry("Grayscale", false)){
    if(chipname != "chips_mono.png")
      loadChips("chips_mono.png");
  }
  else{
    if(chipname != "chips.png") loadChips("chips.png");
  }

  if(config->readBoolEntry("Animation", true) == false)
    setAnimationSpeed(-1);
  else
    setAnimationSpeed(10-config->readNumEntry("AnimationSpeed", DEFAULT_ANIMATION_DELAY));
  setZoom(config->readNumEntry("Zoom", 100));
  setFixedSize(sizeHint());
  setStrength(config->readNumEntry("Skill", 1));


  if(config->readBoolEntry("BackgroundColorChoice", false))
    setColor(config->readColorEntry("BackgroundColor"));
  else{
    QPixmap pm(config->readEntry("Background", PICDATA("background/Light_Wood.png")));
    if(!pm.isNull())
      setPixmap(pm);
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


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


#define PICDATA(x) KGlobal::dirs()->findResource("appdata", QString("pics/")+ x)

const uint  HINT_BLINKRATE        = 250000;
const uint  ANIMATION_DELAY       = 3000;
const uint  CHIP_OFFSET[NbColors] = { 24, 1 };
const uint  CHIP_SIZE             = 36;


//-----------------------------------------------------------------


Board::Board(QWidget *parent)
    : QWidget(parent, "board"),
      m_humanColor(Black), chiptype(Unloaded)
{
  m_engine = new Engine();
  m_game   = new Game();

  setStrength(1);
}


Board::~Board() {
  delete m_engine;
  delete m_game;
}


// ----------------------------------------------------------------


uint Board::zoomedSize() const
{
  return qRound(float(CHIP_SIZE) * Prefs::zoom() / 100);
}


// Start it all up.
//

void Board::start()
{
  // make sure a signal is emitted
  setStrength(strength());
  newGame();
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


// Takes back last set of moves
//

void Board::undo()
{
  if (state() != Ready) 
    return;

  Color last_color = m_game->lastMove().color();
  while (m_game->moveNumber() != 0
	 && last_color == m_game->lastMove().color())
    m_game->TakeBackMove();

  m_game->TakeBackMove();
  update();
}


// Interrupt thinking of game engine.
//

void Board::interrupt()
{
  m_engine->setInterrupt(TRUE);
}


bool Board::interrupted() const 
{
  return ((m_game->toMove() == computerColor()) && (state() == Ready));
}


// Continues a move if it was interrupted earlier.
//

void Board::doContinue()
{
  if (interrupted())
    computerMakeMove();
}


// Starts a new game.
//

void Board::newGame()
{
  m_game->Reset();
  m_competitiveGame = Prefs::competitiveGameChoice();
  //kdDebug() << "Competitive: " << m_competitiveGame << endl;

  updateBoard(TRUE);
  setState(Ready);

  emit turn(Black);

  // Black always makes first move.
  if (m_humanColor == White)
    computerMakeMove();
}


// Handle mouse clicks.
//

void Board::mousePressEvent(QMouseEvent *e)
{
  if ( e->button() != LeftButton ) {
    e->ignore();
    return;
  }

  if ( interrupted() ) {
    illegalMove();
    return;
  }

  if (state() == Ready) {
    int px = (e->pos().x()-1) / zoomedSize();
    int py = (e->pos().y()-1) / zoomedSize();
    fieldClicked(py, px);
  } 
  else if (state() == Hint)
    setState(Ready);
  else
    illegalMove();
}


void Board::illegalMove()
{
  KNotifyClient::event(winId(), "illegal_move", i18n("Illegal move"));
}


// Handle piece settings.
//

void Board::fieldClicked(int row, int col)
{
  if (state() != Ready) 
    return;

  Color color = m_game->toMove();

  // Create a move from the mouse click and see if it is legal.
  // If it is, then make a human move.
  Move  m(color, col + 1, row + 1);
  if (m_game->moveIsLegal(m)) {
    m_game->MakeMove(m);
    animateChanged(m);

    if (!m_game->moveIsAtAllPossible()) {
      updateBoard();
      setState(Ready);
      gameEnded();
      return;
    }

    updateBoard();

    if (color != m_game->toMove())
      computerMakeMove();
  } else
    illegalMove();
}


// Makes a computer move.
//

void Board::computerMakeMove()
{
  // Check if the computer can move.
  Color color    = m_game->toMove();
  Color opponent = ::opponent(color);

  emit turn(color);

  if (!m_game->moveIsPossible(color))
    return;
 
  // Make computer moves until the human can play or until the game is over.
  setState(Thinking);
  do {
    Move  move;

    if (!m_game->moveIsAtAllPossible()) {
      setState(Ready);
      gameEnded();
      return;
    }

    move = m_engine->computeMove(*m_game, m_competitiveGame);
    if (move.x() == -1) {
      setState(Ready);
      return;
    }
    usleep(300000); // Pretend we have to think hard.

    //playSound("click.wav");
    m_game->MakeMove(move);
    animateChanged(move);
    updateBoard();
  } while (!m_game->moveIsPossible(opponent));


  emit turn(opponent);
  setState(Ready);

  if (!m_game->moveIsAtAllPossible()) {
    gameEnded();
    return;
  }
}


// Calculate the final score.
//

void Board::gameEnded()
{
  uint  black = score(Black);
  uint  white = score(White);
  if (black > white)
    emit gameWon(Black);
  else if (black < white)
    emit gameWon(White);
  else
    emit gameWon(Nobody);

  emit turn(Nobody);
}


void Board::switchSides()
{
  if (state() != Ready) 
    return;

  m_humanColor = opponent(m_humanColor);
  Prefs::setHumanColor((int) m_humanColor);
  Prefs::setComputerColor((int) opponent(m_humanColor));
  Prefs::writeConfig();
  emit score();
  kapp->processEvents();
  computerMakeMove();
}


void Board::setState(State nstatus)
{
  m_status = nstatus;
  emit statusChange(m_status);
}


void Board::setStrength(uint st)
{
  // FIXME: 7 should be MAXSTRENGTH or something similar.
  Q_ASSERT( 1 <= st && st <= 7 );

  st = QMAX(QMIN(st, 7), 1);
  m_engine->setStrength(st);
  KExtHighscore::setGameType(st-1);
}


uint Board::strength() const
{
  return m_engine->strength();
}


uint Board::moveNumber() const
{
  return m_game->moveNumber();
}


uint Board::score(Color color) const
{
  return m_game->score(color);
}


Color Board::whoseTurn() const
{
  return m_game->toMove();
}


void Board::hint()
{
  if (state() != Ready) 
    return;

  setState(Thinking);
  Move  move = m_engine->computeMove(*m_game, m_competitiveGame);

  setState(Hint);
  if (move.x() != -1) {
    // The isVisible condition has been added so that when the player
    // was viewing a hint and quits the game window, the game doesn't
    // still have to do all this looping and directly ends
    for (int flash = 0;
	 (flash < 100) && (state() != Ready) && isVisible(); flash++) {

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
    drawPiece(move.y() - 1, move.x() - 1, m_game->color(move.x(), move.y()));
  }

  setState(Ready);
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

  // draw the new piece
  drawPiece(move.y() - 1, move.x() - 1, move.color());

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
  for (uint row = 0; row < 8; row++)
    for (uint col = 0; col < 8; col++)
      if ( force || m_game->squareModified(col + 1, row + 1) ) {
	Color  color = m_game->color(col + 1, row + 1);
	drawPiece(row, col, color);
      }

  QPainter  p(this);
  p.setPen(black);
  p.drawRect(0, 0, 8 * zoomedSize() + 2, 8 * zoomedSize() + 2);

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


void Board::drawPiece(uint row, uint col, Color color)
{
  int i = (color == Nobody ? -1 : int(CHIP_OFFSET[color]));
  drawOnePiece(row, col, i);
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


// Saves the game.  Only one game at a time can be saved.
//

void Board::saveGame(KConfig *config)
{
  // Stop thinking.
  interrupt(); 

  // Write the data to the config file.
  config->writeEntry("NumberOfMoves", moveNumber());
  config->writeEntry("State", state());
  config->writeEntry("Strength", strength());

  // Write the game itself to the file.
  for (uint i = moveNumber(); i > 0; i--) {
    Move  move = m_game->lastMove();
    m_game->TakeBackMove();

    QString s, idx;
    s.sprintf("%d %d %d", move.x(), move.y(), (int)move.color());
    idx.sprintf("Move_%d", i);
    config->writeEntry(idx, s);
  }

  // save whose turn it is and if the game is casual.
  config->writeEntry("WhoseTurn",   (int) m_humanColor);
  config->writeEntry("Competitive", (int) m_competitiveGame);
  config->sync();

  // All moves must be redone.
  loadGame(config, TRUE);

  // Continue with the move if applicable.
  doContinue(); 
}


// Loads the game.  Only one game at a time can be saved.
//

bool Board::loadGame(KConfig *config, bool noupdate)
{
  interrupt(); // stop thinking

  uint  nmoves = config->readNumEntry("NumberOfMoves", 0);
  if (nmoves==0) 
    return false;

  m_game->Reset();
  uint movenumber = 1;
  while (nmoves--) {
    // Read one move.
    QString  idx;
    idx.sprintf("Move_%d", movenumber++);

    QStringList  s = config->readListEntry(idx, ' ');
    uint         x = (*s.at(0)).toUInt();
    uint         y = (*s.at(1)).toUInt();
    Color        color = (Color)(*s.at(2)).toInt();

    Move move(color, x, y);
    m_game->MakeMove(move);
  }

  if (noupdate)
    return true;

  m_humanColor = (Color)config->readNumEntry("WhoseTurn");
  Prefs::setHumanColor((int) m_humanColor);
  Prefs::setComputerColor((int) opponent(m_humanColor));
  Prefs::writeConfig();
  updateBoard(TRUE);
  setState(State(config->readNumEntry("State")));
  setStrength(config->readNumEntry("Strength", 1));
  m_competitiveGame = (bool) config->readNumEntry("Competitive");
  //kdDebug() << "Competitive set to: " << m_competitiveGame << endl;

  if (interrupted())
    doContinue();
  //it's always the human's turn to play in a load game as you cannot save a game while playing
  emit turn(m_humanColor);
  return true;
}


void Board::loadSettings()
{
  m_humanColor = (Color) Prefs::humanColor();
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
  setStrength(Prefs::skill());

  // This is set at the start of a game and can only be downgraded
  // during the game.
  if ( !Prefs::competitiveGameChoice() )
    m_competitiveGame = false;

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


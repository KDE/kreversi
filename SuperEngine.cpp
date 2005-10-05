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
 * Created 1997 by Mario Weilguni <mweilguni@sime.com>. This file 
 * is ported from Mats Luthman's <Mats.Luthman@sylog.se> JAVA applet. 
 * Many thanks to Mr. Luthman who has allowed me to put this port 
 * under the GNU GPL. Without his wonderful game engine kreversi 
 * would be just another of those Reversi programs a five year old 
 * child could beat easily. But with it it's a worthy opponent!
 *
 * If you are interested on the JAVA applet of Mr. Luthman take a
 * look at http://www.sylog.se/~mats/
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
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *******************************************************************
 */

// The class SuperEngine is a super class for engines that produce moves.
// It implements functionality that move engines have in common, which is
// useful if you want to use several different engines in the same program
// (for instance when you are test playing different strategies against each
// other).
//
// SuperEngine implements:
//
// Random number handling (engines should not play exactly the same games
// repeatedly).
//
// Setting playing strength level.
//
// Functionality for telling the engine to interrupt calculation.
//

//  Public and protected functions:

//  public SuperEngine(int st)
//     Creates an engine playing at level st. All integers greater than 0
//     should be possible levels. There need not be any actual difference in
//     playing strength between levels, but if there is, higher number should
//     mean greater playing strength.

//  public SuperEngine(int st, int sd)
//     The same as above, but uses sd as the seed for the random generator.
//     This means that the engine always behaves in exactly the same way
//     (practical when testing).

//  public synchronized final void SetInterrupt(boolean intr)
//     This function could be called when ComputeMove() (see below) is
//     executing. It tells the engine to interrupt calculation as
//     soon as possible and return null from ComputeMove().

//  protected synchronized final boolean GetInterrupt()
//     Returns true when SetInterrupt() has been called. Should be called
//     with short intervals from ComputeMove().

//  public void SetStrength(int st)
//     Sets playing strength level.

//  public int GetStrength()
//     Gets playing strength level.

//  public void SetSeed(int sd)
//     Changes the random seed.

//  public abstract Move ComputeMove(Game g)
//     This function should produce a move. If SetInterrupt() is called
//     during its execution it should return null as soon as possible.

//  Protected members:

//  protected int m_strength
//     Is set and read by SetStrength() and GetStrength().

//  protected Random m_random
//     Could (and should in a good engine) be used to prevent the engine from
//     repeating itself, always playing the same moves in the same positions.
//     If this is not done, winning once would make it possible to play the
//     same moves and win every time against the program.


#include "SuperEngine.h"


SuperEngine::SuperEngine(int st) 
{
  m_strength = st;
  m_random.setSeed(0);
  m_interrupt = false;
}


SuperEngine::SuperEngine(int st, int sd) 
{
  m_strength = st;
  m_random.setSeed(sd);
  m_interrupt = false;
}


void SuperEngine::setSeed(int sd) 
{ 
  m_random.setSeed(sd); 
}


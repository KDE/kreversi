
// This code is freely distributable, but may neither be sold nor used or
// included in any product sold for profit without permission from
// Mats Luthman.

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

SuperEngine::SuperEngine(int st) {
  m_strength = st;
  // JAVA m_random = new Random();
  m_interrupt = false;
}


SuperEngine::SuperEngine(int st, int sd) {
  m_strength = st;
  // JAVA m_random = new Random(sd);
  m_random.setSeed(sd);
  m_interrupt = false;
}


void SuperEngine::SetInterrupt(bool intr)
{
  m_interrupt =intr;
}


bool SuperEngine::GetInterrupt() { 
  return m_interrupt; 
}

void SuperEngine::SetStrength(int st) { 
  m_strength = st; 
}

int SuperEngine::GetStrength() { 
  return m_strength; 
}

void SuperEngine::SetSeed(int sd) { 
  m_random.setSeed(sd); 
}


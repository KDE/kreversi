/*
 * Copyright (c) 2004 Nicolas HADACEK (hadacek@kde.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include "highscores.h"

#include <klocale.h>
#include <kconfig.h>
#include <kapplication.h>


namespace KExtHighscore
{

const ExtManager::Data ExtManager::DATA[SuperEngine::NbStrengths] = {
  { I18N_NOOP("1 (Beginner)"), "beginner" },
  { I18N_NOOP("2"), 0 },
  { I18N_NOOP("3"), 0 },
  { I18N_NOOP("4 (Average)"), "average" },
  { I18N_NOOP("5"), 0 },
  { I18N_NOOP("6"), 0 },
  { I18N_NOOP("7 (Expert)"), "expert" }
};


ExtManager::ExtManager()
    : Manager(SuperEngine::NbStrengths)
{
  setShowStatistics(true);
  setShowDrawGamesStatistic(true);

  const uint       RANGE[6] = { 0, 32, 40, 48, 56, 64 };
  QMemArray<uint>  s;
  s.duplicate(RANGE, 6);
  setScoreHistogram(s, ScoreBound);
}


QString ExtManager::gameTypeLabel(uint gameType, LabelType type) const
{
  const Data &data = DATA[gameType];
  switch (type) {
  case Icon:     return data.icon;
  case Standard: return QString::number(gameType);
  case I18N:     return i18n(data.label);
  case WW:       break;
  }

  return QString::null;
}


void ExtManager::convertLegacy(uint gameType)
{
  // Since there is no information about the skill level
  // in the legacy highscore list, consider they are
  // for beginner skill ...
  qDebug("convert legacy %i", gameType);

  if ( gameType!=0 )
    return;

  KConfigGroupSaver  cg(kapp->config(), "High Score");

  for (uint i = 1; i <= 10; i++) {
    QString  key = "Pos" + QString::number(i);
    QString  name = cg.config()->readEntry(key + "Name", QString::null);

    if ( name.isEmpty() )
      name = i18n("anonymous");

    uint  score = cg.config()->readUnsignedNumEntry(key + "NumChips", 0);
    if ( score==0 )
      continue;

    QString    sdate = cg.config()->readEntry(key + "Date", QString::null);
    QDateTime  date  = QDateTime::fromString(sdate);
    Score      s(Won);

    s.setScore(score);
    s.setData("name", name);
    if ( date.isValid() )
      s.setData("date", date);
    submitLegacyScore(s);
  }
}


} // Namespace

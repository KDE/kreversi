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
#include <QVector>
#include <QDateTime>
#include <kglobal.h>
#include <kconfiggroup.h>


namespace KExtHighscore
{

ExtManager::ExtManager()
    : Manager(7)
{
    setShowMode( NeverShow );
    setShowStatistics(true);
    setShowDrawGamesStatistic(true);

    const uint       RANGE[6] = { 0, 32, 40, 48, 56, 64 };
    QVector<uint>  s;
    s.resize(6);
    qCopy(RANGE, RANGE + 6, s.begin());
    setScoreHistogram(s, ScoreBound);

    // FIXME dimsuz: somehow rearrange the code to be sure that this and in mainwindow.cpp are 
    // always in sync
    m_typeLabels << i18n("Very Easy") << i18n("Easy") << i18n("Normal");
    m_typeLabels << i18n("Hard") << i18n("Very Hard") << i18n("Unbeatable") << i18n("Champion");
}


QString ExtManager::gameTypeLabel(uint gameType, LabelType type) const
{
    switch (type) {
        case Standard:
            return QString::number(gameType);
        case I18N: 
            return m_typeLabels.at(gameType);
        case Icon:
            // FIXME dimsuz: implement
            break;
        case WW:
            break;
    }

    return QString();
}


// FIXME dimsuz: is this still needed?
/* 
void ExtManager::convertLegacy(uint gameType)
{
  // Since there is no information about the skill level
  // in the legacy highscore list, consider they are
  // for beginner skill ...
  if ( gameType!=0 )
    return;

  KConfigGroup  cg(KGlobal::config(), "High Score");

  for (uint i = 1; i <= 10; i++) {
    QString  key = "Pos" + QString::number(i);
    QString  name = cg.readEntry(key + "Name", QString());

    if ( name.isEmpty() )
      name = i18n("anonymous");

    uint  score = cg.readEntry(key + "NumChips", (uint)0);
    if ( score==0 )
      continue;

    QString    sdate = cg.readEntry(key + "Date", QString());
    QDateTime  date  = QDateTime::fromString(sdate);
    Score      s(Won);

    s.setScore(score);
    s.setData("name", name);
    if ( date.isValid() )
      s.setData("date", date);
    submitLegacyScore(s);
  }
}
 */


} // Namespace

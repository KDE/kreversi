/*
    SPDX-FileCopyrightText: 2001-2003 Nicolas Hadacek <hadacek@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "kexthighscore_item.h"

#include <QLocale>

#include "kexthighscore_internal.h"
#include "kexthighscore_gui.h"


namespace KExtHighscore
{

//-----------------------------------------------------------------------------
Item::Item(const QVariant &def, const QString &label, Qt::AlignmentFlag alignment)
    : _default(def), _label(label), _alignment(alignment),
      _format(NoFormat), _special(NoSpecial)
{}

Item::~Item()
{}

QVariant Item::read(uint, const QVariant &value) const
{
    return value;
}

void Item::setPrettyFormat(Format format)
{
    bool buint = (_default.metaType().id() == QMetaType::UInt);
    bool bdouble = (_default.metaType().id() == QMetaType::Double);
    bool bnum = (buint || bdouble || _default.metaType().id() == QMetaType::Int);

    switch (format) {
    case OneDecimal:
    case Percentage:
        Q_ASSERT(bdouble);
        break;
    case MinuteTime:
        Q_ASSERT(bnum);
        break;
    case DateTime:
        Q_ASSERT(_default.metaType().id() == QMetaType::QDateTime);
	break;
    case NoFormat:
        break;
    }

    _format = format;
}

void Item::setPrettySpecial(Special special)
{
    bool buint = (_default.metaType().id() == QMetaType::UInt);
    bool bnum = (buint || _default.metaType().id() == QMetaType::Double
                 || _default.metaType().id() == QMetaType::Int);

    switch (special) {
    case ZeroNotDefined:
        Q_ASSERT(bnum);
        break;
    case NegativeNotDefined:
        Q_ASSERT(bnum && !buint);
        break;
    case DefaultNotDefined:
        break;
    case Anonymous:
        Q_ASSERT(_default.metaType().id() == QMetaType::QString);
        break;
    case NoSpecial:
        break;
    }

     _special = special;
}

QString Item::timeFormat(uint n)
{
    Q_ASSERT( n<=3600 && n!=0 );
    n = 3600 - n;
    return QString::number(n / 60).rightJustified(2, QLatin1Char( '0' )) + QLatin1Char( ':' )
        + QString::number(n % 60).rightJustified(2, QLatin1Char( '0' ));
}

QString Item::pretty(uint, const QVariant &value) const
{
    switch (_special) {
    case ZeroNotDefined:
        if ( value.toUInt()==0 ) return QStringLiteral( "--" );
        break;
    case NegativeNotDefined:
        if ( value.toInt()<0 ) return QStringLiteral( "--" );
        break;
    case DefaultNotDefined:
        if ( value==_default ) return QStringLiteral( "--" );
        break;
    case Anonymous:
        if ( value.toString()==QLatin1String( ItemContainer::ANONYMOUS ) )
            return ItemContainer::ANONYMOUS_LABEL.toString();
        break;
    case NoSpecial:
        break;
    }

    switch (_format) {
    case OneDecimal:
        return QString::number(value.toDouble(), 'f', 1);
    case Percentage:
        return QString::number(value.toDouble(), 'f', 1) + QLatin1Char( '%' );
    case MinuteTime:
        return timeFormat(value.toUInt());
    case DateTime:
        if ( value.toDateTime().isNull() ) return QStringLiteral( "--" );
        return QLocale().toString(value.toDateTime());
    case NoFormat:
        break;
    }

    return value.toString();
}

//-----------------------------------------------------------------------------
Score::Score(ScoreType type)
    : _type(type)
{
    const ItemArray &items = internal->scoreInfos();
    for (int i=0; i<items.size(); i++)
        _data[items[i]->name()] = items[i]->item()->defaultValue();
}

Score::~Score()
{}

QVariant Score::data(const QString &name) const
{
    Q_ASSERT( _data.contains(name) );
    return _data[name];
}

void Score::setData(const QString &name, const QVariant &value)
{
    Q_ASSERT( _data.contains(name) );
    Q_ASSERT(_data[name].metaType() == value.metaType());
    _data[name] = value;
}

bool Score::isTheWorst() const
{
    Score s;
    return score()==s.score();
}

bool Score::operator <(const Score &score) const
{
    return internal->manager.isStrictlyLess(*this, score);
}

QDataStream &operator <<(QDataStream &s, const Score &score)
{
    s << (quint8)score.type();
    s << score._data;
    return s;
}

QDataStream &operator >>(QDataStream &s, Score &score)
{
    quint8 type;
    s >> type;
    score._type = (ScoreType)type;
    s >> score._data;
    return s;
}

} // namespace

/*
    SPDX-FileCopyrightText: 2001-2004 Nicolas Hadacek <hadacek@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KEXTHIGHSCORE_INTERNAL_H
#define KEXTHIGHSCORE_INTERNAL_H

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <KHighscore>

#include "kexthighscore.h"

#include <QDateTime>
#include <QTextStream>
#include <QUrl>
#include <QVector>

class QDomNamedNodeMap;


namespace KExtHighscore
{

class PlayerInfos;
class Score;
class Manager;


//-----------------------------------------------------------------------------
class RankItem : public Item
{
 public:
    RankItem()
        : Item((uint)0, i18n("Rank"), Qt::AlignRight) {}

    QVariant read(uint i, const QVariant &value) const  override { Q_UNUSED(value); return i; }
    QString pretty(uint i, const QVariant &value) const override
        { Q_UNUSED(value); return QString::number(i+1); }
};

class NameItem : public Item
{
 public:
    NameItem()
        : Item(QString(), i18n("Name"), Qt::AlignLeft) {
            setPrettySpecial(Anonymous);
    }
};

class DateItem : public Item
{
 public:
    DateItem()
        : Item(QDateTime(), i18n("Date"), Qt::AlignRight) {
            setPrettyFormat(DateTime);
    }
};

class SuccessPercentageItem : public Item
{
 public:
    SuccessPercentageItem()
        : Item((double)-1, i18n("Success"), Qt::AlignRight) {
            setPrettyFormat(Percentage);
            setPrettySpecial(NegativeNotDefined);
    }
};

//-----------------------------------------------------------------------------
class ItemContainer
{
 public:
    ItemContainer();
    ~ItemContainer();

    void setItem(Item *item);
    const Item *item() const { return _item; }
    Item *item() { return _item; }

    void setName(const QString &name) { _name = name; }
    QString name() const { return _name; }

    void setGroup(const QString &group) { _group = group; }
    bool isStored() const { return !_group.isNull(); }

    void setSubGroup(const QString &subGroup) { _subGroup = subGroup; }
    bool canHaveSubGroup() const { return !_subGroup.isNull(); }

    static const char ANONYMOUS[]; // name assigned to anonymous players
    static const char ANONYMOUS_LABEL[];

    QVariant read(uint i) const;
    QString pretty(uint i) const;
    void write(uint i, const QVariant &value) const;
    // for UInt QVariant (return new value)
    uint increment(uint i) const;

 private:
    Item    *_item;
    QString  _name, _group, _subGroup;

    QString entryName() const;

    ItemContainer(const ItemContainer &);
    ItemContainer &operator =(const ItemContainer &);
};

//-----------------------------------------------------------------------------
/**
 * Manage a bunch of @ref Item which are saved under the same group
 * in KHighscores config file.
 */
class ItemArray : public QVector<ItemContainer *>
{
 public:
    ItemArray();
    virtual ~ItemArray();

    virtual uint nbEntries() const = 0;

    const ItemContainer *item(const QString &name) const;
    ItemContainer *item(const QString &name);

    void addItem(const QString &name, Item *, bool stored = true,
                 bool canHaveSubGroup = false);
    void setItem(const QString &name, Item *);
    int findIndex(const QString &name) const;

    void setGroup(const QString &group);
    void setSubGroup(const QString &subGroup);

    void read(uint k, Score &data) const;
    void write(uint k, const Score &data, uint maxNbLines) const;

    void exportToText(QTextStream &) const;

 private:
    QString _group, _subGroup;

    void _setItem(uint i, const QString &name, Item *, bool stored,
                  bool canHaveSubGroup);

    ItemArray(const ItemArray &);
    ItemArray &operator =(const ItemArray &);
};

//-----------------------------------------------------------------------------
class ScoreInfos : public ItemArray
{
 public:
    ScoreInfos(uint maxNbEntries, const PlayerInfos &infos);

    uint nbEntries() const override;
    uint maxNbEntries() const { return _maxNbEntries; }

 private:
    uint _maxNbEntries;
};

//-----------------------------------------------------------------------------
class ConfigGroup : public KConfigGroup
{
 public:
    explicit ConfigGroup(const QString &group = QLatin1String( "" ))
        : KConfigGroup(KSharedConfig::openConfig(), group) {}
};

//-----------------------------------------------------------------------------
class PlayerInfos : public ItemArray
{
 public:
    PlayerInfos();

    bool isNewPlayer() const { return _newPlayer; }
    bool isOldLocalPlayer() const { return _oldLocalPlayer; }
    uint nbEntries() const override;
    QString name() const { return item(QStringLiteral( "name" ))->read(_id).toString(); }
    bool isAnonymous() const;
    QString prettyName() const { return prettyName(_id); }
    QString prettyName(uint id) const { return item(QStringLiteral( "name" ))->pretty(id); }
    QString registeredName() const;
    QString comment() const { return item(QStringLiteral( "comment" ))->pretty(_id); }
    bool isWWEnabled() const;
    QString key() const;
    uint id() const { return _id; }
    uint oldLocalId() const { return _oldLocalId; }

    void createHistoItems(const QVector<uint> &scores, bool bound);
    QString histoName(int i) const;
    int histoSize() const;
    const QVector<uint> &histogram() const { return _histogram; }

    void submitScore(const Score &) const;
    // return true if the nickname is already used locally
    bool isNameUsed(const QString &name) const;
    void modifyName(const QString &newName) const;
    void modifySettings(const QString &newName, const QString &comment,
                        bool WWEnabled, const QString &newKey) const;
    void removeKey();

 private:
    bool _newPlayer, _bound, _oldLocalPlayer;
    uint _id, _oldLocalId;
    QVector<uint> _histogram;
};

//-----------------------------------------------------------------------------
class ManagerPrivate
{
 public:
    ManagerPrivate(uint nbGameTypes, Manager &manager);
    void init(uint maxNbentries);
    ~ManagerPrivate();

    bool modifySettings(const QString &newName, const QString &comment,
                        bool WWEnabled, QWidget *widget);

    void setGameType(uint type);
    void checkFirst();
    int submitLocal(const Score &score);
    int submitScore(const Score &score, QWidget *widget, bool askIfAnonymous);
    Score readScore(uint i) const;

    uint gameType() const        { return _gameType; }
    uint nbGameTypes() const     { return _nbGameTypes; }
    bool isWWHSAvailable() const { return !serverURL.isEmpty(); }
    ScoreInfos &scoreInfos()     { return *_scoreInfos; }
    PlayerInfos &playerInfos()   { return *_playerInfos; }
    KHighscore &hsConfig()       { return *_hsConfig; }
    enum QueryType { Submit, Register, Change, Players, Scores };
    QUrl queryUrl(QueryType type, const QString &newName = QLatin1String("")) const;

    void exportHighscores(QTextStream &);

    Manager &manager;
    QUrl     serverURL;
    QString  version;
    bool     showStatistics, showDrawGames, trackLostGames, trackDrawGames;
    Manager::ShowMode showMode;

 private:
    KHighscore   *_hsConfig;
    PlayerInfos  *_playerInfos;
    ScoreInfos   *_scoreInfos;
    bool          _first;
    const uint    _nbGameTypes;
    uint          _gameType;

    // return -1 if not a local best score
    int rank(const Score &score) const;

    bool submitWorldWide(const Score &score, QWidget *parent) const;
    static bool doQuery(const QUrl &url, QWidget *parent,
                        QDomNamedNodeMap *map = nullptr);
    static bool getFromQuery(const QDomNamedNodeMap &map, const QString &name,
                             QString &value, QWidget *parent);
    void convertToGlobal();
};

} // namespace

#endif

/*
    SPDX-FileCopyrightText: 2001-2003 Nicolas Hadacek <hadacek@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KEXTHIGHSCORE_ITEM_H
#define KEXTHIGHSCORE_ITEM_H

#include <QMap>
#include <QVariant>
#include <QVector>
#include <libkdegames_export.h>
class QWidget;


namespace KExtHighscore
{

//-----------------------------------------------------------------------------
/**
 * This class defines how to convert and how to display
 * a highscore element (such as the score, the date, ...) or a player
 * info (such as the player name, the best score, ...).
 */
class  Item
{
 public:
    /**
     * Possible display format.
     * <ul>
     * <li> @p NoFormat : no formatting (default) </li>
     * <li> @p OneDecimal : with one decimal (only for Double) </li>
     * <li> @p Percentage : with one decimal + % (only for Double) </li>
     * <li> @p MinuteTime : MM:SS ie 3600 is 00:00, 1 is 59:59 and 0 is
     *      undefined (only for UInt, Int and Double) </li>
     * <li> @p DateTime : date and time according to locale (only for
     *      DateTime) </li>
     * </ul>
     */
    enum Format { NoFormat, OneDecimal, Percentage, MinuteTime,
		          DateTime };

    /**
     * Possible special value for display format.
     * <ul>
     * <li> @p NoSpecial : no special value ; a null DateTime is replaced by
     *      "--" (default) </li>
     * <li> ZeroNotDefined : 0 is replaced by "--" (only for UInt, Int and
     *      Double) </li>
     * <li> @p NegativeNotDefined : negative values are replaced by "--" (only
     *      for Int and Double) </li>
     * <li> @p DefaultNotDefined : default value is replaced by "--" </li>
     * <li> @p Anonymous : replace the special value ItemBase::ANONYMOUS
     *      by i18n("anonymous") (only for String) </li>
     * </ul>
     */
    enum Special { NoSpecial, ZeroNotDefined, NegativeNotDefined,
                   DefaultNotDefined, Anonymous };

    /**
     * Constructor.
     *
     * @param def default value ; the QVariant also gives the type of data.
     * Be sure to cast the value to the required type (for e.g. with uint).
     * @param label the label corresponding to the item. If empty, the item
     *              is not shown.
     * @param alignment the alignment of the item.
     */
    explicit Item(const QVariant &def = QVariant::Invalid,
         const QString &label = QString(), Qt::AlignmentFlag alignment = Qt::AlignRight);

    virtual ~Item();

    /**
     * Set the display format.
     * @see Format
     */
    void setPrettyFormat(Format format);

    /**
     * Set the special value for display.
     * @see Special
     */
    void setPrettySpecial(Special special);

    /**
     * @return if the item is shown.
     */
    bool isVisible() const    { return !_label.isEmpty(); }

    /**
     * Set the label.
     */
    void setLabel(const QString &label) { _label = label; }

    /**
     * @return the label.
     */
    QString label() const { return _label; }

    /**
     * @return the alignment.
     */
    int alignment() const { return _alignment; }

    /**
     * Set default value.
     */
    void setDefaultValue(const QVariant &value) { _default = value; }

    /**
     * @return the default value.
     */
    QVariant defaultValue() const { return _default; }

    /**
     * @return the converted value (by default the value is left
     * unchanged). Most of the time you don't need to reimplement this method.
     *
     * @param i the element index ("rank" for score / "id" for player)
     * @param value the value to convert
     */
    virtual QVariant read(uint i, const QVariant &value) const;

    /**
     * @return the string to be displayed. You may need to reimplement this
     * method for special formatting (different from the standard ones).
     *
     * @param i the element index ("rank" for score / "id" for player)
     * @param value the value to convert
     */
    virtual QString pretty(uint i, const QVariant &value) const;

 private:
    QVariant _default;
    QString  _label;
    Qt::AlignmentFlag   _alignment;
    Format   _format;
    Special  _special;

    class ItemPrivate;
    ItemPrivate *d;

    static QString timeFormat(uint);
};

//-----------------------------------------------------------------------------
/**
 * Possible score type.
 * @p Won the game has been won.
 * @p Lost the game has been lost or has been aborted.
 * @p Draw the game is a draw.
 */
enum ScoreType { Won = 0, Lost = -1, Draw = -2 };

class Score;
 QDataStream &operator <<(QDataStream &stream, const Score &score);
 QDataStream &operator >>(QDataStream &stream, Score &score);

/**
 * This class contains data for a score. You should not inherit from
 * this class but reimplement the methods in Highscores.
 */
class  Score
{
 public:
    explicit Score(ScoreType type = Won);

    ~Score();

    /**
     * @return the game type.
     */
    ScoreType type() const { return _type; }

    /**
     * Set the game type.
     */
    void setType(ScoreType type) { _type = type; }

    /**
     * @return the data associated with the named Item.
     */
    QVariant data(const QString &name) const;

    /**
     * Set the data associated with the named Item. Note that the
     * value should have the type of the default value of the
     * Item.
     */
    void setData(const QString &name, const QVariant &value);

    /**
     * @return the score value.
     *
     * Equivalent to <pre>data("score").toUInt()</pre>.
     */
    uint score() const { return data(QStringLiteral("score")).toUInt(); }

    /**
     * Set the score value.
     *
     * Equivalent to <pre>setData("score", score)</pre>.
     */
    void setScore(uint score) { setData(QStringLiteral("score"), score); }

    /**
     * @return true if this is the worst possible score (ie the default
     * argument of ScoreItem).
     */
    bool isTheWorst() const;

    /**
     * Comparison operator.
     *
     * @see Manager::isStrictlyLess
     */
    bool operator <(const Score &score) const;

 private:
    ScoreType  _type;
    QMap<QString, QVariant> _data;

    class ScorePrivate;
    ScorePrivate *d;

    friend class MultiplayerScores;

    friend  QDataStream &operator <<(QDataStream &stream, const Score &score);
    friend  QDataStream &operator >>(QDataStream &stream, Score &score);
};

class MultiplayerScores;
 QDataStream &operator <<(QDataStream &stream, const MultiplayerScores &score);
 QDataStream &operator >>(QDataStream &stream, MultiplayerScores &score);

/**
 * This class is used to store and show scores for multiplayer games.
 *
 * Example of use:
 * Initialize the class:
 * <pre>
 * KExtHighscore::MultiScore ms(2);
 * ms.setPlayerName(0, "player 1");
 * ms.setPlayerName(1, "player 2");
 * </pre>
 * At the end of each game, add the score of each players:
 * <pre>
 * KExtHighscore::Score score(KExtHighscore::Won);
 * score.setScore(100);
 * ms.addScore(0, score);
 * score.setType(KExtHighscore::Lost);
 * score.setScore(20);
 * ms.addScore(1, score);
 * </pre>
 */
class  MultiplayerScores
{
 public:
    MultiplayerScores();

    ~MultiplayerScores();

    /**
     * Set the number of players and clear the scores.
     */
    void setPlayerCount(uint nb);

    /**
     * Set the name of player.
     */
    void setName(uint player, const QString &name);

    /**
     * Add the score of player.
     */
    void addScore(uint player, const Score &score);

    /**
     * Clear all scores.
     */
    void clear();

    /**
     * Show scores.
     */
    void show(QWidget *parent);

 private:
    QVector<uint>  _nbGames;
    QVector<Score> _scores;

    class MultiplayerScoresPrivate;
    MultiplayerScoresPrivate *d;

    friend  QDataStream &operator <<(QDataStream &stream,
                                    const MultiplayerScores &score);
    friend  QDataStream &operator >>(QDataStream &stream,
                                    MultiplayerScores &score);
};

} // namespace

#endif

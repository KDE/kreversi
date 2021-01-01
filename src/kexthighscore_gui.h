/*
    SPDX-FileCopyrightText: 2001-02 Nicolas Hadacek <hadacek@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KEXTHIGHSCORE_GUI_H
#define KEXTHIGHSCORE_GUI_H

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QList>
#include <QTreeWidget>

#include <KPageDialog>

#include "kexthighscore.h"

class QTabWidget;
class KUrlLabel;

namespace KExtHighscore
{

class ItemContainer;
class ItemArray;
class Score;
class AdditionalTab;

//-----------------------------------------------------------------------------
class ShowItem : public QTreeWidgetItem
{
 public:
    ShowItem(QTreeWidget *, bool highlight);

 private:
    bool _highlight;
};

class ScoresList : public QTreeWidget
{
 Q_OBJECT
 public:
    explicit ScoresList(QWidget *parent);

    void addHeader(const ItemArray &);

 protected:
    QTreeWidgetItem *addLine(const ItemArray &, uint index, bool highlight);
    virtual QString itemText(const ItemContainer &, uint row) const = 0;

 private:
    virtual void addLineItem(const ItemArray &, uint index,
                             QTreeWidgetItem *item);
};

//-----------------------------------------------------------------------------
class HighscoresList : public ScoresList
{
 Q_OBJECT
 public:
    explicit HighscoresList(QWidget *parent);

    void load(const ItemArray &, int highlight);

 protected:
    QString itemText(const ItemContainer &, uint row) const override;
};

class HighscoresWidget : public QWidget
{
 Q_OBJECT
 public:
    explicit HighscoresWidget(QWidget *parent);

    void load(int rank);

 Q_SIGNALS:
    void tabChanged(int i);

 public Q_SLOTS:
    void changeTab(int i);

 private Q_SLOTS:
    void handleUrlClicked();
    void handleTabChanged() { Q_EMIT tabChanged(_tw->currentIndex()); }

 private:
    QTabWidget     *_tw = nullptr;
    HighscoresList *_scoresList = nullptr;
    HighscoresList *_playersList = nullptr;
    KUrlLabel      *_scoresUrl = nullptr;
    KUrlLabel *_playersUrl = nullptr;
    AdditionalTab  *_statsTab = nullptr;
    AdditionalTab *_histoTab = nullptr;
};

class HighscoresDialog : public KPageDialog
{
 Q_OBJECT
 public:
    HighscoresDialog(int rank, QWidget *parent);

 private Q_SLOTS:
    void slotUser1();
    void slotUser2();
    void tabChanged(int i) { _tab = i; }
    void highscorePageChanged(KPageWidgetItem *newpage, KPageWidgetItem *before);

 private:
    int _rank, _tab;
    QWidget *_current = nullptr;
    QList<KPageWidgetItem*> _pages;
};

//-----------------------------------------------------------------------------
class LastMultipleScoresList : public ScoresList
{
    Q_OBJECT
public:
    LastMultipleScoresList(const QVector<Score> &, QWidget *parent);

private:
    void addLineItem(const ItemArray &, uint index, QTreeWidgetItem *line) override;
    QString itemText(const ItemContainer &, uint row) const override;

private:
    const QVector<Score> &_scores;
};

class TotalMultipleScoresList : public ScoresList
{
    Q_OBJECT
public:
    TotalMultipleScoresList(const QVector<Score> &, QWidget *parent);

private:
    void addLineItem(const ItemArray &, uint index, QTreeWidgetItem *line) override;
    QString itemText(const ItemContainer &, uint row) const override;

private:
    const QVector<Score> &_scores;
};

//-----------------------------------------------------------------------------
class ConfigDialog : public QDialog
{
 Q_OBJECT
 public:
    explicit ConfigDialog(QWidget *parent);

    bool hasBeenSaved() const { return _saved; }

 private Q_SLOTS:
    void modifiedSlot();
    void removeSlot();
    void accept() override;
    void slotApply() { save(); }
    void nickNameChanged(const QString &);

 private:
    bool         _saved;
    QCheckBox   *_WWHEnabled = nullptr;
    QLineEdit   *_nickname = nullptr;
    QLineEdit *_comment = nullptr;
    QLineEdit   *_key = nullptr;
    QLineEdit *_registeredName = nullptr;
    QPushButton *_removeButton = nullptr;
    QDialogButtonBox *buttonBox = nullptr;

    void load();
    bool save();
};

//-----------------------------------------------------------------------------
class AskNameDialog : public QDialog
{
 Q_OBJECT
 public:
    explicit AskNameDialog(QWidget *parent);

    QString name() const { return _edit->text(); }
    bool dontAskAgain() const { return _checkbox->isChecked(); }

 private Q_SLOTS:
    void nameChanged();

 private:
    QDialogButtonBox *_buttonBox = nullptr;
    QLineEdit *_edit = nullptr;
    QCheckBox *_checkbox = nullptr;
};

} // namespace

#endif

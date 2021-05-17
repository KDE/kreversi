/*
    SPDX-FileCopyrightText: 2001-2003 Nicolas Hadacek <hadacek@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "kexthighscore_gui.h"

#include <QApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QIcon>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include <QTemporaryFile>
#include <QTextStream>
#include <QVBoxLayout>

#include <KIO/OpenUrlJob>
#include <KIO/JobUiDelegate>
#include <KGuiItem>
#include <kio_version.h>
#include <KIO/StatJob>
#include <KIO/CopyJob>
#include <KJobWidgets>
#include <KMessageBox>
#include <KUrlLabel>

#include "kexthighscore_internal.h"
#include "kexthighscore_tab.h"


namespace KExtHighscore
{

//-----------------------------------------------------------------------------
ShowItem::ShowItem(QTreeWidget *list, bool highlight)
    : QTreeWidgetItem(list), _highlight(highlight)
{
//   kDebug(11001) ;
  if (_highlight) {
    for (int i=0; i < columnCount();i++) {
      setForeground(i, Qt::red);
    }
  }
}

//-----------------------------------------------------------------------------
ScoresList::ScoresList(QWidget *parent)
    : QTreeWidget(parent)
{
//   kDebug(11001) ;
    setSelectionMode(QTreeWidget::NoSelection);
//     setItemMargin(3);
    setAllColumnsShowFocus(true);
//     setSorting(-1);
    header()->setSectionsClickable(false);
    header()->setSectionsMovable(false);
}

void ScoresList::addHeader(const ItemArray &items)
{
//   kDebug(11001) ;
    addLineItem(items, 0, nullptr);
}

QTreeWidgetItem *ScoresList::addLine(const ItemArray &items,
                                   uint index, bool highlight)
{
//   kDebug(11001) ;
    QTreeWidgetItem *item = new ShowItem(this, highlight);
    addLineItem(items, index, item);
    return item;
}

void ScoresList::addLineItem(const ItemArray &items,
                             uint index, QTreeWidgetItem *line)
{
//   kDebug(11001) ;
    uint k = 0;
    for (int i=0; i<items.size(); i++) {
        const ItemContainer& container = *items[i];
        if ( !container.item()->isVisible() ) {
	  continue;
	}
        if (line) {
	  line->setText(k, itemText(container, index));
	  line->setTextAlignment(k, container.item()->alignment());
	}
        else {
	    headerItem()->setText(k, container.item()->label() );
            headerItem()->setTextAlignment(k, container.item()->alignment());
        }
        k++;
    }
    update();
}

//-----------------------------------------------------------------------------
HighscoresList::HighscoresList(QWidget *parent)
    : ScoresList(parent)
{
//   kDebug(11001) ;
}

QString HighscoresList::itemText(const ItemContainer &item, uint row) const
{
//   kDebug(11001) ;
    return item.pretty(row);
}

void HighscoresList::load(const ItemArray &items, int highlight)
{
//     kDebug(11001) ;
    clear();
    QTreeWidgetItem *line = nullptr;
    for (int j=items.nbEntries()-1; j>=0; j--) {
        QTreeWidgetItem *item = addLine(items, j, j==highlight);
        if ( j==highlight ) line = item;
    }
    scrollTo(indexFromItem(line));
}

//-----------------------------------------------------------------------------
HighscoresWidget::HighscoresWidget(QWidget *parent)
    : QWidget(parent)
{
//     kDebug(11001) << ": HighscoresWidget";

    setObjectName( QStringLiteral("show_highscores_widget" ));
    const ScoreInfos &s = internal->scoreInfos();
    const PlayerInfos &p = internal->playerInfos();

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setSpacing(QApplication::fontMetrics().lineSpacing());

    _tw = new QTabWidget(this);
    connect(_tw, &QTabWidget::currentChanged, this, &HighscoresWidget::handleTabChanged);
    vbox->addWidget(_tw);

    // scores tab
    _scoresList = new HighscoresList(nullptr);
    _scoresList->addHeader(s);
    _tw->addTab(_scoresList, i18n("Best &Scores"));

    // players tab
    _playersList = new HighscoresList(nullptr);
    _playersList->addHeader(p);
    _tw->addTab(_playersList, i18n("&Players"));

    // statistics tab
    if ( internal->showStatistics ) {
        _statsTab = new StatisticsTab(nullptr);
        _tw->addTab(_statsTab, i18n("Statistics"));
    }

    // histogram tab
    if ( p.histogram().size()!=0 ) {
        _histoTab = new HistogramTab(nullptr);
        _tw->addTab(_histoTab, i18n("Histogram"));
    }

    // url labels
    if ( internal->isWWHSAvailable() ) {
        QUrl url = internal->queryUrl(ManagerPrivate::Scores);
        _scoresUrl = new KUrlLabel(url.url(),
                                   i18n("View world-wide highscores"), this);
        connect(_scoresUrl, QOverload<>::of(&KUrlLabel::leftClickedUrl),
                this, &HighscoresWidget::handleUrlClicked);
        vbox->addWidget(_scoresUrl);

        url = internal->queryUrl(ManagerPrivate::Players);
        _playersUrl = new KUrlLabel(url.url(),
                                    i18n("View world-wide players"), this);
        connect(_playersUrl, QOverload<>::of(&KUrlLabel::leftClickedUrl),
                this, &HighscoresWidget::handleUrlClicked);
        vbox->addWidget(_playersUrl);
    }
    load(-1);
}

void HighscoresWidget::changeTab(int i)
{
//   kDebug(11001) ;
    if ( i!=_tw->currentIndex() )
        _tw->setCurrentIndex(i);
}

void HighscoresWidget::handleUrlClicked()
{
    auto* label = qobject_cast<KUrlLabel*>(sender());
    if (!label) {
        return;
    }
//   kDebug(11001) ;
    auto *job = new KIO::OpenUrlJob(QUrl(label->url()));
    job->setUiDelegate(new KIO::JobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, this));
    job->start();
}

void HighscoresWidget::load(int rank)
{
//   kDebug(11001) << rank;
    _scoresList->load(internal->scoreInfos(), rank);
    _playersList->load(internal->playerInfos(), internal->playerInfos().id());
    if (_scoresUrl)
        _scoresUrl->setUrl(internal->queryUrl(ManagerPrivate::Scores).url());
    if (_playersUrl)
        _playersUrl->setUrl(internal->queryUrl(ManagerPrivate::Players).url());
    if (_statsTab) _statsTab->load();
    if (_histoTab) _histoTab->load();
}

//-----------------------------------------------------------------------------
HighscoresDialog::HighscoresDialog(int rank, QWidget *parent)
    : KPageDialog(parent), _rank(rank), _tab(0)
{
//     kDebug(11001) << ": HighscoresDialog";

    setWindowTitle( i18n("Highscores") );
// TODO    setButtons( Close|User1|User2 );
// TODO    setDefaultButton( Close );
    if ( internal->nbGameTypes()>1 )
        setFaceType( KPageDialog::Tree );
    else
        setFaceType( KPageDialog::Plain );
// TODO    setButtonGuiItem( User1, KGuiItem(i18n("Configure..."), QLatin1String( "configure" )) );
// TODO    setButtonGuiItem( User2, KGuiItem(i18n("Export...")) );

    for (uint i=0; i<internal->nbGameTypes(); i++) {
        QString title = internal->manager.gameTypeLabel(i, Manager::I18N);
        QString icon = internal->manager.gameTypeLabel(i, Manager::Icon);
        HighscoresWidget *hsw = new HighscoresWidget(nullptr);
        KPageWidgetItem *pageItem = new KPageWidgetItem( hsw, title);
        const int iconSize = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
        pageItem->setIcon(QIcon::fromTheme(icon).pixmap(iconSize));
        addPage( pageItem );
        _pages.append(pageItem);
        connect(hsw, &HighscoresWidget::tabChanged, this, &HighscoresDialog::tabChanged);
    }

    connect(this, &KPageDialog::currentPageChanged,
            this, &HighscoresDialog::highscorePageChanged);
    setCurrentPage(_pages[internal->gameType()]);

    setStandardButtons(QDialogButtonBox::Close);
}

void HighscoresDialog::highscorePageChanged(KPageWidgetItem* page, KPageWidgetItem* pageold)
{
    Q_UNUSED(pageold);
//   kDebug(11001) ;
    int idx = _pages.indexOf( page );
    Q_ASSERT(idx != -1);

    internal->hsConfig().readCurrentConfig();
    uint type = internal->gameType();
    bool several = ( internal->nbGameTypes()>1 );
    if (several)
        internal->setGameType(idx);
    HighscoresWidget *hsw = static_cast<HighscoresWidget*>(page->widget());
    hsw->load(uint(idx)==type ? _rank : -1);
    if (several) setGameType(type);
    hsw->changeTab(_tab);
}

void HighscoresDialog::slotUser1()
{
//   kDebug(11001) ;
    if ( KExtHighscore::configure(this) )
        highscorePageChanged(currentPage(), nullptr);//update data
}

void HighscoresDialog::slotUser2()
{
//   kDebug(11001) ;
    QUrl url = QFileDialog::getSaveFileUrl(this, tr("HighscoresDialog"), QUrl(), QString());
    if ( url.isEmpty() ) return;
    auto job = KIO::statDetails(url, KIO::StatJob::SourceSide, KIO::StatNoDetails);
    KJobWidgets::setWindow(job, this);
    job->exec();
    if (!job->error()) {
        KGuiItem gi = KStandardGuiItem::save();
        gi.setText(i18n("Overwrite"));
        int res = KMessageBox::warningContinueCancel(this,
                                 i18n("The file already exists. Overwrite?"),
                                 i18n("Export"), gi);
        if ( res==KMessageBox::Cancel ) return;
    }
    QTemporaryFile tmp;
    tmp.open();
    QTextStream stream(&tmp);
    internal->exportHighscores(stream);
    stream.flush();
//     KIO::NetAccess::upload(tmp.fileName(), url, this);
    auto copyJob = KIO::copy(QUrl::fromLocalFile(tmp.fileName()), url);
    copyJob->exec();
}

//-----------------------------------------------------------------------------
LastMultipleScoresList::LastMultipleScoresList(
                            const QVector<Score> &scores, QWidget *parent)
    : ScoresList(parent), _scores(scores)
{
//     kDebug(11001) << ": LastMultipleScoresList";

    const ScoreInfos &s = internal->scoreInfos();
    addHeader(s);
    for (int i=0; i<scores.size(); i++) addLine(s, i, false);
}

void LastMultipleScoresList::addLineItem(const ItemArray &si,
                                         uint index, QTreeWidgetItem *line)
{
//   kDebug(11001) ;
    uint k = 1; // skip "id"
    for (int i=0; i<si.size()-2; i++) {
        if ( i==3 ) k = 5; // skip "date"
        const ItemContainer& container = *si[k];
        k++;
        if (line) {
	  line->setText(i, itemText(container, index));
	  line->setTextAlignment(i, container.item()->alignment());
	}
        else {
	    headerItem()->setText(i, container.item()->label() );
            headerItem()->setTextAlignment(i, container.item()->alignment());
        }
    }
}

QString LastMultipleScoresList::itemText(const ItemContainer &item,
                                         uint row) const
{
//   kDebug(11001) ;
    QString name = item.name();
    if ( name==QLatin1String( "rank" ) )
        return (_scores[row].type()==Won ? i18n("Winner") : QString());
    QVariant v = _scores[row].data(name);
    if ( name==QLatin1String( "name" ) ) return v.toString();
    return item.item()->pretty(row, v);
}

//-----------------------------------------------------------------------------
TotalMultipleScoresList::TotalMultipleScoresList(
                            const QVector<Score> &scores, QWidget *parent)
    : ScoresList(parent), _scores(scores)
{
//     kDebug(11001) << ": TotalMultipleScoresList";
    const ScoreInfos &s = internal->scoreInfos();
    addHeader(s);
    for (int i=0; i<scores.size(); i++) addLine(s, i, false);
}

void TotalMultipleScoresList::addLineItem(const ItemArray &si,
                                          uint index, QTreeWidgetItem *line)
{
//   kDebug(11001) ;
    const PlayerInfos &pi = internal->playerInfos();
    uint k = 1; // skip "id"
    for (uint i=0; i<4; i++) { // skip additional fields
        const ItemContainer *container;
        if ( i==2 ) container = pi.item(QStringLiteral( "nb games" ));
        else if ( i==3 ) container = pi.item(QStringLiteral( "mean score" ));
        else {
            container = si[k];
            k++;
        }

        if (line) {
	  line->setText(i, itemText(*container, index));
	  line->setTextAlignment(i, container->item()->alignment());
	}
        else {
            QString label =
                (i==2 ? i18n("Won Games") : container->item()->label());
	    headerItem()->setText(i, label );
            headerItem()->setTextAlignment(i, container->item()->alignment());
        }
    }
}

QString TotalMultipleScoresList::itemText(const ItemContainer &item,
                                          uint row) const
{
//   kDebug(11001) ;
    QString name = item.name();
    if ( name==QLatin1String( "rank" ) ) return QString::number(_scores.size()-row);
    else if ( name==QLatin1String( "nb games" ) )
        return QString::number( _scores[row].data(QStringLiteral( "nb won games" )).toUInt() );
    QVariant v = _scores[row].data(name);
    if ( name==QLatin1String( "name" ) ) return v.toString();
    return item.item()->pretty(row, v);
}


//-----------------------------------------------------------------------------
ConfigDialog::ConfigDialog(QWidget *parent)
    : QDialog(parent),
      _saved(false), _WWHEnabled(nullptr)
{
//     kDebug(11001) << ": ConfigDialog";
    
    setWindowTitle( i18n("Configure Highscores") );
    setModal( true );
    
    QWidget *page = nullptr;
    QTabWidget *tab = nullptr;
    
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);
    
    if ( internal->isWWHSAvailable() ) {
        tab = new QTabWidget(this);
        layout->addWidget(tab);
        page = new QWidget;
        tab->addTab(page, i18n("Main"));
    } 
    
    else {
        page = new QWidget(this);
        layout->addWidget(page);
    }

    QGridLayout *pageTop =
        new QGridLayout(page);
    //pageTop->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    //pageTop->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    
    layout->addLayout(pageTop);

    QLabel *label = new QLabel(i18n("Nickname:"), page);
    pageTop->addWidget(label, 0, 0);
    _nickname = new QLineEdit(page);
    connect(_nickname, &QLineEdit::textChanged,
            this, &ConfigDialog::modifiedSlot);
    connect(_nickname, &QLineEdit::textChanged,
            this, &ConfigDialog::nickNameChanged);

    _nickname->setMaxLength(16);
    pageTop->addWidget(_nickname, 0, 1);

    label = new QLabel(i18n("Comment:"), page);
    pageTop->addWidget(label, 1, 0);
    _comment = new QLineEdit(page);
    connect(_comment, &QLineEdit::textChanged,
            this, &ConfigDialog::modifiedSlot);
    _comment->setMaxLength(50);
    pageTop->addWidget(_comment, 1, 1);

    if (tab) {
        _WWHEnabled
            = new QCheckBox(i18n("World-wide highscores enabled"), page);
        connect(_WWHEnabled, &QAbstractButton::toggled,
                this, &ConfigDialog::modifiedSlot);
        pageTop->addWidget(_WWHEnabled, 2, 0, 1, 2 );

        // advanced tab
        QWidget *page = new QWidget;
        tab->addTab(page, i18n("Advanced"));
        QVBoxLayout *pageTop = new QVBoxLayout(page);
        //pageTop->setMargin(QApplication::style()->pixelMetric(QStyle::PM_DefaultChildMargin));
        //pageTop->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));

        QGroupBox *group = new QGroupBox(page);
        group->setTitle( i18n("Registration Data") );
        pageTop->addWidget(group);
        QGridLayout *groupLayout = new QGridLayout(group);
        //groupLayout->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));

        label = new QLabel(i18n("Nickname:"), group);
        groupLayout->addWidget(label, 0, 0);
        _registeredName = new QLineEdit(group);
        _registeredName->setReadOnly(true);
        groupLayout->addWidget(_registeredName, 0, 1);

        label = new QLabel(i18n("Key:"), group);
        groupLayout->addWidget(label, 1, 0);
        _key = new QLineEdit(group);
        _key->setReadOnly(true);
        groupLayout->addWidget(_key, 1, 1);

        KGuiItem gi = KStandardGuiItem::clear();
        gi.setText(i18n("Remove"));
        _removeButton = new QPushButton(group);
	KGuiItem::assign(_removeButton, gi);
	groupLayout->addWidget(_removeButton, 2, 0);
        connect(_removeButton, &QAbstractButton::clicked, this, &ConfigDialog::removeSlot);
    }
    
    buttonBox = new QDialogButtonBox(this);
    
    buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel); 
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    // TODO mapping for Apply button
    pageTop->addWidget(buttonBox);

    load();
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled( !_nickname->text().isEmpty() );
    buttonBox->button(QDialogButtonBox::Apply)->setEnabled( false );
    
}

void ConfigDialog::nickNameChanged(const QString &text)
{
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled( !text.isEmpty() );
}


void ConfigDialog::modifiedSlot()
{
    buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true && !_nickname->text().isEmpty() );
}

void ConfigDialog::accept()
{
    if ( save() ) {
        QDialog::accept();
        KSharedConfig::openConfig()->sync(); // safer
    }
}

void ConfigDialog::removeSlot()
{
    KGuiItem gi = KStandardGuiItem::clear();
    gi.setText(i18n("Remove"));
    int res = KMessageBox::warningContinueCancel(this,
                               i18n("This will permanently remove your "
                               "registration key. You will not be able to use "
                               "the currently registered nickname anymore."),
                               QString(), gi);
    if ( res==KMessageBox::Continue ) {
        internal->playerInfos().removeKey();
        _registeredName->clear();
        _key->clear();
        _removeButton->setEnabled(false);
        _WWHEnabled->setChecked(false);
        modifiedSlot();
    }
}

void ConfigDialog::load()
{
    internal->hsConfig().readCurrentConfig();
    const PlayerInfos &infos = internal->playerInfos();
    _nickname->setText(infos.isAnonymous() ? QString() : infos.name());
    _comment->setText(infos.comment());
    if (_WWHEnabled) {
        _WWHEnabled->setChecked(infos.isWWEnabled());
        if ( !infos.key().isEmpty() ) {
            _registeredName->setText(infos.registeredName());
            _registeredName->home(false);
            _key->setText(infos.key());
            _key->home(false);
        }
        _removeButton->setEnabled(!infos.key().isEmpty());
    }
}

bool ConfigDialog::save()
{
    bool enabled = (_WWHEnabled ? _WWHEnabled->isChecked() : false);

    // do not bother the user with "nickname empty" if he has not
    // messed with nickname settings ...
    QString newName = _nickname->text();
    if ( newName.isEmpty() && !internal->playerInfos().isAnonymous()
         && !enabled ) return true;

    if ( newName.isEmpty() ) {
        KMessageBox::sorry(this, i18n("Please choose a non empty nickname."));
        return false;
    }
    if ( internal->playerInfos().isNameUsed(newName) ) {
        KMessageBox::sorry(this, i18n("Nickname already in use. Please "
                                      "choose another one"));
        return false;
    }

    int res =
        internal->modifySettings(newName, _comment->text(), enabled, this);
    if (res) {
        load(); // needed to update view when "apply" is clicked
        buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
    }
    _saved = true;
    return res;
}

//-----------------------------------------------------------------------------
AskNameDialog::AskNameDialog(QWidget *parent)
    : QDialog(parent)
{
//     kDebug(11001) << ": AskNameDialog";

    setWindowTitle( i18n("Enter Your Nickname") );

    internal->hsConfig().readCurrentConfig();
    QVBoxLayout *top = new QVBoxLayout;
    //top->setMargin( QApplication::style()->pixelMetric(QStyle::PM_DefaultChildMargin) );
    //top->setSpacing( QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing) );
    setLayout(top);
        
    QLabel *label =
        new QLabel(i18n("Congratulations, you have won!"), this);
    top->addWidget(label);

    QHBoxLayout *hbox = new QHBoxLayout;
    top->addLayout(hbox);
    label = new QLabel(i18n("Enter your nickname:"), this);
    hbox->addWidget(label);
    _edit = new QLineEdit(this);
    _edit->setFocus();
    connect(_edit, &QLineEdit::textChanged, this, &AskNameDialog::nameChanged);
    hbox->addWidget(_edit);

    //top->addSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));
    _checkbox = new QCheckBox(i18n("Do not ask again."),  this);
    top->addWidget(_checkbox);
    
    _buttonBox = new QDialogButtonBox(this);
    
    _buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    top->addWidget(_buttonBox);

    nameChanged();
}

void AskNameDialog::nameChanged()
{
    _buttonBox->button(QDialogButtonBox::Ok)->setEnabled( !name().isEmpty()
                      && !internal->playerInfos().isNameUsed(name())); 
}

} // namespace

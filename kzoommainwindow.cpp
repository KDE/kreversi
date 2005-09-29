/*
    This file is part of the KDE games library
    Copyright (C) 2004 Nicolas Hadacek (hadacek@kde.org)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/


#include "kzoommainwindow.h"
#include "kzoommainwindow.moc"

#include <kaction.h>
#include <kstdaction.h>
#include <kmenubar.h>
#include <kcmenumngr.h>


KZoomMainWindow::KZoomMainWindow(uint min, uint max, uint step, 
				 const char *name)
  : KMainWindow(0, name), m_zoomStep(step), m_minZoom(min), m_maxZoom(max)
{
  installEventFilter(this);
  
  m_zoomInAction = 
    KStdAction::zoomIn(this, SLOT(zoomIn()), actionCollection());
  m_zoomOutAction =
    KStdAction::zoomOut(this, SLOT(zoomOut()), actionCollection());
  m_menu =
    KStdAction::showMenubar(this, SLOT(toggleMenubar()), actionCollection());
}


void KZoomMainWindow::init(const char *popupName)
{
  // zoom
  setZoom(readZoomSetting());

  // menubar
  m_menu->setChecked( menubarVisibleSetting() );  
  toggleMenubar();
  
  // context popup
  if (popupName) {
    QPopupMenu *popup =
      static_cast<QPopupMenu *>(factory()->container(popupName, this));
    Q_ASSERT(popup);
    if (popup)
      KContextMenuManager::insert(this, popup);
  }
}

void KZoomMainWindow::addWidget(QWidget *widget)
{
  widget->adjustSize();

  QWidget          *tlw = widget->topLevelWidget();
  KZoomMainWindow  *zm = 
    static_cast<KZoomMainWindow *>(tlw->qt_cast("KZoomMainWindow"));

  Q_ASSERT(zm);
  zm->m_widgets.append(widget);
  connect(widget, SIGNAL(destroyed()), zm, SLOT(widgetDestroyed()));
}


void KZoomMainWindow::widgetDestroyed()
{
  m_widgets.remove(static_cast<const QWidget *>(sender()));
}


bool KZoomMainWindow::eventFilter(QObject *o, QEvent *e)
{
  if ( e->type()==QEvent::LayoutHint )
    setFixedSize(minimumSize()); // because K/QMainWindow
                                 // does not manage fixed central widget
                                 // with hidden menubar...
  return KMainWindow::eventFilter(o, e);
}


void KZoomMainWindow::setZoom(uint zoom)
{
  m_zoom = zoom;
  writeZoomSetting(m_zoom);

  QPtrListIterator<QWidget>  it(m_widgets);
  for (; it.current(); ++it)
    (*it)->adjustSize();

  m_zoomOutAction->setEnabled( m_zoom > m_minZoom );
  m_zoomInAction->setEnabled( m_zoom < m_maxZoom );
}


void KZoomMainWindow::zoomIn()
{
  setZoom(m_zoom + m_zoomStep);
}


void KZoomMainWindow::zoomOut()
{
  Q_ASSERT( m_zoom >= m_zoomStep );
  setZoom(m_zoom - m_zoomStep);
}


void KZoomMainWindow::toggleMenubar()
{
  if ( m_menu->isChecked() )
    menuBar()->show();
  else
    menuBar()->hide();
}


bool KZoomMainWindow::queryExit()
{
  writeMenubarVisibleSetting(m_menu->isChecked());

  return KMainWindow::queryExit();
}

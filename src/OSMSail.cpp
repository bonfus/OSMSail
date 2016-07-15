/*
  OSMScout - a Qt backend for libosmscout and libosmscout-map
  Copyright (C) 2010  Tim Teulings

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifdef _SAILFISH_BUILD
#include <sailfishapp.h>
#include <QQmlEngine>
#else
#include <QQmlApplicationEngine>
#endif
// Qt includes
#include <QScopedPointer>
#include <QGuiApplication>
#include <QQuickView>

// Redirect Debug
#include <QDebug>
#include <QtGlobal>
#include <iostream>

// Custom QML objects
#include "MapWidget.h"
#include "SearchLocationModel.h"
#include "RoutingModel.h"

// Application settings
#include "Settings.h"

// Position source
#include "PositionSource.h"

// Application theming
#include "Theme.h"

#include <osmscout/util/Logger.h>



Q_DECLARE_METATYPE(osmscout::TileRef)

static QObject *ThemeProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    Theme *theme = new Theme();

    return theme;
}




void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "=> Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
//    case QtInfoMsg:
//        fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
//        break;
    case QtWarningMsg:
        fprintf(stderr, "=> Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "=> Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "=> Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

int main(int argc, char* argv[])
{
  qInstallMessageHandler(myMessageOutput);
#ifdef Q_WS_X11
  QCoreApplication::setAttribute(Qt::AA_X11InitThreads);
#endif

#ifdef _SAILFISH_BUILD
  QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
  QScopedPointer<QQuickView> view(SailfishApp::createView());
#else  
  QScopedPointer<QGuiApplication> app(argc,argv);
#endif
  SettingsRef     settings;
  PositionSourceRef     position;
  int             result;

  app->setOrganizationName("libosmscout");
  app->setOrganizationDomain("libosmscout.sf.net");
  app->setApplicationName("OSMSailer");

  qRegisterMetaType<RenderMapRequest>();
  qRegisterMetaType<DatabaseLoadedResponse>();
  qRegisterMetaType<osmscout::TileRef>();

  qmlRegisterType<MapWidget>("net.sf.libosmscout.map", 1, 0, "Map");
  qmlRegisterType<Location>("net.sf.libosmscout.map", 1, 0, "Location");
  qmlRegisterType<LocationListModel>("net.sf.libosmscout.map", 1, 0, "LocationListModel");
  qmlRegisterType<RouteStep>("net.sf.libosmscout.map", 1, 0, "RouteStep");
  qmlRegisterType<RoutingListModel>("net.sf.libosmscout.map", 1, 0, "RoutingListModel");

  qmlRegisterSingletonType<Theme>("net.sf.libosmscout.map", 1, 0, "Theme", ThemeProvider);

  osmscout::log.Debug(true);



  QThread thread;

  if (!DBThread::InitializeInstance()) {
    std::cerr << "Cannot initialize DBThread" << std::endl;
    return 1;
  }

  settings=std::make_shared<Settings>();
  position=std::make_shared<PositionSource>();

  DBThread* dbThread=DBThread::GetInstance();

  dbThread->connect(&thread, SIGNAL(started()), SLOT(Initialize()));
  dbThread->connect(&thread, SIGNAL(finished()), SLOT(Finalize()));

  dbThread->moveToThread(&thread);


#ifdef _SAILFISH_BUILD
  // Start the application.
  view->setSource(SailfishApp::pathTo("qml/OSMSail.qml"));

  thread.start();
  view->show();
  result = app->exec();
  
#else
  QQmlApplicationEngine window(QUrl("qrc:/qml/main.qml"));
  thread.start();
  result=app->exec();
#endif

  thread.quit();
  thread.wait();

  DBThread::FreeInstance();

  return result;
}

#ifndef MAPWIDGET_H
#define MAPWIDGET_H

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

#include <QQuickPaintedItem>

#include <osmscout/GeoCoord.h>
#include <osmscout/util/GeoBox.h>

#include "DBThread.h"
#include "SearchLocationModel.h"

class MapWidget : public QQuickPaintedItem
{
  Q_OBJECT
  Q_PROPERTY(double lat READ GetLat)
  Q_PROPERTY(double lon READ GetLon)

//protected:
//  void componentComplete();
  //bool childMouseEventFilter(QQuickItem * item, QEvent * event);

private:
  osmscout::GeoCoord           center;
  double                       angle;
  osmscout::Magnification      magnification;

  // Drag and drop
  int                          startX;
  int                          startY;
  double                       startAngle;
  double                       startMagnification;
  osmscout::MercatorProjection startProjection;

  // Controlling rerendering...
  bool                         mouseDragging;
  bool                         dbInitialized;
  bool                         hasBeenPainted;

signals:
  void TriggerMapRenderingSignal(const RenderMapRequest& request);
  void latChanged();
  void lonChanged();


public slots:
  void initialisationFinished(const DatabaseLoadedResponse& response);
  void redraw();
  void zoomIn(double zoomFactor);
  void zoomOut(double zoomFactor);
  void zoomInPos(int dx, int dy, double zoomFactor );
  void left();
  void right();
  void up();
  void down();
  void rotateLeft();
  void rotateRight();

  void toggleDaylight();
  void reloadStyle();

  void showCoordinates(double lat, double lon);
  void showLocation(Location* location);

// pinch
  void handlePinchStart();
  void handlePinchUpdate(QPointF actualCenter, QPointF startCenter, qreal scale, qreal pangle);
  void handlePinchEnd(QPointF actualCenter, QPointF startCenter, qreal scale, qreal pangle);

// mousearea
//  void qmlMouseAreaEvent(int x, int y, int eventType);
//  void qmlMouseMove(QPointF center);
//  void qmlMouseReleased(QPointF center);
  
private:
  void TriggerMapRendering();

  void HandleMouseMove(int event_x, int event_y);
//pinch  
  void makePinch(QPoint actualCenter, QPoint startCenter, qreal scale, qreal pangle);

public:
  MapWidget(QQuickItem* parent = 0);
  virtual ~MapWidget();

  inline double GetLat() const
  {
      return center.GetLat();
  }

  inline double GetLon() const
  {
      return center.GetLon();
  }

  void mousePressEvent(QMouseEvent* event);
  Q_INVOKABLE void mousePressEvent(int event_x, int event_y);
  void mouseMoveEvent(QMouseEvent* event);
  Q_INVOKABLE void mouseMoveEvent(int event_x, int event_y);
  void mouseReleaseEvent(QMouseEvent* event);
  Q_INVOKABLE void mouseReleaseEvent(int event_x, int event_y);
  void wheelEvent(QWheelEvent* event);

  void paint(QPainter *painter);
};

#endif

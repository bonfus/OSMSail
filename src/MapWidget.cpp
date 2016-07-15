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

#include "MapWidget.h"

#include <iostream>

//! We rotate in 16 steps
static double DELTA_ANGLE=2*M_PI/16.0;

MapWidget::MapWidget(QQuickItem* parent)
    : QQuickPaintedItem(parent),
      center(0.0,0.0),
      angle(0.0),
      magnification(64),
      mouseDragging(false),
      dbInitialized(false),
      hasBeenPainted(false)

{
    std::cout << "MapWidget CONSTRUCTOR!" << std::endl;
    setOpaquePainting(true);
    setAcceptedMouseButtons(Qt::LeftButton);

    DBThread *dbThread=DBThread::GetInstance();
    //setFocusPolicy(Qt::StrongFocus);

    connect(dbThread,SIGNAL(InitialisationFinished(DatabaseLoadedResponse)),
            this,SLOT(initialisationFinished(DatabaseLoadedResponse)));

    connect(this,SIGNAL(TriggerMapRenderingSignal(RenderMapRequest)),
            dbThread,SLOT(TriggerMapRendering(RenderMapRequest)));

    connect(dbThread,SIGNAL(HandleMapRenderingResult()),
            this,SLOT(redraw()));

    connect(dbThread,SIGNAL(Redraw()),
            this,SLOT(redraw()));
            
    //setFiltersChildMouseEvents(true);
    //setAcceptedMouseButtons(Qt::LeftButton);            
}

MapWidget::~MapWidget()
{
    // no code
}

void MapWidget::redraw()
{
    update();
}



//void MapWidget::componentComplete()
//{
//    //setFiltersChildMouseEvents(true);
//    QQuickItem::componentComplete();
//    QQuickItem *pinch = findChild<QQuickItem*>("pinchArea");
//	if (pinch)
//	{
//		//pinch->setFiltersChildMouseEvents(childMouseEventFilter());
//        connect(pinch,SIGNAL(pinchStarted()),
//            this,SLOT(handlePinchStart()));
//    }
//}

//bool MapWidget::childMouseEventFilter(QQuickItem * item, QEvent * event)
//{
//    switch (event->type()) {
//        case QEvent::MouseButtonPress: {
//            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
//            mousePressEvent(mouseEvent);
//            std::cout << "mouse press" << std::endl;
//            break;
//        }
//        case QEvent::MouseMove: {
//            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
//            mouseMoveEvent(mouseEvent);
//            std::cout << "mouse move" << std::endl;
//            break;
//        }
//        case QEvent::UngrabMouse:
//            std::cout << "mouse ungrab" << std::endl;
//            break;
//        case QEvent::MouseButtonRelease: {
//            std::cout << "mouse release" << std::endl;
//            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
//            mouseReleaseEvent(mouseEvent);
//            break;
//        }
//        default:
//            break;
//    }
//    return false;
//}

void MapWidget::handlePinchStart()
{
    DBThread *dbThread=DBThread::GetInstance();

    dbThread->GetProjection(startProjection);
    
    startAngle = angle;
    startMagnification = magnification.GetMagnification();
    
    setFocus(true);
    mouseDragging=true;    
}

void MapWidget::handlePinchUpdate(QPointF actualCenter, QPointF startCenter, qreal scale, qreal pangle)
{
    
    makePinch(actualCenter.toPoint(),startCenter.toPoint(), scale, pangle);
    update();
}

void MapWidget::handlePinchEnd(QPointF actualCenter, QPointF startCenter, qreal scale, qreal pangle)
{
    
    makePinch(actualCenter.toPoint(),startCenter.toPoint(), scale, pangle);
    
    mouseDragging=false;

    TriggerMapRendering();
    
}

void MapWidget::makePinch(QPoint actualCenter,QPoint startCenter, qreal scale, qreal pangle)
{
    scale = sqrt(scale);
    scale = qBound(static_cast<qreal>(0.5), scale, static_cast<qreal>(2.0));
    //pinch(center, scale, state);
    std::cout << "This is a pinch!!" << actualCenter.x() << " " << actualCenter.y() << " "<< startCenter.x() << " " << startCenter.y() << " " << scale << " " << pangle << std::endl;
    
    osmscout::MercatorProjection projection=startProjection;

    if (!projection.IsValid()) {
        return;
    }

    if (!projection.Move(startCenter.x()-actualCenter.x(),
                         actualCenter.y()-startCenter.y())) {
        return;
    }

    center=projection.GetCenter();
    
    
    osmscout::Magnification maxMag;

    maxMag.SetLevel(20);

    if (startMagnification*((float)scale)>maxMag.GetMagnification()) {
        magnification.SetMagnification(maxMag.GetMagnification());
        std::cout << "setting max magn" << std::endl;
    } else if (startMagnification*((float)scale)<1) {
        magnification.SetMagnification(1);
        std::cout << "setting min magn" << std::endl;
    }
    else {
        magnification.SetMagnification(startMagnification*((float)scale));
    }
    
    // still problems with PinchArea rotation
    std::cout << "Rotation of " << fmod(pangle,180.) << std::endl;
    //angle=round((startAngle+(pangle/180)*M_PI)/DELTA_ANGLE)*DELTA_ANGLE-DELTA_ANGLE;
    //
    //if (angle<0) {
    //    angle+=2*M_PI;
    //}
}


void MapWidget::initialisationFinished(const DatabaseLoadedResponse& response)
{
    size_t zoom=1;
    double dlat=360;
    double dlon=180;
    center=response.boundingBox.GetCenter();

    while (dlat>response.boundingBox.GetHeight() &&
           dlon>response.boundingBox.GetWidth()) {
        zoom=zoom*2;
        dlat=dlat/2;
        dlon=dlon/2;
    }

    magnification=zoom;

    dbInitialized=true;

    if (hasBeenPainted) {
        TriggerMapRendering();
    }
}

void MapWidget::TriggerMapRendering()
{
    DBThread         *dbThread=DBThread::GetInstance();
    RenderMapRequest request;

    request.lat=center.GetLat();
    request.lon=center.GetLon();
    request.angle=angle;
    request.magnification=magnification;
    request.width=width();
    request.height=height();

    dbThread->CancelCurrentDataLoading();

    emit TriggerMapRenderingSignal(request);
}


void MapWidget::HandleMouseMove(int event_x, int event_y)
{
    osmscout::MercatorProjection projection=startProjection;

    if (!projection.IsValid()) {
        return;
    }

    if (!projection.Move(startX-event_x,
                         event_y-startY)) {
        return;
    }

    center=projection.GetCenter();
}

void MapWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button()==1) {
        DBThread *dbThread=DBThread::GetInstance();

        dbThread->GetProjection(startProjection);

        startX=event->x();
        startY=event->y();

        setFocus(true);
        mouseDragging=true;
    }
}

void MapWidget::mousePressEvent(int event_x, int event_y)
{
    DBThread *dbThread=DBThread::GetInstance();

    dbThread->GetProjection(startProjection);

    startX=event_x;
    startY=event_y;

    setFocus(true);
    mouseDragging=true;
}

void MapWidget::mouseMoveEvent(QMouseEvent* event)
{
    HandleMouseMove(event->x(),event->y());
    update();
}
void MapWidget::mouseMoveEvent(int event_x, int event_y)
{
    HandleMouseMove(event_x,event_y);
    update();
}

void MapWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button()==1 &&
        (startX!=event->x() ||
         startY!=event->y())) {
        mouseDragging=false;
        HandleMouseMove(event->x(),event->y());
        TriggerMapRendering();
        update();
    }
}
void MapWidget::mouseReleaseEvent(int event_x, int event_y)
{
    if (startX!=event_x ||
         startY!=event_y) {
        mouseDragging=false;
        HandleMouseMove(event_x,event_y);
        TriggerMapRendering();
        update();
    }
}

void MapWidget::wheelEvent(QWheelEvent* event)
{
    int numDegrees=event->delta()/8;
    int numSteps=numDegrees/15;

    if (numSteps>=0) {
        zoomIn(numSteps*1.35);
    }
    else {
        zoomOut(-numSteps*1.35);
    }

    event->accept();
}

void MapWidget::paint(QPainter *painter)
{
    DBThread *dbThread=DBThread::GetInstance();
    if (dbInitialized) {
        RenderMapRequest request;
        QRectF           boundingBox=contentsBoundingRect();

        request.lat=center.GetLat();
        request.lon=center.GetLon();
        request.angle=angle;
        request.magnification=magnification;
        request.width=boundingBox.width();
        request.height=boundingBox.height();

        if (!dbThread->RenderMap(*painter,request)) {
            if (!mouseDragging) {
                TriggerMapRendering();
            }
        }
    }
    else {
      dbThread->RenderMessage(*painter,width(),height(),"Database not initialized yet");
    }

    hasBeenPainted=true;
}

void MapWidget::zoomInPos(int dx, int dy, double zoomFactor)
{
    
    DBThread *dbThread=DBThread::GetInstance();

    dbThread->GetProjection(startProjection);    
    
    qDebug() << "dx is " << dx << " dy is " << dy;
    
    if (!startProjection.Move(dx,
                         dy)) {
        return;
    }    
    
    center=startProjection.GetCenter();
    
    osmscout::Magnification maxMag;

    maxMag.SetLevel(20);

    if (magnification.GetMagnification()*zoomFactor>maxMag.GetMagnification()) {
        magnification.SetMagnification(maxMag.GetMagnification());
    }
    else {
        magnification.SetMagnification(magnification.GetMagnification()*zoomFactor);
    }

    TriggerMapRendering();
}

void MapWidget::zoomIn(double zoomFactor)
{
    osmscout::Magnification maxMag;

    maxMag.SetLevel(20);

    if (magnification.GetMagnification()*zoomFactor>maxMag.GetMagnification()) {
        magnification.SetMagnification(maxMag.GetMagnification());
    }
    else {
        magnification.SetMagnification(magnification.GetMagnification()*zoomFactor);
    }

    TriggerMapRendering();
}

void MapWidget::zoomOut(double zoomFactor)
{
    if (magnification.GetMagnification()/zoomFactor<1) {
        magnification.SetMagnification(1);
    }
    else {
        magnification.SetMagnification(magnification.GetMagnification()/zoomFactor);
    }

    TriggerMapRendering();
}

void MapWidget::left()
{
    DBThread                     *dbThread=DBThread::GetInstance();
    osmscout::MercatorProjection projection;

    dbThread->GetProjection(projection);

    if (!projection.IsValid()) {
        TriggerMapRendering();
        return;
    }

    projection.MoveLeft(width()/3);

    center=projection.GetCenter();

    TriggerMapRendering();
}

void MapWidget::right()
{
    DBThread                     *dbThread=DBThread::GetInstance();
    osmscout::MercatorProjection projection;

    dbThread->GetProjection(projection);

    if (!projection.IsValid()) {
        TriggerMapRendering();
        return;
    }

    projection.MoveRight(width()/3);

    center=projection.GetCenter();

    TriggerMapRendering();
}

void MapWidget::up()
{
    DBThread                     *dbThread=DBThread::GetInstance();
    osmscout::MercatorProjection projection;

    dbThread->GetProjection(projection);

    if (!projection.IsValid()) {
        TriggerMapRendering();
        return;
    }

    projection.MoveUp(height()/3);

    center=projection.GetCenter();

    TriggerMapRendering();
}

void MapWidget::down()
{
    DBThread                     *dbThread=DBThread::GetInstance();
    osmscout::MercatorProjection projection;

    dbThread->GetProjection(projection);

    if (!projection.IsValid()) {
        TriggerMapRendering();
        return;
    }

    projection.MoveDown(height()/3);

    center=projection.GetCenter();

    TriggerMapRendering();
}

void MapWidget::rotateLeft()
{
    angle=round(angle/DELTA_ANGLE)*DELTA_ANGLE-DELTA_ANGLE;

    if (angle<0) {
        angle+=2*M_PI;
    }

    TriggerMapRendering();
}

void MapWidget::rotateRight()
{
    angle=round(angle/DELTA_ANGLE)*DELTA_ANGLE+DELTA_ANGLE;

    if (angle>=2*M_PI) {
        angle-=2*M_PI;
    }

    TriggerMapRendering();
}

void MapWidget::toggleDaylight()
{
    DBThread *dbThread=DBThread::GetInstance();

    dbThread->ToggleDaylight();
    TriggerMapRendering();
}

void MapWidget::reloadStyle()
{
    DBThread *dbThread=DBThread::GetInstance();

    dbThread->ReloadStyle();
    TriggerMapRendering();
}

void MapWidget::showCoordinates(double lat, double lon)
{
    center=osmscout::GeoCoord(lat,lon);
    this->magnification=osmscout::Magnification::magVeryClose;

    TriggerMapRendering();
}

void MapWidget::showLocation(Location* location)
{
    if (location==NULL) {
        qDebug() << "MapWidget::showLocation(): no location passed!";

        return;
    }

    qDebug() << "MapWidget::showLocation(\"" << location->getName().toLocal8Bit().constData() << "\")";

    if (location->getType()==Location::typeObject) {
        osmscout::ObjectFileRef reference=location->getReferences().front();

        DBThread* dbThread=DBThread::GetInstance();

        if (reference.GetType()==osmscout::refNode) {
            osmscout::NodeRef node;

            if (dbThread->GetNodeByOffset(reference.GetFileOffset(),node)) {
                center=node->GetCoords();
                this->magnification=osmscout::Magnification::magVeryClose;

                TriggerMapRendering();
            }
        }
        else if (reference.GetType()==osmscout::refArea) {
            osmscout::AreaRef area;

            if (dbThread->GetAreaByOffset(reference.GetFileOffset(),area)) {
                if (area->GetCenter(center)) {
                    this->magnification=osmscout::Magnification::magVeryClose;

                    TriggerMapRendering();
                }
            }
        }
        else if (reference.GetType()==osmscout::refWay) {
            osmscout::WayRef way;

            if (dbThread->GetWayByOffset(reference.GetFileOffset(),way)) {
                if (way->GetCenter(center)) {
                    this->magnification=osmscout::Magnification::magVeryClose;

                    TriggerMapRendering();
                }
            }
        }
        else {
            assert(false);
        }
    }
    else if (location->getType()==Location::typeCoordinate) {
        osmscout::GeoCoord coord=location->getCoord();

        qDebug() << "MapWidget: " << coord.GetDisplayText().c_str();

        center=coord;
        this->magnification=osmscout::Magnification::magVeryClose;

        TriggerMapRendering();
    }
}

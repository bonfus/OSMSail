import QtQuick 2.2

import QtQuick.Layouts 1.1
import QtQuick.Window 2.0

import QtPositioning 5.2
import Sailfish.Silica 1.0

import net.sf.libosmscout.map 1.0

// import "custom"

Page {
//    id: mapPage  // Already set
//    objectName: "main" is this needed?
    visible: true
    
    property Location currentLocation;
    property Location targetLocation;

    onStatusChanged: {
        if (status === PageStatus.Active) {
            if (currentLocation != null) {
                showLocation(currentLocation)
                currentLocation = null
            }
        }
    }

    function showLocation(location) {
        map.showLocation(location)
    }


    Map {
        id: map
//           Layout.fillWidth: true
//           Layout.fillHeight: true
        anchors.fill: parent   
//        anchors.margins: Theme.paddingLarge            
                    
        focus: true

        function getFreeRect() {
            return Qt.rect(Theme.horizSpace,
                            Theme.vertSpace+searchDialog.height+Theme.vertSpace,
                            map.width-2*Theme.horizSpace,
                            map.height-searchDialog.y-searchDialog.height-3*Theme.vertSpace)
        }
        
        PinchArea {
            id: mappinch
            width: map.width
            height: map.height
            pinch.minimumScale: 1.0
            pinch.maximumScale: 2.0
            pinch.minimumRotation: 0.0
            pinch.maximumRotation: 90.0            
            pinch.dragAxis: Pinch.XAndYAxis
            anchors.fill: parent
            onPinchStarted: { map.handlePinchStart() }
            onPinchUpdated: { map.handlePinchUpdate(pinch.center, pinch.startCenter, pinch.scale, pinch.rotation) }
            onPinchFinished:{ map.handlePinchEnd(pinch.center, pinch.startCenter, pinch.scale, pinch.rotation) }
        
        
            MouseArea {
                id: mapmouse
                acceptedButtons: Qt.LeftButton
                width: map.width
                height: map.height
                onReleased: {map.mouseReleaseEvent(mouse.x, mouse.y)}
                onPressed: {map.mousePressEvent(mouse.x, mouse.y)}
                onPositionChanged: {map.mouseMoveEvent(mouse.x, mouse.y)}
                onDoubleClicked: {map.zoomInPos(mouse.x-(mapmouse.width/2),(mapmouse.height/2)-mouse.y,2.0)}
            }
        }

        Keys.onPressed: {
            if (event.key === Qt.Key_Plus) {
                map.zoomIn(2.0)
                event.accepted = true
            }
            else if (event.key === Qt.Key_Minus) {
                map.zoomOut(2.0)
                event.accepted = true
            }
            else if (event.key === Qt.Key_Up) {
                map.up()
                event.accepted = true
            }
            else if (event.key === Qt.Key_Down) {
                map.down()
                event.accepted = true
            }
            else if (event.key === Qt.Key_Left) {
                if (event.modifiers & Qt.ShiftModifier) {
                    map.rotateLeft();
                }
                else {
                    map.left();
                }
                event.accepted = true
            }
            else if (event.key === Qt.Key_Right) {
                if (event.modifiers & Qt.ShiftModifier) {
                    map.rotateRight();
                }
                else {
                    map.right();
                }
                event.accepted = true
            }
            else if (event.modifiers===Qt.ControlModifier &&
                        event.key === Qt.Key_F) {
                searchDialog.focus = true
                event.accepted = true
            }
            else if (event.modifiers===Qt.ControlModifier &&
                        event.key === Qt.Key_D) {
                map.toggleDaylight();
            }
            else if (event.modifiers===Qt.ControlModifier &&
                        event.key === Qt.Key_R) {
                map.reloadStyle();
            }
        }
    }
    Item {
        id: toolbar
        width: parent.width
        height: Theme.itemSizeSmall
    
        anchors {
            bottom: parent.bottom
        }
        Rectangle {
            anchors.fill: parent
            color:  Theme.rgba("black", 0.5)
        }
        IconButton {
            id: searchBtn
            anchors.left: parent.left
            anchors.leftMargin: Theme.paddingSmall
            anchors.verticalCenter: parent.verticalCenter
            icon.source: "qrc:icons/search.png"
    
            onClicked: {
                onClicked: pageStack.push(Qt.resolvedUrl("pages/Search.qml"))
            }
        }
        IconButton {
            id: routeBtn
            anchors.left: searchBtn.right
            anchors.leftMargin: Theme.paddingSmall
            anchors.verticalCenter: parent.verticalCenter
            icon.source: "qrc:icons/route-1.png"
    
            onClicked: {
                onClicked: pageStack.push(Qt.resolvedUrl("pages/Route.qml"))
            }
        }

    }
}

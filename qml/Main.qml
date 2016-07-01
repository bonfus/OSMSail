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
 

    PositionSource {
        id: positionSource

        active: true

        onValidChanged: {
            console.log("Positioning is " + valid)
            console.log("Last error " + sourceError)

            for (var m in supportedPositioningMethods) {
                console.log("Method " + m)
            }
        }

        onPositionChanged: {
            console.log("Position changed:")

            if (position.latitudeValid) {
                console.log("  latitude: " + position.coordinate.latitude)
            }

            if (position.longitudeValid) {
                console.log("  longitude: " + position.coordinate.longitude)
            }

            if (position.altitudeValid) {
                console.log("  altitude: " + position.coordinate.altitude)
            }

            if (position.speedValid) {
                console.log("  speed: " + position.speed)
            }

            if (position.horizontalAccuracyValid) {
                console.log("  horizontal accuracy: " + position.horizontalAccuracy)
            }

            if (position.verticalAccuracyValid) {
                console.log("  vertical accuracy: " + position.verticalAccuracy)
            }
        }
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
            pinch.maximumScale: 5.0
            pinch.dragAxis: Pinch.XAndYAxis
            anchors.fill: parent
            onPinchStarted: { map.handlePinchStart(pinch.center) }
            onPinchUpdated: { map.handlePinchUpdate(pinch.center, pinch.scale, pinch.angle) }
            onPinchFinished:{ map.handlePinchEnd(pinch.center, false, pinch.scale, pinch.angle) }                
        
        
            MouseArea {
                id: mapmouse
                acceptedButtons: Qt.LeftButton
                width: map.width
                height: map.height
                onReleased: {map.qmlMouseAreaEvent(mouse.x, mouse.y, 2)}
                onPressed: {map.qmlMouseAreaEvent(mouse.x, mouse.y, 0)}
                onPositionChanged: {map.qmlMouseAreaEvent(mouse.x, mouse.y, 1)}
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
        height: Theme.itemSizeLarge
    
        anchors {
            bottom: parent.bottom
        }
        Rectangle {
            anchors.fill: parent
            color: Qt.darker(Theme.highlightColor)
        }
        IconButton {
            id: searchBtn
            anchors.left: parent.left
            anchors.leftMargin: Theme.paddingSmall
            anchors.verticalCenter: parent.verticalCenter
            icon.source: "image://theme/icon-m-search"
    
            onClicked: {
                onClicked: pageStack.push(Qt.resolvedUrl("pages/Search.qml"))
            }
        }
        IconButton {
            id: routeBtn
            anchors.left: searchBtn.right
            anchors.leftMargin: Theme.paddingSmall
            anchors.verticalCenter: parent.verticalCenter
            icon.source: "image://theme/icon-m-search"
    
            onClicked: {
                onClicked: pageStack.push(Qt.resolvedUrl("pages/Route.qml"))
            }
        }

    }
}

import QtQuick 2.0
import Sailfish.Silica 1.0
import net.sf.libosmscout.map 1.0

Page {
    id: searchPage
    
    property bool wroteDuringInterval: false
    property string lastSearchedText: ""

    property Location startLocation;
    property Location destinationLocation;

    property bool firstEntry:true;

    RoutingListModel {
        id: routingModel
    }
    
    function selectLocation(selectedLocation) {
        if (firstEntry) {
            startLocation = selectedLocation
            firstEntry = false
        } else {
            destinationLocation = selectedLocation
            route()
            pageStack.pop()
        }
    }

    function route() {
        if (startLocation !== null && destinationLocation !== null) {
            routingModel.setStartAndTarget(startLocation,
                                           destinationLocation)
        }
    }
    
    function updateSuggestions() {
    
        if (wroteDuringInterval) {
            wroteDuringInterval=false
            return
        }
        if (searchEdit.text.length < 2) {
            return
        }
        
        if (searchEdit.text === lastSearchedText)
        {
            return
        }
        
        // we have valid input
        suggestionModel.setPattern(searchEdit.text)
        lastSearchedText = searchEdit.text
    }
    
    LocationListModel {
        id: suggestionModel
    }
    
    Timer {
        id: searchTimer
        interval: 500; running: true; repeat: true
        onTriggered: updateSuggestions()
    }
    
    SearchField {
        id: searchEdit
        width: parent.width
        placeholderText: "Enter start place"
        onTextChanged: {
            wroteDuringInterval = true
        }
    }
    
    SilicaListView {

        id: suggestionsView

        anchors { top: searchEdit.bottom; left: parent.left; right: parent.right;
                    bottom: parent.bottom; topMargin: Theme.paddingMedium; }      
        
        model: suggestionModel
        delegate: ListItem {
            Label {
                anchors {
                    left: parent.left
//                    leftMargin: searchField.textLeftMargin
//                    verticalCenter: parent.verticalCenter
                }
                text: label
            }
            onClicked: {
                suggestionsView.currentIndex = index;
                var selectedLocation = suggestionModel.get(index);
                selectLocation(selectedLocation);
                searchEdit.text = ""
                searchEdit.placeholderText = "Enter destination"
                suggestionModel.clear()
            }                
        }
    }
}

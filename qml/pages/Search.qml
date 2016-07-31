import QtQuick 2.0
import Sailfish.Silica 1.0
import net.sf.libosmscout.map 1.0

Page {
    id: searchPage
    
    property bool wroteDuringInterval: false
    property string lastSearchedText: ""

    
    function selectLocation(selectedLocation) {
        mapPage.currentLocation=selectedLocation
        pageStack.pop()
    }

    
    function updateSuggestions() {
    
        if (wroteDuringInterval) {
            wroteDuringInterval=false
            return
        }
        if (searchCity.text.length < 2) {
            return
        }        
        
        if ((searchCity.text+searchAddress.text) === lastSearchedText)
        {
            return
        }
        
        // we have valid input
        suggestionModel.setPattern(searchCity.text, searchAddress.text,searchAddress.text)
        lastSearchedText = searchCity.text+searchAddress.text
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
        id: searchCity
        width: parent.width
        height: 60
        placeholderText: "Enter city"
        onTextChanged: {
            wroteDuringInterval = true
        }
    }
    SearchField {
        id: searchAddress
        width: parent.width
        height: 60
        anchors { top: searchCity.bottom; left: parent.left; right: parent.right; topMargin: 0; }        
        placeholderText: "Enter address"
        onTextChanged: {
            wroteDuringInterval = true
        }
    }
    
    SilicaListView {

        id: suggestionsView

        anchors { top: searchAddress.bottom; left: parent.left; right: parent.right;
                    bottom: parent.bottom; topMargin: 0; }      
        
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
            }                
        }
    }
}

import QtQuick 2.0
import Sailfish.Silica 1.0
import net.sf.libosmscout.map 1.0

Page {
    id: searchPage
    
    function updateSuggestions() {
        suggestionModel.setPattern(searchEdit.text)

        if (suggestionModel.count>=1 &&
            searchEdit.text===suggestionModel.get(0).name) {
//            location=suggestionModel.get(0)
        }
    }
    
    LocationListModel {
        id: suggestionModel
    }        




    SearchField {
        id: searchEdit
        width: parent.width
        placeholderText: "Search"

        onTextChanged: {
            if (searchEdit.text.length > 2) {
                updateSuggestions()
            }
        }
    }
    
    SilicaListView {
        id: suggestionsView
        anchors.fill: parent
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
        }
    }
}

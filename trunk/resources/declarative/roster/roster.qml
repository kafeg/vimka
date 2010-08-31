import Qt 4.7

Rectangle {
    id: roster

    color: "#fff"

    VisualDataModel {
        id: visualModel
        model: rosterModel
        delegate: RosterItemDelegate {}
    }

    ListView {
        id: rosterView; width: parent.width; height: parent.height;
        model: visualModel;
        cacheBuffer : 30;
        //snapMode: ListView.SnapToItem
    }

    ScrollBar {
        id: verticalScrollBar
        width: 12; height: rosterView.height-12
        anchors.right: rosterView.right
        opacity: 0
        orientation: Qt.Vertical
        position: rosterView.visibleArea.yPosition
        pageSize: rosterView.visibleArea.heightRatio
    }

    // Only show the scrollbars when the view is moving.
    states: State {
        name: "ShowBars"
        when: rosterView.movingVertically
        PropertyChanges { target: verticalScrollBar; opacity: 1 }
    }

    transitions: Transition {
        from: "*"; to: "*"
        NumberAnimation { properties: "opacity"; duration: 400 }
    }

}

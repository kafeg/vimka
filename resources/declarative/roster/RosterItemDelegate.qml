import Qt 4.7

Component {
    id: rosterDelegate

    Item {

        XmlListModel {
            id: activityModel
            query: "/response"

            XmlRole { name: "id"; query: "id/string()" }
            XmlRole { name: "time"; query: "time/string()" }
            XmlRole { name: "activity"; query: "activity/string()" }

            onStatusChanged: {
                if (status == XmlListModel.Ready){
                    if ( source != "" ){
                        contactInfoActivity.text = get(0).activity
                        contactInfoActivity.opacity = 1
                        activityLoading.on = false
                    }
                }
            }
        }

        id: wrapper;
        anchors {
            left:parent.left
            right:parent.right
        }
        height: (avatar.y + avatar.height + 10) //MEGAHACK

        Rectangle {
            color: "#CBD2D8"
            height: 1
            width: wrapper.width
            y: 0
        }

        Rectangle {
            id: bgRect;
            opacity: index % 2 ? 0.2 : 0.3
            height: wrapper.height
            width: wrapper.width
            y: 1
        }

        Rectangle {
            id: avatarBorder
            width: (avatar.width + 2); height: (avatar.height + 2);
            color: "#CBD2D8";
            smooth: true
            opacity: 0.5;

            Image {
                id: avatar
                width: 32; height: 32
                sourceSize.height:150;
                source: UserPhoto
                smooth: true
                anchors.centerIn: parent
                opacity: 0;
            }

            Image {
                id: birthday
                width: 18; height: 18
                source: "qrc:/images/birthday.png"
                smooth: true
                anchors.left: avatar.left
                anchors.bottom: avatar.bottom
                opacity: isBirthday()

                function isBirthday() {
                    //return 1;
                    var date = new Date;
                    var day = date.getDate()
                    var month = date.getMonth()

                    var dayUser = UserBDate.getDate()
                    var monthUser = UserBDate.getMonth()

                    if (month == monthUser){
                        if ((dayUser <= (day + 3)) &&  (dayUser >= (day - 1)) ){
                            //console.log(monthUser, dayUser, month, day);
                            return 1;
                        }
                    }
                    return 0;
                }
            }

            Image {
                id: incomming
                width: 18; height: 18
                source: "qrc:/images/incomming.png"
                smooth: true
                anchors.right: avatar.right
                anchors.bottom: avatar.bottom
                opacity: UserHasUnreaded
                rotation: 15;
            }

            states: State {
                name: "ShowImg"
                when: avatar.progress == 1.0 && avatar.status == Image.Ready
                && (avatarBorder.width == 92 || avatarBorder.width == 34)
                PropertyChanges { target: avatar; opacity: 1 }
            }

            transitions: Transition {
                from: "*"; to: "*"
                NumberAnimation { properties: "opacity"; duration: 150 }
            }

            anchors.left: wrapper.left
            anchors.leftMargin: 5
            anchors.topMargin: 5
            anchors.top: parent.top
        }

        Text {
            id: userName
            text: UserFullName
            color: "#c2c2c2";
            font.bold: false

            anchors.top: avatarBorder.top
            anchors.left: avatarBorder.right
            anchors.leftMargin: 5
        }

        Image {
            id: contactInfoSex
            width: 16;
            height: 16;
            source: UserSex
            anchors.right:  wrapper.right
            anchors.top: avatarBorder.top
            anchors.rightMargin: 5
            opacity: 0.5;
        }

        BusyIndicator {
            id: activityLoading
            height: 16
            width: 16
            anchors.right: contactMoreInfoButton.left
            anchors.top: avatarBorder.top
        }

        Image {
            id: contactMoreInfoButton
            width: 16;
            height: 16;
            source: "qrc:/images/more.gif"
            anchors.right:  contactInfoSex.left
            anchors.top: avatarBorder.top
            anchors.rightMargin: 5
            opacity: 0.5;
        }

        Text {
            id: contactStatus
            text: UserStatus
            color: "#738392"
            font.bold: false

            anchors.left: avatarBorder.right
            anchors.leftMargin: 10
            anchors.top: userName.bottom

            onTextChanged: {
                if (text.length == 0){
                    wrapper.state = 'offline'
                }else{
                    wrapper.state = 'normal'
                }
            }
        }

        MouseArea {
            anchors.fill: wrapper
            hoverEnabled: true
            onDoubleClicked: {
                rosterModel.openChat(index);
            }
            onEntered: {
                bgRect.color = "#CBD2D8";
            }
            onExited: {
                bgRect.color = "white";
            }
            /*onClicked: {
            if (wrapper.state == 'expanded') {
                    if (contactStatus.text.length == 0){
                        wrapper.state = 'offline'
                    }else{
                        wrapper.state = 'normal'
                    }
                    activityModel.source = ""
                } else {
                    wrapper.state = 'expanded'
                    activityModel.source = UserActivityXml
                    //console.log(activityModel.roles);
                }
            }*/
        }

        MouseArea {
            anchors.fill: contactMoreInfoButton
            onClicked: {
                if (wrapper.state == 'expanded') {
                    if (contactStatus.text.length == 0){
                        wrapper.state = 'offline'
                    }else{
                        wrapper.state = 'normal'
                    }
                    activityModel.source = ""
                } else {
                    wrapper.state = 'expanded'
                    activityModel.source = UserActivityXml
                }
            }
        }

        Item {
            id: contactInfo
            anchors.top: contactStatus.bottom
            anchors.left: avatarBorder.right
            anchors.right: wrapper.right
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            anchors.topMargin: 5
            opacity: 0;

            ProgressBar {
                id: contactInfoRate
                width: contactInfo.width
                progress: UserRate
            }
            Text {
                id: contactInfoBDate
                text: Qt.formatDate ( UserBDate, "dd.MM.yyyy");
                color: "#738392"
                font.bold: false
                anchors.top: contactInfoRate.bottom
            }
            Text {
                id: contactInfoHomePhone
                text: UserPhoneHome
                color: "#738392"
                font.bold: false
                anchors.top: contactInfoBDate.bottom
                anchors.leftMargin: 5
            }
            Text {
                id: contactInfoMobilePhone
                text: UserPhoneMobile
                color: "#738392"
                font.bold: false
                anchors.top: contactInfoHomePhone.bottom
                anchors.leftMargin: 5
            }
            Text {
                id: contactInfoUniversity
                text: UserUniversityName + " " + UserFacultyName
                color: "#738392"
                font.bold: false
                anchors.top: contactInfoMobilePhone.bottom
                anchors.leftMargin: 5
                opacity: 0;
                /*onTextChanged: {
                if (UserUniversityName.length < 5 || UserFacultyName.length < 5 ) {
                        opacity: 0
                    }
                    //console.log(text);
                }*/
            }
        }
        Text {
            id: contactInfoActivity
            text: ""
            color: "#738392"
            font.bold: false
            anchors.top: avatarBorder.bottom
            anchors.right: wrapper.right
            anchors.leftMargin: 5
            anchors.rightMargin: 5
            anchors.left: wrapper.left
            wrapMode: "WordWrap"
            width: wrapper.width
        }

        states: [
            State {
                name: "expanded"
                PropertyChanges { target: wrapper; height: avatar.height + contactInfoActivity.height + 10; }
                PropertyChanges { target: contactInfo; opacity: 1; }
                PropertyChanges { target: contactInfoActivity; opacity: 1; }
                PropertyChanges { target: activityLoading; on: true; }
                PropertyChanges { target: bgRect; color: "white" }
                PropertyChanges { target: userName; color: "black" }
                PropertyChanges { target: avatarBorder; opacity: 1; }
                PropertyChanges { target: contactInfoSex; opacity: 1;  }
                PropertyChanges { target: avatar; source: UserPhotoMedium; width:90; height: 120;
                    fillMode: Image.PreserveAspectFit; }
                PropertyChanges { target: birthday;  width:32; height: 32; }
                PropertyChanges { target: contactStatus; anchors.leftMargin: 5 }
            },
            State {
                name: "normal"
                PropertyChanges { target: wrapper; height: (avatar.y + avatar.height + 10)  }
                PropertyChanges { target: contactInfo; opacity: 0; }
                PropertyChanges { target: contactInfoActivity; opacity: 0; }
                PropertyChanges { target: activityLoading; on: false; }
                PropertyChanges { target: bgRect; color: "white"  }
                PropertyChanges { target: userName; color: "black"  }
                PropertyChanges { target: avatarBorder; opacity: 1;  }
                PropertyChanges { target: contactInfoSex; opacity: 1;  }
                PropertyChanges { target: avatar; width:32; height:32; fillMode: Image.Stretch; source: UserPhoto }
                PropertyChanges { target: contactStatus; anchors.leftMargin: 10 }
                PropertyChanges { target: birthday;  width:18; height: 18; }
            },
            State {
                name: "offline"
                PropertyChanges { target: userName; color: "#c2c2c2"  }
                PropertyChanges { target: avatarBorder; opacity: 0.5;  }
                PropertyChanges { target: contactInfoSex; opacity: 0.5;  }
                PropertyChanges { target: avatar; width:32; height:32; source: UserPhoto }
                PropertyChanges { target: contactInfoActivity; opacity: 0; }
                PropertyChanges { target: contactStatus; anchors.leftMargin: 10 }
                PropertyChanges { target: birthday;  width:18; height: 18; }
            }
        ]

        transitions: Transition {
            PropertyAnimation { properties: "height, width, opacity"; duration: 300 }
            ColorAnimation { properties: "color"; duration: 300 }
            NumberAnimation { properties: "anchors.leftMargin"; duration: 300 }
        }

        Component.onCompleted: {
            if (contactStatus.text.length > 0){
                wrapper.state = "normal"
            }
        }
    }
}

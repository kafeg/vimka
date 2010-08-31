import Qt 4.7

/*
<response>
<id>167378513</id>
<time>1281592855</time>
<activity>Болен.</activity>
</response>
*/

XmlListModel {

    query: "/response/"

    XmlRole { name: "id"; query: "pid/string()" }
    XmlRole { name: "time"; query: "pid/string()" }
    XmlRole { name: "activity"; query: "pid/string()" }
}

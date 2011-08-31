#include "vkrequest.h"

#include <QCryptographicHash>
#include <QDebug>
#include "vkmessage.h"

VKRequest::VKRequest(Request reqType, QString mid, QString sid, QString secret,
                     QString api_id, int _reqId)
{
    requestTypeText = "";
    requestUrlPath = "";
    this->reqType = reqType;
    m_mid = mid;
    m_sid = sid;
    m_secret = secret;
    m_api_id = api_id;
    reqId = _reqId;
}

void VKRequest::makeSig()
{
    m_sigSrc.append(m_mid);
    foreach(QString elem, m_requestSrc){
        if (!elem.contains("sid=")){
            m_sigSrc.append(elem);
        }
    }

    m_sigSrc.append(m_secret);

    QByteArray data = "";
    data.append(m_sigSrc.join(""));
    m_sig = QCryptographicHash::hash ( QByteArray().append(data),
                                       QCryptographicHash::Md5  ).toHex().constData();

    m_requestSrc.insert(m_requestSrc.indexOf("sid="+m_sid)+1, "sig="+m_sig);
    requestUrlPath = m_requestSrc.join("&");
}

void VKRequest::make_getProfiles(QStringList uids, QStringList fields)
{
    requestTypeText = "getProfiles";

    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("fields="+fields.join(","));
    m_requestSrc.append("format=XML");
    m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("uids="+uids.join(","));
    m_requestSrc.append("v=3.0");

    makeSig();
}

void VKRequest::make_getGroups()
{
    requestTypeText = "getGroups";

    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("format=XML");
    m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("v=3.0");

    makeSig();
}

void VKRequest::make_Activity_get(QString uid)
{
    requestTypeText = "activity.get";

    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("format=XML");
    m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("uid="+uid);
    m_requestSrc.append("v=3.0");

    makeSig();
}

void VKRequest::make_Activity_set(QString text)
{
    requestTypeText = "activity.set";

    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("format=XML");
    m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("text="+text);
    m_requestSrc.append("v=3.0");

    makeSig();
}

void VKRequest::make_Messages_getLongPollServer()
{
    requestTypeText = "messages.getLongPollServer";

    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("format=XML");
    m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("v=3.0");

    makeSig();
}

void VKRequest::make_Friends_get(QStringList fields, QString name_case)
{
    requestTypeText = "friends.get";

    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("fields="+fields.join(","));
    m_requestSrc.append("format=XML");
    m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("name_case="+name_case);
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("v=3.0");

    makeSig();
}

void VKRequest::make_Messages_send(VKMessage *message)
{
    requestTypeText = "messages.send";

    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("format=XML");
    m_requestSrc.append("message="+message->getText());
    m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("title="+message->subject);
    m_requestSrc.append("type=1"); //0 - обычное сообщение, 1 - сообщение из чата. (по-умолчанию 0)
    m_requestSrc.append("uid="+message->to_id);
    m_requestSrc.append("v=3.0");

    makeSig();
}

void VKRequest::make_Messages_markAsRead(QStringList mids)
{
    requestTypeText = "messages.markAsRead";

    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("format=XML");
    m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("mids="+mids.join(","));
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("v=3.0");

    makeSig();
}

void VKRequest::make_Messages_get(QString offset, QString count, QString out,
                       QString filters, QString preview_length,
                       QString time_offset)
{
    //    out
    //                    если этот параметр равен 1, сервер вернет исходящие сообщения.
    //    offset
    //                    смещение, необходимое для выборки определенного подмножества сообщений.
    //    count
    //                    количество сообщений, которое необходимо получить (но не более 100).
    //    filters
    //                    фильтр возвращаемых сообщений: 1 - только непрочитанные; 2 - не из чата; 4 - сообщения от друзей. Если установлен флаг "4", то флаги "1" и "2" не учитываются.
    //    preview_length
    //                    Количество симоволов по которому нужно обрезать сообщение. Укажите 0, если Вы не хотите обрезать сообщение. (по умолчанию 90).
    //    time_offset
    //                    Макисмальное время прошедшее с момента отправки сообщения до текущего момента в секундах. 0, если Вы хотите получить сообщения любой давности.

    requestTypeText = "messages.get";

    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("count="+count);
    if (filters!="0") { m_requestSrc.append("filters="+filters); }
    m_requestSrc.append("format=XML");
    m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("offset="+offset);
    if (filters=="1") { m_requestSrc.append("out="+out); }
    m_requestSrc.append("preview_length="+preview_length);
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("time_offset="+time_offset);
    m_requestSrc.append("v=3.0");

    makeSig();
}

void VKRequest::make_Messages_getHistory(QString uid, QString offset, QString count)
{
//    uid     ✔	идентификатор пользователя, историю переписки с которым необходимо вернуть.
//    offset    смещение, необходимое для выборки определенного подмножества сообщений.
//    count     количество сообщений, которое необходимо получить (но не более 100).
    requestTypeText = "messages.getHistory";

    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("count="+count);
    m_requestSrc.append("format=XML");
    m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("offset="+offset);
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("uid="+uid);
    m_requestSrc.append("v=3.0");

    makeSig();
}

void VKRequest::make_Audio_get(QString uid, QString gid, QString aids,
                    QString need_user)
{
    //    uid id пользователя, которому принадлежат аудиозаписи. По умолчанию id текущего пользователя.
    //    gid id группы, которой принадлежат аудиозаписи. Если указан параметр gid, uid игнорируется.
    //    aids перечисленные через запятую id аудиозаписей, входящие в выборку по uid или gid.
    //    need_user если этот параметр равен 1, сервер возвратит базовую информацию о владельце аудиозаписей в структуре user (id, photo, name, name_gen).
    requestTypeText = "audio.get";

    if (aids != "-1") { m_requestSrc.append("aids="+aids); }
    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("format=XML");
    if (gid != "-1") { m_requestSrc.append("gid="+gid); }
    m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("need_user="+need_user);
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("uid="+uid);
    m_requestSrc.append("v=3.0");

    makeSig();
}

void VKRequest::make_Audio_getById(QStringList audios)
{
    //    audios
    //    перечисленные через запятую идентификаторы, которые представляют собой идущие через знак подчеркивания id пользователей, которым принадлежат аудиозаписи, и id самих аудиозаписей. Если аудиозапись принадлежит группе, то в качестве первого параметра используется -id группы.
    //    Пример значения audios: 2_67859194,-683495_39822725,2_63937759.
    //    requestTypeText = "audio.get";
    requestTypeText = "audio.getById";

    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("audios="+audios.join(","));
    m_requestSrc.append("format=XML");
   m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("v=3.0");

    makeSig();
}

void VKRequest::make_Photos_getAlbums(QString uid, QStringList aids)
{
    //    uid ID пользователя, которому принадлежат альбомы. По умолчанию ID текущего пользователя.
    //    aids перечисленные через запятую ID альбомов.
    requestTypeText = "photos.getAlbums";

    if (aids.isEmpty()) { m_requestSrc.append("aids="+aids.join(",")); }
    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("format=XML");
    m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("uid="+uid);
    m_requestSrc.append("v=3.0");

    makeSig();
}

void VKRequest::make_Photos_get(QString uid, QString aid, QStringList pids)
{
    //    uid ✔ ID пользователя, которому принадлежит альбом с фотографиями.
    //    aid ✔ ID альбома с фотографиями.
    //    pids перечисленные через запятую ID фотографий.
    requestTypeText = "photos.get";

    m_requestSrc.clear();
    m_sigSrc.clear();

    m_requestSrc.append("aid="+aid);
    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("format=XML");
    m_requestSrc.append("method="+requestTypeText);
    if (!pids.isEmpty()) { m_requestSrc.append("pids="+pids.join(",")); }
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("uid="+uid);
    m_requestSrc.append("v=3.0");

    makeSig();
}

void VKRequest::make_Photos_getById(QStringList photos)
{
    //    photos
    //    перечисленные через запятую идентификаторы, которые представляют собой идущие через знак подчеркивания id пользователей, разместивших фотографии, и id самих фотографий.
    //    Пример значения photos: 1_129207899,6492_135055734
    requestTypeText = "photos.getById";

    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("format=XML");
    m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("photos="+photos.join(","));
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("v=3.0");

    makeSig();
}

void VKRequest::make_Photos_getUploadServer(QString aid, QString gid)
{
    //    photos
    //    aid
    //    ✔
    //    ID альбома, в который необходимо загрузить фотографии.
    //    gid
    //    ID группы, при загрузке фотографии в группу.
    requestTypeText = "photos.getUploadServer";

    m_requestSrc.append("aid="+aid);
    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("format=XML");
    if ( !gid.isEmpty() ) m_requestSrc.append("gid="+gid);
    m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("v=3.0");

    makeSig();
}

void VKRequest::make_Video_get(QString uid, QStringList videos,
                    QString width, QString count,
                    QString offset)
{
//    videos
//    перечисленные через запятую идентификаторы, которые представляют собой идущие через знак подчеркивания id пользователей, которым принадлежат видеозаписи, и id самих видеозаписей. Если видеозапись принадлежит группе, то в качестве первого параметра используется -id группы.
//    Пример значения videos: -4363_136089719,13245770_137352259.
//    uid
//    id пользователя, видеозаписи которого нужно вернуть. Если указан параметр videos, uid игнорируется.
//    width
//    требуемая ширина изображений видеозаписей в пикселах. Возможные значения - 130, 160 (по умолчанию), 320.
//    count
//    количество возвращаемых видеозаписей (максимум 200).
//    offset
//    смещение относительно первой найденной видеозаписи для выборки определенного подмножества.
    requestTypeText = "video.get";

    m_requestSrc.append("api_id="+m_api_id);
    if (count != "-1") { m_requestSrc.append("count="+count); }
    m_requestSrc.append("format=XML");
    m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("offset="+offset);
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("uid="+uid);
    if (!videos.isEmpty()) { m_requestSrc.append("videos="+videos.join(",")); }
    m_requestSrc.append("v=3.0");
    m_requestSrc.append("width="+width);

    makeSig();
}

void VKRequest::make_Audio_add(QString aid, QString oid, QString gid)
{
//    aid
//    ✔
//    id аудиозаписи.
//    oid
//    ✔
//    id владельца аудиозаписи. Если копируемая аудиозапись находится на странице группы, в этом параметре должно стоять значение, равное -id группы.
//    gid
//    id группы, в которую следует копировать аудиозапись. Если параметр не указан, аудиозапись копируется не в группу, а на страницу текущего пользователя. Если аудиозапись все же копируется в группу, у текущего пользователя должны быть права на эту операцию.

    requestTypeText = "audio.add";

    m_requestSrc.append("aid="+aid);
    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("format=XML");
    if (gid != "-1") { m_requestSrc.append("gid="+gid); }
    m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("oid="+oid);
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("v=3.0");

    makeSig();
}

void VKRequest::make_Messages_delete(QString mid)
{
    //    mid идентификатор сообщения.

    requestTypeText = "messages.delete";

    m_requestSrc.append("api_id="+m_api_id);
    m_requestSrc.append("format=XML");
    m_requestSrc.append("method="+requestTypeText);
    m_requestSrc.append("mid="+mid);
    m_requestSrc.append("sid="+m_sid);
    m_requestSrc.append("v=3.0");

    makeSig();
}

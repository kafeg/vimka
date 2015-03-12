**Внимание! В данный момент проект мной не развивается, но если найдутся люди, которым будет интересно его развитие, то я с радостью предоставлю доступ к этому проекту на Google Code, чтобы вы могли коммитить изменения и выкладывать сборки прям сюда.**

**Vimka** - проект открытого IM-клиента, основанного на API социальной сети ВКонтакте. Всю необходимую информацию можно найти в официальной группе клиента.

<a href='https://plus.google.com/111192421155878279129'>Google+</a>

Вся необходимая информация находится в официальной группе клиента. Ссылка слева.

Поддерживаемые платформы:
  * Windows
  * Linux
  * Mac OS X
  * Meego
  * Symbian
  * Android (теоретически... Через android-lighthouse)

Возможности:

  * мульти-язычность (в наличии русский/английский)
  * однооконный вариант интерфейса - специально для мобильных устройств, с эффектом слайдинга.
  * проверка орфографии (русский/английский)
  * профили пользователей, автовход
  * функциональный стиль чата, позволяет прямо в окошке чата совершать множество действий... Группирует сообщения пользователей в истории сообщений. Ну а кликнув по аватарке или по имени пользователя можно просмотреть его профиль ВКонтакте в браузере по умолчанию
  * отображение и смена статуса пользователя В Контакте
  * персональные чаты пользователей с вконтактовскими фичами подсвечивания непрочитанных сообщений и обновлением unread-статуса в реальном времени. Кроме того из чата можно удалить сообщение либо пометить его как прочитанное, жмякнув на соответствующую кнопку
  * отправка собеседнику аудио со своей страницы
  * добавление на свою страничку присланного аудио одним кликом. При этом в чат вышлется сообщение о том что Вы добавили аудио
  * просмотр своих альбомов, просмотр фотографий в альбомах
  * отправка любой фотографии из любого своего альбома собеседнику
  * просмотр присланной фотографии тут же в окошке чата, после клика по фотке
  * система оповещения, включает: мигающую иконку в трее, сервис-сообщения в трее, мигающие заголовки вкладок в чатах. Умная звуковая оповещалка и оповещалка в трее. Умная потому что первое: если Вы в данный момент слушаете музыку присланную собеседником, или же сообщение прислал собеседник в текущем открытом чате, то оповещалка не будет бестолку играть мелодию и раздражать... второе: уведомлялка втрее интегрируется с рабочим столом винды и показывает ровно то что нужно: пришло сообщение, кликни для прочтения... Сначала это маленькое оповещение может показаться лишним, но на самом деле это позволяет не захламлять рабочий стол и не отвлекать пользователя попусту
  * сохранение/загрузка настроек приложения, вроде положения и размера окошка, настройки соединения с сетью и прочее
  * получение истории сообщений, догрузкой их в окошко чата. После релиза сюда должен добавиться фильтр
  * смайлики
  * отправка видео со своей странички
  * собеседник может просмотреть видео на сайте ВКонтакте в браузере по умочланию, кликнув по пришедшему видео. Видеоплеер в приложении имеется, но он выключен потому как в правилах описан запрет на разбор кода страниц, а прямой ссылки на видео ВКонтакте не даёт
  * обработка тела сообщения и нахождение смайликов, гипер-ссылок, email-адресов, escape-элементов, а также медиа контента в сообщениях при их приходе, перед отображением
  * поддержка прокси-сервера Socks5 или http
  * проверка доставки сообщения, удаление текста сообщения из окошка ввода только после его доставки
  * flood-контроль, а точнее обработка вконтактовского флуд-контроля...
  * повсюду где было можно, выкинуты лишние надписи и заменены на аккуратные тултипки, всплывающие при необходимости. Это к примеру иконки в окошке чата, это кнопки меню и прочее
  * напоминание о дне рождения друга - тортик рядом с его аватаркой =)
  * загрузка информации о непрочитанных сообщений при старте приложения и рисование маленького конвертика поверх аватарки как уведомления что от этого контакта есть сообщение которое пока не прочитано. Конвертик исчезает после открытия окошка чата с данным контактом
  * кнопка дополнительной информации которая показывает статус, увеличенную аватарку и некоторые личные данные о пользователе прямо здесь же в списке контактов, с хорошей анимацией
  * никакой локальной истории, никакого сохранения пароля, если не стоит галочка "Автовход". Можно войти в свой аккаунт с любого компьютера, пообщаться, нажать кнопку выхода и всё, никакой конфиденциальной информации сохранено не будет, кроме логина =)
  * автоопределние текущего языка системы и подгрузка необходимого файла локализации при его наличии. Иначе грузися язык по умолчанию - английский
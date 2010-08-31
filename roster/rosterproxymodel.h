#ifndef ROSTERPROXYMODEL_H
#define ROSTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class SettingsManager;

class RosterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit RosterProxyModel(QObject *parent, SettingsManager *sMn);

    enum RosterRoles
    {
        NameRole = Qt::UserRole+1,
        UrlRole,
        UrlMediumRole,
        UserStatusRole,
        UserSexRole,
        UserBDateRole,
        UserActivityRole,
        UserRateRole,
        UserHomePhoneRole,
        UserMobilePhoneRole,
        UserUniverstiyNameRole,
        UserFacultyNameRole,
        UserCityRole,
        UserCountryRole,
        UserHasUnreadedMessages
    };

    QVariant data(const QModelIndex &index, int role) const;
    SettingsManager *m_sMn;

signals:

public slots:
    void openChat(int row);

};

#endif // ROSTERPROXYMODEL_H

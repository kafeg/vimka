#include "rosterproxymodel.h"
#include <QStandardItemModel>
#include <QDebug>
#include "../vk/vkroster.h"
#include <QDate>
#include "../settingsmanager.h"
#include <QUrl>

RosterProxyModel::RosterProxyModel(QObject *parent, SettingsManager *sMn) :
    QSortFilterProxyModel(parent)
{
    m_sMn = sMn;
}

QVariant RosterProxyModel::data(const QModelIndex &index, int role) const
{
    QStandardItemModel *srcModel = qobject_cast<QStandardItemModel *>(sourceModel());
    QModelIndex srcIndex = mapToSource(index);

    switch(role){
    case NameRole:
        //qDebug() << "ItemName";
        return srcModel->item(srcIndex.row(), 3)->text();
        break;
    case UrlRole:
        return srcModel->item(srcIndex.row(), 8)->text();
        break;
    case UrlMediumRole:
        return srcModel->item(srcIndex.row(), 16)->text();
        break;
    case UserStatusRole:
        return srcModel->item(srcIndex.row(), 12)->text();
        break;
    case UserSexRole:{
            //return tr("Sex: %1").arg( srcModel->item(srcIndex.row(), 13)->text());
            QString res = "";
             //1 - женский, 2 - мужской, 0 - без указания пола.
            switch(srcModel->item(srcIndex.row(), 7)->text().toInt()){
            case 0:
                res = m_sMn->sexnonepath;
                break;
            case 1:
                res = m_sMn->sexfemalepath;
                break;
            case 2:
                res = m_sMn->sexmalepath;
                break;
            }
            return QUrl::fromLocalFile(res);
            break;
        }
    case UserBDateRole:
        //return tr("Birthday: %1").arg( srcModel->item(srcIndex.row(), 10)->text());
        return QDate::fromString(srcModel->item(srcIndex.row(), 10)->text(), "dd.MM.yyyy");
        break;
    case UserActivityRole:
        return srcModel->item(srcIndex.row(), 14)->text();
        break;
    case UserRateRole:
        return srcModel->item(srcIndex.row(), 15)->text().toInt();
        break;
    case UserHomePhoneRole:
        return tr("Home: %1").arg(srcModel->item(srcIndex.row(), 17)->text().trimmed().isEmpty()
                                  ? tr("undefined") : srcModel->item(srcIndex.row(), 17)->text().trimmed());
        break;
    case UserMobilePhoneRole:
        return tr("Mobile: %1").arg(srcModel->item(srcIndex.row(), 18)->text().trimmed().isEmpty()
                                    ? tr("undefined") : srcModel->item(srcIndex.row(), 18)->text().trimmed());
        break;
    case UserUniverstiyNameRole:
        return srcModel->item(srcIndex.row(), 19)->text().trimmed();
        break;
    case UserFacultyNameRole:
        return srcModel->item(srcIndex.row(), 20)->text().trimmed();
        break;
    case UserHasUnreadedMessages:
        return srcModel->item(srcIndex.row(), 21)->text().trimmed();
        break;
    default:
        return srcModel->data(srcIndex, role);
    }
}

void RosterProxyModel::openChat(int row)
{

    QModelIndex pInd = index(row,0);
    //qDebug() << pInd;

    //QModelIndex srcIndex = mapToSource(pInd);

    //qDebug() << srcIndex;
    VKRoster *srcModel = qobject_cast<VKRoster *>(sourceModel());
    srcModel->slotOpenChat(pInd);
}

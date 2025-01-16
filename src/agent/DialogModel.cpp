#include "DialogModel.h"

DialogModel::DialogModel(QObject *parent)
    : QAbstractItemModel(parent) {
}

DialogModel::~DialogModel() = default;

QHash<int, QByteArray> DialogModel::roleNames() const {
    static QHash<int, QByteArray> roles{
        {CookieRole, "cookie"},
        {TypeRole, "type"},
        {DescriptionRole, "description"},
        {MessageRole, "message"},
        {ActionsRole, "actions"},
    };
    return roles;
}

QModelIndex DialogModel::index(int row, int column,
                               const QModelIndex &parent) const {
    if (parent.isValid()) {
        return QModelIndex();
    }

    if (column != 0) {
        return QModelIndex();
    }

    return createIndex(row, column, nullptr);
}

QModelIndex DialogModel::parent(const QModelIndex &child) const {
    return QModelIndex();
}

int DialogModel::rowCount(const QModelIndex &parent) const {
    Q_ASSERT(!parent.isValid());

    return m_data.size();
}

int DialogModel::columnCount(const QModelIndex &parent) const {
    Q_ASSERT(!parent.isValid());

    return 1;
}

QVariant DialogModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    auto row = index.row();

    if (role == CookieRole) {
        return m_data[row]->cookie;
    } else if (role == DescriptionRole) {
        return m_data[row]->description;
    } else if (role == MessageRole) {
        return m_data[row]->message;
    } else if (role == ActionsRole) {
        return m_data[row]->actions;
    }

    return QVariant();
}

void DialogModel::newDialog(DialogInstance *di, const QVariantMap &options) {
    auto row = m_data.size();

    beginInsertRows(QModelIndex(), row, row);
    m_data.append(di);
    endInsertRows();
}

void DialogModel::removeDialog(const QString &cookie) {
    auto i = std::find_if(m_data.begin(), m_data.end(), [&cookie](auto &di) {
        return di->cookie == cookie;
    });

    if (i == m_data.end()) {
        qWarning() << "Dialog not found" << cookie;
        return;
    }

    DialogInstance *di = *i;

    int row = std::distance(m_data.begin(), i);
    beginRemoveRows(QModelIndex(), row, row);
    m_data.erase(i);
    endRemoveRows();
}

void DialogModel::trigger(const QString &cookie, int actionIdx) {
    auto i = std::find_if(m_data.begin(), m_data.end(), [&cookie](auto &di) {
        return di->cookie == cookie;
    });

    if (i == m_data.end()) {
        qWarning() << "Dialog not found" << cookie;
        return;
    }

    DialogInstance *di = *i;
    Q_ASSERT(actionIdx >= 0 && actionIdx < di->actions.size());

    int row = std::distance(m_data.begin(), i);
    beginRemoveRows(QModelIndex(), row, row);
    m_data.erase(i);
    endRemoveRows();

    emit triggered(cookie, actionIdx);
}

#pragma once

#include <QAbstractItemModel>
#include <QQmlEngine>

#include "DialogInstance.h"

class DialogModel : public QAbstractItemModel {
    Q_OBJECT
    QML_ELEMENT

public:
    enum Roles {
        CookieRole = Qt::UserRole, // actually "pluginId::itemKey" or an internal one.
        TypeRole,
        DescriptionRole,
        MessageRole,
        ActionsRole,
    };
    Q_ENUM(Roles)

    DialogModel(QObject *parent = nullptr);
    ~DialogModel();

    void newDialog(DialogInstance *di, const QVariantMap &options);
    void removeDialog(const QString &cookie);

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE QModelIndex index(int row, int column,
                                  const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE QModelIndex parent(const QModelIndex &child) const override;

    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    Q_INVOKABLE QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE void trigger(const QString &cookie, int actionIdx);

Q_SIGNALS:
    void triggered(const QString &cookie, int actionIdx);

private:
    QList<DialogInstance *> m_data;
};

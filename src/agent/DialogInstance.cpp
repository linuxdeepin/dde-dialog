#include "DialogInstance.h"

#include <QDBusObjectPath>

#include <libintl.h>

#include <fmt/format.h>
#include <fmt/args.h>

DialogInstance::DialogInstance(const QString &cookie, const Schema &schema, const QVariantMap &options, QObject *parent)
    : QObject(parent)
    , cookie(cookie)
    , type(schema.type) {
    auto gettextDomain = schema.gettextDomain.toStdString();
    for (auto &action : schema.actions) {
        if (action == "cancel") {
            actions << tr("Cancel");
            continue;
        } else if (action == "ok") {
            actions << tr("Ok");
            continue;
        } else if (action.startsWith("custom:")) {
            auto tmp = action.mid(7);
            if (schema.gettextDomain.isEmpty()) {
                actions << tmp;
            } else {
                QString tmp1 = dgettext(gettextDomain.c_str(), tmp.toStdString().c_str());
                actions << tmp1;
            }
            continue;
        }

        Q_UNREACHABLE();
    }

    if (schema.gettextDomain.isEmpty()) {
        description = schema.description;
        message = schema.message;
    } else {
        description = dgettext(gettextDomain.c_str(), schema.description.toStdString().c_str());
        message = dgettext(gettextDomain.c_str(), schema.message.toStdString().c_str());
    }

    if (options.contains("fields")) {
        auto fieldsRaw = options["fields"].value<QDBusArgument>();

        QList<QVariant> fields;
        fieldsRaw >> fields;

        message = QString::fromStdString(fmt::vformat(message.toStdString(), getFieldsStore(fields)));
    }
}

DialogInstance::~DialogInstance() = default;

fmt::dynamic_format_arg_store<fmt::format_context> DialogInstance::getFieldsStore(const QList<QVariant> &fields) {
    fmt::dynamic_format_arg_store<fmt::format_context> store;
    for (auto field : fields) {
        switch (field.userType()) {
            case QMetaType::Bool: {
                store.push_back(field.value<bool>());
                break;
            }
            case QMetaType::UChar: {
                store.push_back(field.value<unsigned char>());
                break;
            }
            case QMetaType::Short: {
                store.push_back(field.value<short>());
                break;
            }
            case QMetaType::UShort: {
                store.push_back(field.value<unsigned short>());
                break;
            }
            case QMetaType::Int: {
                store.push_back(field.value<int>());
                break;
            }
            case QMetaType::UInt: {
                store.push_back(field.value<unsigned int>());
                break;
            }
            case QMetaType::LongLong: {
                store.push_back(field.value<long long>());
                break;
            }
            case QMetaType::ULongLong: {
                store.push_back(field.value<unsigned long long>());
                break;
            }
            case QMetaType::Double: {
                store.push_back(field.value<double>());
                break;
            }
            case QMetaType::QString: {
                store.push_back(field.value<QString>().toStdString());
                break;
            }
            default: {
                qWarning() << "Unknown field type" << field.typeName();
            }
        }
    }

    return store;
}

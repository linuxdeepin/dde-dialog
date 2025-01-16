#include "OpenOptions.h"

#include <QDBusArgument>

OpenOptions OpenOptions::fromVariantMap(const QVariantMap &options) {
    OpenOptions res;

    if (options.contains("fields")) {
        auto fields = options["fields"];
        if (fields.metaType() == QMetaType::fromType<QDBusArgument>()) {
            auto tmp = fields.value<QDBusArgument>();
            tmp >> res.fields;
        } else if (fields.metaType() == QMetaType::fromType<QList<QVariant>>()) {
            res.fields = fields.value<QList<QVariant>>();
        }
    }

    return res;
}

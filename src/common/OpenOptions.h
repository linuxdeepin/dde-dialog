#pragma once

#include <QVariant>

struct OpenOptions {
    QVariantList fields;

    static OpenOptions fromVariantMap(const QVariantMap &options);
};

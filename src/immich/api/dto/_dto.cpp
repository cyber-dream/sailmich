#include "_dto.h"
#include <QDebug>
using namespace Immich::Api;

const QMap<QString, Models::AssetVisibility> Models::assetVisibilityStrs = {
    {"ARCHIVE", Models::AssetVisibility::ARCHIVE},
    {"TIMELINE", Models::AssetVisibility::TIMELINE},
    {"HIDDEN", Models::AssetVisibility::HIDDEN},
    {"LOCKED", Models::AssetVisibility::LOCKED}};

const QMap<QString, Models::AssetOrder> Models::assetOrderStrs = {
    {"ASC", Models::AssetOrder::ASC}, {"DESC", Models::AssetOrder::DESC}};

const QMap<QString, Models::AssetMediaSize> Models::assetMediaSizeStrs = {
    {"FULLSIZE", Models::AssetMediaSize::FULLSIZE},
    {"PREVIEW", Models::AssetMediaSize::PREVIEW},
    {"THUMBNAIL", Models::AssetMediaSize::THUMBNAIL},
};

Models::AssetVisibility Models::AssetVisibilityFromString(const QString &str) {
  return assetVisibilityStrs.value(
      str.toUpper(), Models::AssetVisibility::AssetVisibilityNONE);
}

QString Models::AssetVisibilityToString(const Models::AssetVisibility vis) {
  return assetVisibilityStrs.key(vis).toLower();
}

bool Models::AssetVisibilityIsValidString(const QString &str) {
  return assetVisibilityStrs.contains(str.toUpper());
}

Models::AssetOrder Models::AssetOrderFromString(const QString &str) {
  return assetOrderStrs.value(str.toUpper(),
                              Models::AssetOrder::AssetOrderNONE);
}

QString Models::AssetOrderToString(const AssetOrder order) {
  return assetOrderStrs.key(order).toLower();
}

bool Models::AssetOrderIsValidString(const QString &str) {
  return assetOrderStrs.contains(str.toUpper());
}

Models::AssetMediaSize Models::AssetMediaSizeFromString(const QString &str) {
  return assetMediaSizeStrs.value(str,
                                  Models::AssetMediaSize ::AssetMediaSizeNONE);
}

QString Models::AssetMediaSizeToString(const AssetMediaSize size) {
  return assetMediaSizeStrs.key(size).toLower();
}

bool Models::AssetMediaSizeIsValidString(const QString &str) {
  return assetMediaSizeStrs.contains(str);
};


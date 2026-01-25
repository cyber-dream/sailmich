#ifndef _DTO_H
#define _DTO_H

#include <QColor>
#include <QDateTime>
#include <QJsonObject>
#include <QMap>
#include <QMetaProperty>
#include <QObject>
#include <QString>
#include <QUrl>
#include <QUuid>
#include <QVector>
#include <cstring>
#include <tl-optional.h>

Q_DECLARE_METATYPE(tl::optional<QString>)

#include "json-parser.h"

namespace Immich {
namespace Api {

struct DtoStruct {
  Q_GADGET
  Q_PROPERTY(bool isValid READ isValid CONSTANT)
public:
  DtoStruct() : m_errMessage() {};

  template <typename T>
  static typename std::enable_if<std::is_base_of<DtoStruct, T>::value, T>::type
  fromJson(const QJsonObject &jsonObject) {
    auto obj = T();

    QHash<QString, QString> overrides;
    QHash<QString, bool> nullAsZeroValue;

    const QMetaObject &meta = T::staticMetaObject;
    for (int i = 0; i < meta.classInfoCount(); ++i) {
      QMetaClassInfo info = meta.classInfo(i);
      QString fullTag = QString::fromLatin1(info.name());

      if (fullTag.endsWith(":json"))
        overrides.insert(fullTag.section(':', 0, 0),
                         QString::fromLatin1(info.value()));

      if (fullTag.endsWith(":nullIsZeroValue"))
        nullAsZeroValue.insert(fullTag.section(':', 0, 0),
                               std::strcmp(info.value(), "true"));
    }

    for (int i = meta.propertyOffset(); i < meta.propertyCount(); ++i) {
      const QMetaProperty prop = meta.property(i);
      const QString propName = QString::fromLatin1(prop.name());
      const QString jsonKey = overrides.value(propName, propName);

      if (jsonObject.contains(jsonKey)) {
        const auto result =
            JsonValueParser::prop(jsonObject[jsonKey], prop.type());
        if (!result.isSucceeded()) {
          obj.m_errMessage =
              QString("Field '%1': %2").arg(jsonKey, result.error().message());
          return obj;
        }

        if (!prop.writeOnGadget(&obj, result.data())) {
          obj.m_errMessage = "failed to write field: " + jsonKey;
          return obj;
        }
      } else {
        obj.m_errMessage = "missing field: " + jsonKey;
        return obj;
      }
    }

    return obj;
  };

  template <typename T>
  static typename std::enable_if<std::is_base_of<DtoStruct, T>::value, T>::type
  fromFlattenedArrObject(const QJsonObject &jsonObject, const int pIdx) {
    auto obj = T();

    QHash<QString, QString> overrides;

    const QMetaObject &meta = T::staticMetaObject;
    for (int i = 0; i < meta.classInfoCount(); ++i) {
      QMetaClassInfo info = meta.classInfo(i);
      QString fullTag = QString::fromLatin1(info.name());

      if (fullTag.endsWith(":json")) {
        overrides.insert(fullTag.section(':', 0, 0),
                         QString::fromLatin1(info.value()));
      }
    }

    for (int idx = meta.propertyOffset(); idx < meta.propertyCount(); ++idx) {
      const QMetaProperty prop = meta.property(idx);
      const QString propName = QString::fromLatin1(prop.name());
      const QString jsonKey = overrides.value(propName, propName);

      if (jsonObject.contains(jsonKey)) {
        if (!jsonObject[jsonKey].isArray()) {
          obj.m_errMessage =
              "field \"" + jsonKey + "\" in json object is not an array";
          return obj;
        }
        const auto result = JsonValueParser::prop(
            jsonObject[jsonKey].toArray()[pIdx], prop.type());

        if (!result.isSucceeded()) {
          obj.m_errMessage =
              QString("Field '%1': %2").arg(jsonKey, result.error().message());
          return obj;
        }

        if (!prop.writeOnGadget(&obj, result.data())) {
          obj.m_errMessage = "failed to write field: " + jsonKey;
          return obj;
        }
      } else {
        obj.m_errMessage = "missing field: " + jsonKey;
        return obj;
      }
    }

    return obj;
  }

  bool isValid() const { return !m_errMessage.has_value(); };
  QString errMessage() const {
    return m_errMessage.value_or("NO ERROR!!! YOU has wrong error check!");
  };

private:
  tl::optional<QString> m_errMessage = tl::optional<QString>();
};

struct Models {
  Q_GADGET
public:
  enum AssetVisibility {
    AssetVisibilityNONE = -1,
    ARCHIVE,
    TIMELINE,
    HIDDEN,
    LOCKED
  };
  Q_ENUM(AssetVisibility)
  static AssetVisibility AssetVisibilityFromString(const QString &str);
  static QString AssetVisibilityToString(const AssetVisibility vis);
  static bool AssetVisibilityIsValidString(const QString &str);

  enum AssetOrder { AssetOrderNONE = -1, ASC, DESC };
  Q_ENUM(AssetOrder)
  static AssetOrder AssetOrderFromString(const QString &str);
  static QString AssetOrderToString(const AssetOrder order);
  static bool AssetOrderIsValidString(const QString &str);

  enum AssetMediaSize { AssetMediaSizeNONE = -1, PREVIEW, FULLSIZE, THUMBNAIL };
  Q_ENUM(AssetMediaSize)
  static AssetMediaSize AssetMediaSizeFromString(const QString &str);
  static QString AssetMediaSizeToString(const AssetMediaSize order);
  static bool AssetMediaSizeIsValidString(const QString &str);

  enum SourceType { SourceTypeNONE = -1, MACHINE_LEARNING, EXIF, MANUAL };
  Q_ENUM(SourceType);
  static SourceType SourceTypeFromString(const QString &str);
  static QString SourceTypeToString(const SourceType order);
  static bool SourceTypeIsValidString(const QString &str);

  enum UserAvatarColor {
    UserAvatarColorNONE = -1,
    PRIMARY,
    PINK,
    RED,
    YELLOW,
    BLUE,
    GREEN,
    PURPLE,
    ORANGE,
    GRAY,
    AMBER
  };
  Q_ENUM(UserAvatarColor);
  static UserAvatarColor UserAvatarColorFromString(const QString &str);
  static QString UserAvatarColorToString(const UserAvatarColor order);
  static bool UserAvatarColorIsValidString(const QString &str);

  enum AssetType {
    AssetTypeEnum = -1,
    IMAGE,
    VIDEO,
    AUDIO,
    OTHER,
  };
  Q_ENUM(AssetType)
  static AssetType AssetTypeColorFromString(const QString &str);
  static QString AssetTypeColorToString(const AssetType order);
  static bool AssetTypeColorIsValidString(const QString &str);

  enum AlbumUserRole { AlbumUserRoleNONE = -1, EDITOR, VIEWER };
  Q_ENUM(AlbumUserRole)
  static AlbumUserRole AlbumUserRoleFromString(const QString &str);
  static QString AlbumUserRoleToString(const AlbumUserRole order);
  static bool AlbumUserRoleIsValidString(const QString &str);

private:
  static const QMap<QString, AssetVisibility> assetVisibilityStrs;
  static const QMap<QString, AssetOrder> assetOrderStrs;
  static const QMap<QString, AssetMediaSize> assetMediaSizeStrs;
  static const QMap<QString, SourceType> sourceTypeStrs;
  static const QMap<QString, UserAvatarColor> UserAvatarColorStrs;
  static const QMap<QString, AssetType> AssetTypeStrs;
  static const QMap<QString, AlbumUserRole> AlbumUserRoleStrs;
};

// inline QString boolStr(const bool pBool) { return pBool ? "true" : "false"; }

// inline QDateTime parseIsoTime(QString input) {
//   int dot = input.indexOf('.', input.indexOf('T'));
//   if (dot >= 0) {
//     int msStart = dot + 1;
//     int msEnd = msStart;
//     while (msEnd < input.size() && input[msEnd].isDigit())
//       ++msEnd;

//    int msLength = msEnd - msStart;

//    if (msLength == 0) {
//      input.insert(msStart, "000");
//    } else if (msLength == 1) {
//      input.insert(msEnd, "00"); // .5 → .500
//    } else if (msLength == 2) {
//      input.insert(msEnd, "0"); // .57 → .570
//    } else if (msLength > 3) {
//      input.remove(msStart + 3, msLength - 3); // .5789 → .578
//    }
//  } else {
//    input += ".000";
//  }

//  return QDateTime::fromString(input, "yyyy-MM-dd'T'HH:mm:ss.zzz");
//};

// inline QString uuidStr(const QUuid &uuid) {
//   return uuid.toString().replace("{", "").replace("}", "");
// };

struct ExifResponseDto {
public:
  tl::optional<QString> city;
  tl::optional<QString> country;
  tl::optional<QDateTime> dateTimeOriginal;
  tl::optional<QString> description;
  tl::optional<quint32> exifImageHeight;
  tl::optional<quint32> exifImageWidth;
  tl::optional<QString> exposureTime;
  tl::optional<qreal> fNumber;
  tl::optional<quint64> fileSizeInByte;
  tl::optional<float> focalLength;
  tl::optional<float> iso;
  tl::optional<qreal> latitude;
  tl::optional<QString> lensModel;
  tl::optional<qreal> longitude;
  tl::optional<QString> make;
  tl::optional<QString> model;
  tl::optional<QDateTime> modifyDate;
  tl::optional<QString> orientation;
  tl::optional<QString> projectionType;
  tl::optional<quint8> rating;
  tl::optional<QString> state;
  tl::optional<QString> timeZone;
};

struct UserResponseDto {
  Models::UserAvatarColor avatarColor;

  QUuid id;

  QString email;
  QString name;
  QString profileImagePath;

  QDateTime profileChangedAt;
};

struct AssetFaceWithoutPersonResponseDto {
  float boundingBoxX1;
  float boundingBoxX2;
  float boundingBoxY1;
  float boundingBoxY2;

  QUuid id;

  quint64 imageHeight;
  quint64 imageWidth;

  Models::SourceType sourceType;
};

struct PersonWithFacesResponseDto {
  tl::optional<QDate> birthDate;

  tl::optional<QColor> color;

  QVector<AssetFaceWithoutPersonResponseDto> faces;

  QUuid id;

  tl::optional<bool> isFavorite;

  bool isHidden;

  QString name;
  QString thumbnailPath;

  tl::optional<QDateTime> updatedAt;
};

struct AssetStackResponseDto : public DtoStruct {
  Q_GADGET
public:
  quint64 assetCount;
  QUuid id;
  QUuid primaryAssetId;
};

struct TagResponseDto {
  QUuid id;

  tl::optional<QUuid> parentId;

  QString name;
  QString value;
//  tl::optional<QString> color; // TODO know real type

  QDateTime createdAt;
  QDateTime updatedAt;
};

struct ContributorCountResponseDto {
  quint64 assetCount;
  QUuid userId;
};

struct AlbumUserResponseDto {
  Models::AlbumUserRole role;

  UserResponseDto user;
};

struct AssetResponseDto {
  QString checksum;    // base64 encoded sha1 hash
  QDateTime createdAt; // The UTC timestamp when the asset was originally
                       // uploaded to Immich.

  QString deviceAssetId;

  QString deviceId;

  tl::optional<QString> duplicateId;

  quint64 duration;

  tl::optional<ExifResponseDto> exifInfo;

  QDateTime fileCreatedAt; // The actual UTC timestamp when the file was
                           // created/captured, preserving timezone information.
                           // This is the authoritative timestamp for
                           // chronological sorting within timeline groups.
                           // Combined with timezone data, this can be used to
                           // determine the exact moment the photo was taken.

  QDateTime
      fileModifiedAt; // The UTC timestamp when the file was last modified on
                      // the filesystem. This reflects the last time the
                      // physical file was changed, which may be different from
                      // when the photo was originally taken.

  bool hasMetadata;

  QUuid id;

  bool isArchived;
  bool isFavorite;
  bool isOffline;
  bool isTrashed;

  //    tl::optional<QString> libraryId; Deprecated

  tl::optional<QString> livePhotoVideoId;

  QDateTime
      localDateTime; // The local date and time when the photo/video was taken,
                     // derived from EXIF metadata. This represents the
                     // photographer's local time regardless of timezone, stored
                     // as a timezone-agnostic timestamp. Used for timeline
                     // grouping by "local" days and months.

  QString originalFileName;

  tl::optional<QString> originalMimeType;

  QString originalPath;

  UserResponseDto owner;

  QString ownerId;

  QVector<PersonWithFacesResponseDto> people;
  //    Boolean     resized Deprecated;
  tl::optional<AssetStackResponseDto> stack;

  QVector<TagResponseDto> tags;

  tl::optional<QString> thumbhash;

  Models::AssetType type;

  QVector<AssetFaceWithoutPersonResponseDto> unassignedFaces;

  QDateTime updatedAt; // The UTC timestamp when the asset record was last
                       // updated in the database. This is automatically
                       // maintained by the database and reflects when any field
                       // in the asset was last modified.

  Models::AssetVisibility visibility;
};

} // namespace Api
} // namespace Immich

Q_DECLARE_METATYPE(Immich::Api::DtoStruct);
Q_DECLARE_METATYPE(Immich::Api::Models::AssetMediaSize)

#endif // _DTO_H

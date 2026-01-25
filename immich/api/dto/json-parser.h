#ifndef JSONPARSER_H
#define JSONPARSER_H

#include "_dto.h"
#include <QDebug>
#include <QJsonValue>
#include <QString>
#include <QVector>
#include <immich/result/result.h>
#include <tl-optional.h>
#include <type_traits>

namespace Immich {
namespace Api {

class JsonValueParser {
public:
  inline static QDateTime parseIsoTime(QString input) {
    int dot = input.indexOf('.', input.indexOf('T'));
    if (dot >= 0) {
      int msStart = dot + 1;
      int msEnd = msStart;
      while (msEnd < input.size() && input[msEnd].isDigit())
        ++msEnd;

      int msLength = msEnd - msStart;

      if (msLength == 0) {
        input.insert(msStart, "000");
      } else if (msLength == 1) {
        input.insert(msEnd, "00"); // .5 → .500
      } else if (msLength == 2) {
        input.insert(msEnd, "0"); // .57 → .570
      } else if (msLength > 3) {
        input.remove(msStart + 3, msLength - 3); // .5789 → .578
      }
    } else {
      input += ".000";
    }

    return QDateTime::fromString(input, "yyyy-MM-dd'T'HH:mm:ss.zzz");
  };

//  inline static QString uuidStr(const QUuid &uuid) {
//    return uuid.toString().replace("{", "").replace("}", "");
//  };

  //  enum ValueType {
  //    DOUBLE_VALUE,
  //    INT_VALUE,
  //    UINT_VALUE,
  //    QSTRING_VALUE,
  //    BOOL_VALUE
  //  };

  //  template <typename T>
  //  inline static typename std::enable_if<std::is_base_of<DtoStruct,
  //  T>::value,
  //                                        ResultErrored<QVector<T>>>::type
  //  arrayValue(const QJsonValue &pJson) {
  //    if (!pJson.isArray()) {
  //      return ResultErrored<QVector<T>>("jsonObject not array");
  //    }

  //    const auto jsonArr = pJson.toArray();

  //    QVector<T> arr(jsonArr.size());
  //    if (jsonArr.isEmpty())
  //      return ResultErrored<QVector<T>>(arr);

  //    for (int idx = 0; idx < jsonArr.size(); ++idx) {
  //      arr[idx] = T(jsonArr[idx]);

  //      if (!arr[idx].isValid()) {
  //        return ResultErrored<QVector<T>>(arr[idx].errMessage());
  //      }
  //    }
  //  };

  //  template <typename T>
  //  inline static typename std::enable_if<std::is_same<T, int>::value,
  //                                        ResultErrored<QVector<T>>>::type
  //  arrayValue(const QJsonValue &pJson) {
  //    if (!pJson.isArray()) {
  //      return ResultErrored<QVector<T>>("jsonObject not array");
  //    }

  //    const auto jsonArr = pJson.toArray();

  //    QVector<T> arr(jsonArr.size());
  //    if (jsonArr.isEmpty())
  //      return ResultErrored<QVector<T>>(arr);

  //    for (int idx = 0; idx < jsonArr.size(); ++idx) {
  //      if (const auto err = intValue(&arr[idx], jsonArr[idx])) {
  //        return ResultErrored<QVector<int>>(err);
  //      }
  //    }
  //  }

  template <typename T>
  inline static ResultErrored<T>
  doubleValue(const QJsonValue &pJsonValue,
              const bool isNullAsZeroValue = false) {
    if (!isNullAsZeroValue && pJsonValue.isNull())
      return {ResultError("json value is null")};
    if (!pJsonValue.isDouble())
      return {ResultError("json value is not a double")};

    return {static_cast<T>(pJsonValue.toDouble())};
  };

  //  template <typename T>
  //  inline static bool doubleValueOptional(tl::optional<T> *outValue,
  //                                         const QJsonValue &pJsonValue) {
  //    if (!outValue)
  //      return false;

  //    if (!pJsonValue.isNull() && !pJsonValue.isUndefined()) {
  //      *outValue = static_cast<T>(pJsonValue.toDouble());
  //    }
  //    return true;
  //  };

  template <typename T>
  inline static ResultErrored<T>
  intValue(const QJsonValue &pJsonValue, const bool isNullAsZeroValue = false) {
    if (!isNullAsZeroValue && pJsonValue.isNull())
      return {ResultError("json value is null")};
    if (!pJsonValue.isDouble())
      return {ResultError("json value is not a double")};

    return {static_cast<T>(pJsonValue.toInt())};
  };

  template <typename T>
  inline static ResultErrored<T> *
  uintValue(const QJsonValue &pJsonValue,
            const bool isNullAsZeroValue = false) {
    const auto res = intValue<int64_t>(pJsonValue, isNullAsZeroValue);
    if (!res.isSucceeded())
      return res;
    if (res.data() <= 0)
      return new ResultError("uint value is lesser than zero (" +
                             QVariant::fromValue(res.data()).toString() + ")");
    return nullptr;
  };

  inline static ResultErrored<QDate>
  QDateValue(const QJsonValue &pJsonValue, const bool isNullAsZeroValue = false,
             const QString &formatString = "yyyy-MM-dd") {
    if (!isNullAsZeroValue && pJsonValue.isNull())
      return {ResultError("json value is null")};
    if (!pJsonValue.isString())
      return ResultErrored<QDate>(ResultError("json value is not a string"));

    const auto date = QDate::fromString(pJsonValue.toString(), formatString);

    if (date.isNull())
      return ResultErrored<QDate>(ResultError("date is null"));
    if (!date.isValid())
      return ResultErrored<QDate>(ResultError("date is invalid"));

    return ResultErrored<QDate>(date);
  };

  inline static ResultErrored<QDateTime>
  QDateTimeISOValue(const QJsonValue pJsonValue,
                    const bool isNullAsZeroValue = false) {
    if (!isNullAsZeroValue && pJsonValue.isNull())
      return {ResultError("json value is null")};
    if (!pJsonValue.isString())
      return ResultErrored<QDateTime>(
          ResultError("json value is not a string"));

    const auto dateTime = parseIsoTime(pJsonValue.toString());

    if (dateTime.isNull())
      return ResultErrored<QDateTime>(ResultError("date is null"));
    if (!dateTime.isValid())
      return ResultErrored<QDateTime>(ResultError("date is invalid"));

    return ResultErrored<QDateTime>(dateTime);
  }

  //  inline static bool
  //  QDateTimeISOValueOptional(tl::optional<QDateTime> *outValue,
  //                            const QJsonValue pJsonValue) {
  //    if (!outValue)
  //      return false;
  //    if (!pJsonValue.isString()) {
  //      return true;
  //    }
  //    *outValue = parseIsoTime(pJsonValue.toString());
  //    return outValue->value().isValid() && !outValue->value().isNull();
  //  }

  inline static ResultErrored<QUuid>
  QUuidValue(const QJsonValue &pJsonValue,
             const bool isNullAsZeroValue = false) {
    if (!isNullAsZeroValue && pJsonValue.isNull())
      return {ResultError("json value is null")};
    if (!pJsonValue.isString())
      return ResultErrored<QUuid>(ResultError("json value is not a string"));

    const auto qUuid = QUuid(pJsonValue.toString());

    if (qUuid.isNull())
      return ResultErrored<QUuid>(ResultError("value is null"));

    return ResultErrored<QUuid>(qUuid);
  };

  //  inline static bool QUuidValueOptional(tl::optional<QUuid> *outValue,
  //                                        const QJsonValue &pJsonValue) {
  //    if (!outValue)
  //      return false;
  //    if (!pJsonValue.isNull() && !pJsonValue.isUndefined()) {
  //      *outValue = QUuid(pJsonValue.toString());
  //      return !outValue->value().isNull();
  //    }
  //    return true;
  //  };

  inline static ResultErrored<QString>
  QStringValue(const QJsonValue &pJsonValue,
               const bool isNullAsZeroValue = false) {
    if (!isNullAsZeroValue && pJsonValue.isNull())
      return {ResultError("json value is null")};
    if (!pJsonValue.isString())
      return ResultErrored<QString>(ResultError("json value is not a string"));

    return ResultErrored<QString>(pJsonValue.toString());
  };

  //  inline static bool QStringValueOptional(tl::optional<QString> *outValue,
  //                                          const QJsonValue &pJsonValue) {
  //    if (!outValue)
  //      return false;
  //    if (!pJsonValue.isNull() && !pJsonValue.isUndefined()) {
  //      *outValue = pJsonValue.toString();
  //    }
  //    return true;
  //  };

  inline static ResultErrored<bool>
  boolValue(const QJsonValue &pJsonValue,
            const bool isNullAsZeroValue = false) {
    if (!isNullAsZeroValue && pJsonValue.isNull())
      return {ResultError("json value is null")};
    if (!pJsonValue.isBool())
      return new ResultErrored<bool>(ResultError("json value is not a bool"));

    return pJsonValue.toBool();
    ;
  }

  inline static ResultErrored<QVariant>
  prop(const QJsonValue &pJsonValue, const QVariant::Type pType,
       const bool isNullAsZeroValue = false) {
    Q_UNUSED(pJsonValue)
    switch (pType) {
    case QVariant::Invalid:
      return ResultErrored<QVariant>(ResultError("prop type is invalid"));
      break;
    case QVariant::Bool:
      return boolValue(pJsonValue, isNullAsZeroValue).toVariant();
    case QVariant::Int:
      return intValue<int>(pJsonValue, isNullAsZeroValue).toVariant();
    case QVariant::UInt:
      return intValue<uint>(pJsonValue, isNullAsZeroValue).toVariant();
    case QVariant::LongLong:
      return intValue<qint64>(pJsonValue, isNullAsZeroValue).toVariant();
    case QVariant::ULongLong:
      return intValue<quint64>(pJsonValue, isNullAsZeroValue).toVariant();
    case QVariant::Double:
      return doubleValue<double>(pJsonValue, isNullAsZeroValue).toVariant();
      //    case QVariant::Char:
      //      break;
      //    case QVariant::Map:
      //      break;
      //    case QVariant::List:
      //      break;
    case QVariant::String:
      return QStringValue(pJsonValue, isNullAsZeroValue).toVariant();
      //    case QVariant::StringList:
      //      break;
      //    case QVariant::ByteArray:
      //      break;
      //    case QVariant::BitArray:
      //      break;
    case QVariant::Date:
      return QDateValue(pJsonValue, isNullAsZeroValue).toVariant();
      //    case QVariant::Time:
      //      break;
    case QVariant::DateTime:
      return QDateTimeISOValue(pJsonValue, isNullAsZeroValue).toVariant();
      //    case QVariant::Url:
      //      break;
      //    case QVariant::Locale:
      //      break;
      //    case QVariant::Rect:
      //      break;
      //    case QVariant::RectF:
      //      break;
      //    case QVariant::Size:
      //      break;
      //    case QVariant::SizeF:
      //      break;
      //    case QVariant::Line:
      //      break;
      //    case QVariant::LineF:
      //      break;
      //    case QVariant::Point:
      //      break;
      //    case QVariant::PointF:
      //      break;
      //    case QVariant::RegExp:
      //      break;
      //    case QVariant::RegularExpression:
      //      break;
      //    case QVariant::Hash:
      //      break;
      //    case QVariant::EasingCurve:
      //      break;
    case QVariant::Uuid:
      return QUuidValue(pJsonValue, isNullAsZeroValue).toVariant();
      //    case QVariant::ModelIndex:
      //      break;
      //    case QVariant::PersistentModelIndex:
      //      break;
      //    case QVariant::LastCoreType:
      //      break;
      //    case QVariant::Font:
      //      break;
      //    case QVariant::Pixmap:
      //      break;
      //    case QVariant::Brush:
      //      break;
      //    case QVariant::Color:
      //      break;
      //    case QVariant::Palette:
      //      break;
      //    case QVariant::Image:
      //      break;
      //    case QVariant::Polygon:
      //      break;
      //    case QVariant::Region:
      //      break;
      //    case QVariant::Bitmap:
      //      break;
      //    case QVariant::Cursor:
      //      break;
      //    case QVariant::KeySequence:
      //      break;
      //    case QVariant::Pen:
      //      break;
      //    case QVariant::TextLength:
      //      break;
      //    case QVariant::TextFormat:
      //      break;
      //    case QVariant::Matrix:
      //      break;
      //    case QVariant::Transform:
      //      break;
      //    case QVariant::Matrix4x4:
      //      break;
      //    case QVariant::Vector2D:
      //      break;
      //    case QVariant::Vector3D:
      //      break;
      //    case QVariant::Vector4D:
      //      break;
      //    case QVariant::Quaternion:
      //      break;
      //    case QVariant::PolygonF:
      //      break;
      //    case QVariant::Icon:
      //      break;
      //    case QVariant::LastGuiType:
      //      break;
      //    case QVariant::SizePolicy:
      //      break;
      //    case QVariant::UserType:
      //      break;
      //      case QVariant::LastType:
      //          break;
    default: {
      const char *typeName = QMetaType::typeName(static_cast<int>(pType));
      if (typeName && strcmp(typeName, "tl::optional<QString>") == 0) {
        return QStringValue(pJsonValue).toVariant();
      }
      //      int optionalType = qMetaTypeId<tl::optional<QString>>();

      //      if (static_cast<int>(pType) == optionalType)
      //        return QStringValue(pJsonValue).toVariant();

      return {ResultError("unsupported prop type: " +
                          QString::fromLatin1(QVariant::typeToName(pType)))};
    }
    }
  };
};

} // namespace Api
} // namespace Immich

#endif // JSONPARSER_H

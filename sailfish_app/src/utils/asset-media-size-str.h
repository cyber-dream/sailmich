#ifndef ASSETMEDIASIZESTR_H
#define ASSETMEDIASIZESTR_H

#include <OAIAssetMediaSize.h>
#include <QDebug>

inline QString assetMediaSizeToStr(
    const OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize pSize) {
  switch (pSize) {
  case OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize::ORIGINAL:
    return "orginal";
  case OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize::FULLSIZE:
    return "fullsize";
  case OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize::PREVIEW:
    return "preview";
  case OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize::THUMBNAIL:
    return "thumbnail";
  case OpenAPI::OAIAssetMediaSize::eOAIAssetMediaSize::
      INVALID_VALUE_OPENAPI_GENERATED:
    qWarning() << "requested thumbnail size is not setted "
                  "(INVALID_VALUE_OPENAPI_GENERATED)";
    return "";
  default:
    return "";
  }
}

#endif // ASSETMEDIASIZESTR_H

#include "_albums.h"

Immich::Api::Albums::AlbumResponseDto::AlbumResponseDto(
    const QJsonObject &obj) {
  *this = fromJson<AlbumResponseDto>(obj);
}

#include "_base-api.h"
using namespace Immich::Api;

NetworkManager *Base::nm;
QUrl Base::apiUrl;

Base::Base(QObject *parent) : QObject(parent) {
  if (nm == nullptr) {
    nm = new NetworkManager(this);
  }
}

void Base::setAuthToken(const QString &pToken) { nm->setAuthToken(pToken); }

void Base::setApiUrl(const QUrl &url) {
  apiUrl = url;
  if (!apiUrl.toString().endsWith('/')) {
    apiUrl = apiUrl.toString() + '/';
  }
}

#ifndef PARSE_ISO_TIME_H
#define PARSE_ISO_TIME_H

#include <QDate>

inline QDateTime parseIsoTime(QString input) {
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
#endif // PARSE-ISO-TIME_H

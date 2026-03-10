#ifndef QRANDOMGENERATOR_H
#define QRANDOMGENERATOR_H


#include <QtGlobal>
#include <QTime>

class QRandomGenerator {
public:
    // Конструктор по умолчанию
    QRandomGenerator() {}

    // Конструктор из сида (числа)
    QRandomGenerator(uint seed) { qsrand(seed); }

    // Эмуляция глобального генератора
    static QRandomGenerator* global() {
        static QRandomGenerator instance;
        static bool seeded = false;
        if (!seeded) {
            qsrand(static_cast<uint>(QTime::currentTime().msec()));
            seeded = true;
        }
        return &instance;
    }

    // Метод генерации (в Qt5 используем qrand)
    quint32 generate() {
        return static_cast<quint32>(qrand());
    }
};

#endif // QRANDOMGENERATOR_H

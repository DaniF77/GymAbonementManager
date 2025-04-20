#include "abonement.h"
#include <QStringList>


// пустой конструктор
Abonement::Abonement() {}

// конструктор с параметрами
Abonement::Abonement(const QString &name, const QString &type, const QString &endDate)
    : m_name(name), m_type(type), m_endDate(endDate) {}

// геттеры
QString Abonement::getName() const { return m_name; }
QString Abonement::getType() const { return m_type; }
QString Abonement::getEndDate() const { return m_endDate; }

// сеттеры
void Abonement::setName(const QString &name) { m_name = name; }
void Abonement::setType(const QString &type) { m_type = type; }
void Abonement::setEndDate(const QString &endDate) { m_endDate = endDate; }

// собираем строку CSV (разделитель - запятая)
QString Abonement::toCSV() const {
    return m_name + "," + m_type + "," + m_endDate;
}

// создаём абонемент из CSV строки
Abonement Abonement::fromCSV(const QString &line) {
    QStringList parts = line.split(",");
    if (parts.size() != 3) return Abonement(); // если что-то не так — возвращаем пустой
    return Abonement(parts[0], parts[1], parts[2]);
}

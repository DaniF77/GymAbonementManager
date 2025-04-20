#ifndef ABONEMENT_H
#define ABONEMENT_H

#include <QString>

// Класс Абонемент, хранит инфу об одном абонементе
class Abonement
{
public:
    Abonement();
    Abonement(const QString &name, const QString &type, const QString &endDate); // конструктор с данными

    // геттеры
    QString getName() const;
    QString getType() const;
    QString getEndDate() const;

    // сеттеры
    void setName(const QString &name);
    void setType(const QString &type);
    void setEndDate(const QString &endDate);

    // превращаем абонемент в строку CSV
    QString toCSV() const;

    // создаём абонемент из строки CSV
    static Abonement fromCSV(const QString &line);

private:
    QString m_name;
    QString m_type;
    QString m_endDate;
};

#endif // ABONEMENT_H

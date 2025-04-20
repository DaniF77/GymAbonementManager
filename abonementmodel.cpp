#include "abonementmodel.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

// конструктор
AbonementModel::AbonementModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

// кол-во строк = кол-во абонементов
int AbonementModel::rowCount(const QModelIndex &) const {
    return m_abonements.size();
}

// у нас 3 колонки: имя, тип, срок
int AbonementModel::columnCount(const QModelIndex &) const {
    return 3;
}

// что показывать в таблице
QVariant AbonementModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    const Abonement &a = m_abonements.at(index.row());

    switch (index.column()) {
        case 0: return a.getName();
        case 1: return a.getType();
        case 2: return a.getEndDate();
    }

    return QVariant();
}

// названия колонок сверху
QVariant AbonementModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) return QVariant();
    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return "Имя";
            case 1: return "Тип";
            case 2: return "Срок действия";
        }
    }
    return QVariant();
}

// флаги — разрешаем редактировать ячейки
Qt::ItemFlags AbonementModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

// редактирование ячейки
bool AbonementModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    Abonement &a = m_abonements[index.row()];
    switch (index.column()) {
        case 0: a.setName(value.toString()); break;
        case 1: a.setType(value.toString()); break;
        case 2: a.setEndDate(value.toString()); break;
    }

    emit dataChanged(index, index); // говорим таблице обновиться
    return true;
}

// добавление нового абонемента
void AbonementModel::addAbonement(const Abonement &abonement) {
    beginInsertRows(QModelIndex(), m_abonements.size(), m_abonements.size());
    m_abonements.append(abonement);
    endInsertRows();
}

// удаление по номеру строки
void AbonementModel::removeAbonement(int row) {
    if (row < 0 || row >= m_abonements.size()) return;
    beginRemoveRows(QModelIndex(), row, row);
    m_abonements.removeAt(row);
    endRemoveRows();
}

// возвращаем все абонементы
QList<Abonement> AbonementModel::getAllAbonements() const {
    return m_abonements;
}

// замена всего списка абонементов (например, при загрузке из файла)
void AbonementModel::setAbonements(const QList<Abonement> &abonements) {
    beginResetModel();
    m_abonements = abonements;
    endResetModel();
}

// сохранение данных в CSV
bool AbonementModel::saveToFile(const QString &fileName) const {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { // если файл не открылся
        qWarning() << "Не удалось открыть файл для записи:" << file.errorString();
        return false;
    }

    QTextStream out(&file);
    for (const Abonement &a : m_abonements) { // проходим по всем абонементам
        // если в поле есть запятая, заключаем в кавычки и экранируем кавычки внутри
        QString name = a.getName().contains(",") ? "\"" + a.getName().replace("\"", "\"\"") + "\"" : a.getName();
        // аналогично для типа абонемента
        QString type = a.getType().contains(",") ? "\"" + a.getType().replace("\"", "\"\"") + "\"" : a.getType();
        // дата не требует экранирования, поэтому просто присваиваем её
        QString endDate = a.getEndDate();

        // записываем данные в файл с разделителем запятой
        out << name << "," << type << "," << endDate << "\n";
    }

    file.close();
    return true;
}

// загрузка данных из CSV
bool AbonementModel::loadFromFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { // если файл не открылся
        qWarning() << "Не удалось открыть файл для чтения:" << file.errorString();
        return false;
    }

    QList<Abonement> loaded;
    QTextStream in(&file);
    int lineNumber = 0; // счётчик строк в файле
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        lineNumber++;

        // если строка пустая, пропускаем её
        if (line.isEmpty())
            continue;


        if (line.startsWith("\"") && line.endsWith("\"")) {
            line = line.mid(1, line.length() - 2); // удаляем внешние кавычки
        }

        // разбиение строки с учётом кавычек
        QStringList parts = splitCSVLine(line); // функция ниже

        // проверка, что данных 3 части (имя, тип, срок действия)
        if (parts.size() != 3) {
            qWarning() << "Неверный формат в строке" << lineNumber << ":" << line;
            continue; // пропускаем строку с ошибкой
        }

        // создаём абонемент с загруженными данными
        loaded.append(Abonement(parts[0].trimmed(), parts[1].trimmed(), parts[2].trimmed()));
    }

    file.close();

    // если список абонементов пустой
    if (loaded.isEmpty()) {
        qWarning() << "Файл не содержит допустимых записей.";
        return false;
    }

    // обновляем модель абонементов с новыми данными
    setAbonements(loaded);
    return true;
}
// Функция для правильного разделения строки по запятой с учётом кавычек
QStringList AbonementModel::splitCSVLine(const QString &line) const {
    QStringList parts;
    QString currentPart;
    bool insideQuotes = false;

    for (int i = 0; i < line.length(); ++i) {
        QChar c = line[i];

        if (c == '\"') { // если кавычка
            insideQuotes = !insideQuotes; // переключаем состояние флажка
        } else if (c == ',' && !insideQuotes) { // если запятая и не внутри кавычек
            parts.append(currentPart); // добавляем текущую часть
            currentPart.clear(); // очищаем для следующей части
        } else {
            currentPart.append(c); // добавляем символ к текущей части
        }
    }
    parts.append(currentPart); // добавляем последнюю часть

    return parts;
}




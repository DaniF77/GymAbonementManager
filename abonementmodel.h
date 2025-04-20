#ifndef ABONEMENTMODEL_H
#define ABONEMENTMODEL_H

#include <QAbstractTableModel>
#include "abonement.h"

// Кастомная модель, чтобы данные показывать в QTableView
class AbonementModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit AbonementModel(QObject *parent = nullptr);

    bool saveToFile(const QString& fileName) const;
    bool loadFromFile(const QString& fileName);


    // сколько строк и колонок в таблице
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    // данные для каждой ячейки
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // названия столбцов
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // флаги — редактируемые ли ячейки и т.д.
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // изменение данных через интерфейс
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    // добавляем, удаляем, сетим абонементов
    void addAbonement(const Abonement &abonement);
    void removeAbonement(int row);
    QList<Abonement> getAllAbonements() const;
    void setAbonements(const QList<Abonement> &abonements);

private:
    QList<Abonement> m_abonements; // храним всех наших пацанов здесь
    QStringList splitCSVLine(const QString &line) const;
};

#endif // ABONEMENTMODEL_H

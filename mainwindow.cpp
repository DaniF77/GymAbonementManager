#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "abonementmodel.h"
#include "abonement.h"
#include <QRegularExpression>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // создаём модель данных и подключаем её к таблице
    model = new AbonementModel(this);
    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); //авто подгон колонок


    // Кнопка "Добавить"
    connect(ui->addButton, &QPushButton::clicked, this, [=]() {
        QString name = ui->nameEdit->text();
        QString type = ui->typeComboBox->currentText(); // Получаем выбранный тип из ComboBox
        QString date = ui->dateEdit->text();

        // Проверка на заполненность всех полей
        if (name.isEmpty() || type.isEmpty() || date.isEmpty()) {
            showPopup("Пожалуйста, заполните все поля.");
            return;
        }

        // Проверка типа (теперь через ComboBox)
        QStringList validTypes = {"Gold", "Silver", "Platinum"};
        if (!validTypes.contains(type)) {
            showPopup("Тип должен быть Gold, Silver или Platinum.");
            return;
        }

        // Проверка формата даты
        QRegExp dateRegex("^\\d{4}-\\d{2}-\\d{2}$");
        if (!dateRegex.exactMatch(date)) {
            showPopup("Дата должна быть в формате ГГГГ-ММ-ДД.");
            return;
        }

        QStringList parts = date.split("-");
        int month = parts[1].toInt();
        int day = parts[2].toInt();

        // Проверка месяца
        if (month < 1 || month > 12) {
            showPopup("Месяц должен быть от 01 до 12.");
            return;
        }

        // Проверка дня
        if (day < 1 || day > 31) {
            showPopup("День должен быть от 01 до 31.");
            return;
        }

        // добавляем абонемент в модель
        model->addAbonement(Abonement(name, type, date));

        // очищаем поля
        ui->nameEdit->clear();
        ui->typeComboBox->setCurrentIndex(0); // сбрасываем выбор в ComboBox
        ui->dateEdit->clear();
    });



    // Кнопка "Удалить"
    connect(ui->removeButton, &QPushButton::clicked, this, [this]() {
        QModelIndex index = ui->tableView->currentIndex();
        if (index.isValid()) {
            model->removeAbonement(index.row());
        } else {
            showPopup("Сначала выберите строку для удаления");
        }
    });

    // Кнопка "Сохранить"
    connect(ui->saveButton, &QPushButton::clicked, this, [this]() {
        QString fileName = QFileDialog::getSaveFileName(this, "Сохранить файл", "", "CSV файлы (*.csv)");
        if (!fileName.isEmpty()) {
            if (!model->saveToFile(fileName)) {
                showPopup("Ошибка при сохранении файла");
            }
        }
    });

    // Кнопка "Загрузить"
    connect(ui->loadButton, &QPushButton::clicked, this, [this]() {
        QString fileName = QFileDialog::getOpenFileName(this, "Открыть файл", "", "CSV файлы (*.csv)");
        if (!fileName.isEmpty()) {
            if (!model->loadFromFile(fileName)) {
                showPopup("Ошибка при загрузке файла");
            }
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Всплывающее уведомление снизу окна (исчезает само)
void MainWindow::showPopup(const QString &message) {
    QLabel *popup = new QLabel(message, this);
    popup->setStyleSheet("QLabel { background-color: #333; color: white; padding: 8px; border-radius: 5px; }");
    popup->setWindowFlags(Qt::ToolTip);
    popup->adjustSize();

    // Центрируем по горизонтали, снизу окна
    int x = (width() - popup->width()) / 2;
    int y = height() - popup->height() - 50;
    popup->move(x, y);
    popup->show();

    // Убираем через 2 секунды
    QTimer::singleShot(2000, popup, &QWidget::deleteLater);
}

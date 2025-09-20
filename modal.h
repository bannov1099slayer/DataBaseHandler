#ifndef MODAL_H
#define MODAL_H

#include <QDialog>
#include <QApplication>
#include <QCloseEvent>
#include <QValidator>
#include <QDebug>
//#include <mainwindow.h>

namespace Ui {
class Modal;
}
/**
@brief Класс модального окна
*/
class Modal : public QDialog
{
    Q_OBJECT

public:
/**
@brief Конструктор
@detailed В конструкторе устанавливаются начальные настройки для работы формы модального окна: поведение кнопок
Сохранить и Отмена и настройки лайнэдита индекса i таблицы main_table
@param parent Экземпляр класса QWidget
*/
    explicit Modal(QWidget *parent = nullptr);
    ~Modal();
    int i;         // элемент main_table
    std::string r; // элемент main_table
    std::string t; // элемент main_table
/**
@brief Метод установки параметров i,r,t
@detailed Устанавливаюся значения, которые пришли из главного окна, в лайнэдиты модального окна
@param i Значение i
@param r Значение r
@param t Значение t
*/
    void set_i_r_t_data(const QString& i,const QString& r,const QString& t);
signals:
/**
@brief Сигнал отправки значений параметров i,r,t в главное окно
@param i Значение i
@param r Значение r
@param t Значение t
*/
    void sendData(int, std::string, std::string);

public slots:
/**
@brief Сигнал считывания параметров i,r,t из лайнэдитов модального окна
@detailed Считываются значения с формы и отправляется сигнал в главное окно
@param i Значение i
@param r Значение r
@param t Значение t
*/
    void onSendData();// данные с формы отправляем в основное окно
private:
    Ui::Modal *ui;
};

#endif // MODAL_H

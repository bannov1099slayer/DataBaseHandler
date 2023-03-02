#include "modal.h"
#include "ui_modal.h"

Modal::Modal(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Modal)
{
    ui->setupUi(this);
    ui->m_i_lineEdit->setValidator(new QIntValidator(this)); // можно вводить только значения типа int
    ui->m_i_lineEdit->setEnabled(false);
    ui->m_i_lineEdit->setStyleSheet("background: white;");

    connect(ui->saveButton,    SIGNAL(clicked()),       this, SLOT(onSendData()));
    connect(ui->saveButton,    &QPushButton::clicked,   this, &QDialog::close);
    connect(ui->discardButton, &QPushButton::clicked,   this, &QDialog::close);

}

Modal::~Modal()
{
    delete ui;
}

//-----------------------------------------------------------------------------------------------------------------------
// Действие по нажатию кнопки Сохранить (берутся значения с формы и подставляются в запрос)
//-----------------------------------------------------------------------------------------------------------------------
void Modal::onSendData()
{
    i = ui->m_i_lineEdit->text().toInt();
    qDebug () << "on modal i = " << i;

    r = ui->m_r_lineEdit->text().toStdString();
    qDebug () << "on modal r = " << r.c_str();

    t = ui->m_t_lineEdit->text().toStdString();
    qDebug () << "on modal t = " << t.c_str();

    emit sendData(i,r,t);
}

void Modal::set_i_r_t_data(const QString& i,const QString& r,const QString& t)
{
    qDebug () << "set_i_r_t_data";

    // устанавливаем значения в лайнэдиты модального окна
    ui->m_i_lineEdit->setText(i);

    ui->m_r_lineEdit->setText(r);

    ui->m_t_lineEdit->setText(t);

}

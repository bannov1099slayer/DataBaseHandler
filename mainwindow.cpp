#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);       // крайний столбец растягивается по ширине

    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);   // выбор всей строки при нажатии на неё

    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);  // установка режима выбора

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);    // нельзя редактировать поля в таблице напрямую

    //randomKeyGen(); // первичная генерация ключа р00
}

MainWindow::~MainWindow()
{
    delete ui;
}

//-----------------------------------------------------------------------------------------------------------------------
// Инициализация первичных значений
//-----------------------------------------------------------------------------------------------------------------------
void MainWindow::init()
{
    qDebug () << "init";

    m_db = QSqlDatabase::addDatabase("QPSQL");// подключаем драйвер для работы с PostgreSQL
    m_db.setHostName("localhost");
    m_db.setDatabaseName("mydb1");            // название БД, в которой хранятся наши таблицы
    m_db.setUserName("BANNOV");
    m_db.setPassword("1");

    if (!m_db.open())
    {
        QMessageBox::critical(this, QObject::tr("Database Error"), m_db.lastError().text());
        qDebug () << "Database Error";
        return;
    }
    else qDebug () << "connection exists";

    updateTable();                           // отображаем текущую таблицу на форме
}

//-----------------------------------------------------------------------------------------------------------------------
// Обновление таблицы и вывод её на форму
//-----------------------------------------------------------------------------------------------------------------------
void MainWindow::updateTable()
{
    qDebug () << "updateTable";

    ui->tableWidget->setRowCount(0); // очистка таблицы

    if(m_db.open() == true)
    {
        QSqlQuery query(m_db);

        query.exec("SELECT i,qual_table.p01,t FROM main_table INNER JOIN qual_table ON main_table.r = qual_table.p00 ORDER BY i");

        checkQueryProblems(query); // проверка проблем с запросом

        while(query.next())
        {
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());

            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 0, new QTableWidgetItem(query.value(0).toString()));

            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 1, new QTableWidgetItem(query.value(1).toString()));

            ui->tableWidget->setItem(ui->tableWidget->rowCount()-1, 2, new QTableWidgetItem(query.value(2).toString()));

            ui->tableWidget->item(ui->tableWidget->rowCount()-1,1)->setToolTip(query.value(1).toString()); // установка всплывающих окон для отображения информации в ячейке
                                                                                                           // (если будет слишком много текста)

            ui->tableWidget->item(ui->tableWidget->rowCount()-1,2)->setToolTip(query.value(2).toString());
        }
    }

    else qDebug()  << "no connection to db";
}

//-----------------------------------------------------------------------------------------------------------------------
// Действие по нажатию кнопки Добавить (добавляются новые строки для редактирования в две таблицы)
//-----------------------------------------------------------------------------------------------------------------------
void MainWindow::on_addButton_clicked()
{
    qDebug () << "addButton_clicked";

    bool isTableExist = m_db.tables().contains("main_table");

    if(!isTableExist)
    {
        qDebug()  << "table does not exist";
        return; // выходим из метода - если таблицы нет, то и модальное окно не должно создаваться(нельзя добавть строку в никуда)
    }
    else qDebug() << "table exists";

    std::string key_p00 =  randomKeyGen();// генерируем ключ p00

    QSqlQuery query(m_db);

    query.prepare("insert into qual_table (p00,p01) values (:index,'введите текст');");// заносим в таблицу qual_table сгенерированный ключ и строку "введите текст" в p01
    query.bindValue(":index", key_p00.c_str());
    query.exec();
    checkQueryProblems(query);

    QSqlQuery query1(m_db);

    query1.prepare("insert into main_table (r,t) values (:r,'введите текст');"); // заносим в таблицу main_table сгенерированный ключ и строку "введите текст" в t
    query1.bindValue(":r", key_p00.c_str());
    query1.exec();
    checkQueryProblems(query1);

    updateTable();
}

//-----------------------------------------------------------------------------------------------------------------------
// Действие по нажатию кнопки Удалить (удаляется выбранная строка)
//-----------------------------------------------------------------------------------------------------------------------
void MainWindow::on_deleteButton_clicked()
{
    qDebug () << "deleteButton_clicked";

    if (ui->tableWidget->selectionModel()->selectedRows().count() != 0) //выбрана строка или нет
    {
        QString r;
        QSqlQuery query1(m_db);

        query1.prepare("SELECT r FROM main_table WHERE i = :index;");
        query1.bindValue(":index", ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text());// берём значение из первой ячейки строки
        query1.exec();
        checkQueryProblems(query1);

        r = query1.first();
        r = query1.value(0).toString();
        std::string str1 = r.toStdString();

        qDebug() << "r from main table" << str1.c_str();

        QSqlQuery query2(m_db);

        query2.prepare("DELETE FROM main_table WHERE i = :index;");
        query2.bindValue(":index", ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text());// берём значение из первой ячейки строки
        query2.exec();
        checkQueryProblems(query2);

        QSqlQuery query3(m_db);

        query3.prepare("DELETE FROM qual_table  WHERE p00 = :r;");
        query3.bindValue(":r", str1.c_str());
        query3.exec();
        checkQueryProblems(query3);
    }
    else
    {
        QMessageBox::warning(this, tr("НЕ ВЫБРАНА СТРОКА"),tr("ВЫБЕРИТЕ СТРОКУ"));
        qDebug() <<"Problems - set_i_r_t_data";
    }
    updateTable(); // вывод текущей таблицы
}

//-----------------------------------------------------------------------------------------------------------------------
// Действие по нажатию кнопки Редактировать (открывается модальное окно)
//-----------------------------------------------------------------------------------------------------------------------
void MainWindow::on_editButton_clicked()
{
    qDebug () << "editButton_clicked";

    Modal window_redaction;// экземпляр модального окна

    window_redaction.setWindowTitle("Редактирование записи");
    window_redaction.setFixedSize(618,345);

    if (ui->tableWidget->selectionModel()->selectedRows().count() != 0) //выбрана строка или нет
    {
        QString i = ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text();
        QString r = ui->tableWidget->item(ui->tableWidget->currentRow(),1)->text();
        QString t = ui->tableWidget->item(ui->tableWidget->currentRow(),2)->text();

        window_redaction.set_i_r_t_data(i,r,t); // в модальное окно заносятся значения из выбранной строки

        connect(&window_redaction,SIGNAL(sendData(int, std::string, std::string)),this,SLOT(set_data(int, std::string, std::string)));

        window_redaction.exec();
    }
    else
    {
        QMessageBox::warning(this, tr("НЕ ВЫБРАНА СТРОКА"),tr("ВЫБЕРИТЕ СТРОКУ"));
        qDebug() <<"Problems - set_i_r_t_data";
    }

    updateTable();
}

//-----------------------------------------------------------------------------------------------------------------------
// Проверка ошибок в запросе (в случае ошибки выведет окно с указанием проблемы)
//-----------------------------------------------------------------------------------------------------------------------
void MainWindow::checkQueryProblems(QSqlQuery query)
{
    qDebug () << "checkQueryProblems";

    if (!query.isActive())
    {
        QMessageBox::warning(this, tr("Query Error"),query.lastError().text() );
        qDebug() <<"Problems";
    }
}

//-----------------------------------------------------------------------------------------------------------------------
// Редактирование значений в БД
//-----------------------------------------------------------------------------------------------------------------------
void MainWindow::set_data(int i1, std::string r1,std::string t1)
{
    qDebug () << "set_data";
    qDebug() <<"on mainwindow i = " << i1;          // значение, которое пришло с формы модального окна
    qDebug() <<"on mainwindow r = " << r1.c_str();  // значение, которое пришло с формы модального окна
    qDebug() <<"on mainwindow t = " << t1.c_str();  // значение, которое пришло с формы модального окна

    // Проверяем, есть ли введённый индекс и точно выбрана строка
    if (checkRowIndexExistance(i1) && ui->tableWidget->selectionModel()->selectedRows().count() != 0)
    {
        QSqlQuery query;
        query.prepare("UPDATE qual_table SET p01 = :p01  WHERE p00 = (SELECT r FROM main_table WHERE i = :index_main);");
        query.bindValue(":p01"    , r1.c_str());
        query.bindValue(":index_main", ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text());
        query.exec();
        checkQueryProblems(query);

        QSqlQuery query2;
        query2.prepare("UPDATE main_table SET  t = :t WHERE i = :index;");
        query2.bindValue(":t"    , t1.c_str());// t, которое ввели в модальном окне
        query2.bindValue(":index", ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text()); // i, которое ввели в модальном окне
        query2.exec();

        checkQueryProblems(query2);
    }

    else
    {
        QMessageBox::warning(this, tr("ИНДЕКС ОТСУТСТВУЕТ В ТАБЛИЦЕ"),tr("Выберите нужный индекс"));
        qDebug() <<"Problems - set_data";
    }
}

//-----------------------------------------------------------------------------------------------------------------------
// Проверка индекса
//-----------------------------------------------------------------------------------------------------------------------
bool MainWindow::checkRowIndexExistance(int index)
{
    qDebug () << "checkRowIndexExistance";

    for (int j = 0 ; j < ui->tableWidget->rowCount();j++)
    {
        if (index ==  ui->tableWidget->item(j,0)->text().toInt()) return true;
    }
    return false;
}

//-----------------------------------------------------------------------------------------------------------------------
// Генератор ключей для столбца p00
//-----------------------------------------------------------------------------------------------------------------------
std::string MainWindow::randomKeyGen()
{
    qDebug () << "randomKeyGen";

    std::string str;

    std::mt19937 gen(time(NULL));

    std::uniform_int_distribution <int> uid1(2,2);

    int num = uid1(gen);

    int r;

    std::uniform_int_distribution <int> uid2('0','z');

    for(int m = 0; m < num; m++)
    {
        do {r = uid2(gen);} while ( (r> '9' && r < 'A') || (r > 'Z' && r < 'a'));
        str+=r;
    }

    qDebug() << str.c_str();
    return str;
}





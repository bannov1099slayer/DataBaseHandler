# Простой обработчик базы данных
Написать sql-скрипт, в котором создаются две связанные таблицы:
+ qual_table с полями p00 тип vatcher(2) и p01 тип text;
+ main_table с полями: i тип int4 первичный ключ, r тип varchar(2) ссылается на таблицу qual_table поле p00, t тип text.

В скрипте добавить команды для заполнения этих таблиц тестовыми данными.

Используя библиотеку Qt написать программу для редактирования данных в таблице main_table.
В программе должны быть реализованы следующие функции:
+ Просмотр записей таблицы main_table;
+ Редактирование записей в таблице;
+ Удаление записей из таблицы;
+ Вставка новой записи в таблицу.

---

## SQL-скрипт на создание таблиц и первичное заполнение данными

```sql
create table qual_table (p00 varchar(2) PRIMARY KEY NOT NULL, p01 text)
						 
insert into qual_table (p00,p01)
values ('c3','s3'),
		('c4','s4'),
		('c5','s5');
		
create table main_table (i int4  PRIMARY KEY Generated Always as Identity,
						 r varchar(2) NOT NULL,
						 t text,
						 FOREIGN KEY (r)  REFERENCES qual_table (p00));
						 
insert into main_table (r,t)
values ((SELECT p00 FROM qual_table WHERE p01 ='s3'),'aboba'),
		((SELECT p00 FROM qual_table WHERE p01 ='s4'),'text2'),
        ((SELECT p00 FROM qual_table WHERE p01 ='s5'),'text3');
```
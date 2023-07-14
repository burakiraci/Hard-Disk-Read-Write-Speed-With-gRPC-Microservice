#include "form.h"
#include "ui_form.h"
#include <iostream>
Form::Form(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Form)
{
	ui->setupUi(this);

}

Form::~Form()
{
	delete ui;
}

void Form::on_pushButton_Start_clicked()
{
	isRunning = true;
}

void Form::on_pushButton_Stop_clicked()
{
	isRunning = false;
	ui->textEdit_readspeed->clear();
	ui->textEdit_writespeed->clear();
}



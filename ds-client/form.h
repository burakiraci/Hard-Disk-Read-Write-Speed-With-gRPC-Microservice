#ifndef FORM_H
#define FORM_H

#include <QWidget>

namespace Ui {
class Form;
}

class Form : public QWidget
{
	Q_OBJECT
	friend class SpeedUptadeHandler;
public:
	explicit Form(QWidget *parent = nullptr);
	~Form();
	Ui::Form* getUI() {
			return ui;
		}
	bool isRunning = false;

	bool isRun  (){
		return isRunning;
	}
private:
	Ui::Form *ui;

private slots:
	void on_pushButton_Start_clicked();
	void on_pushButton_Stop_clicked();

};

#endif // FORM_H

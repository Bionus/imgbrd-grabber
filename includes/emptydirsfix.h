#ifndef EMPTYDIRSFIX_H
#define EMPTYDIRSFIX_H

#include <QDialog>

namespace Ui {
    class EmptyDirsFix;
}

class EmptyDirsFix : public QDialog
{
    Q_OBJECT

	public:
		explicit EmptyDirsFix(QWidget *parent = 0);
		~EmptyDirsFix();

	public slots:
		void next();

	private:
		Ui::EmptyDirsFix *ui;
};

#endif // EMPTYDIRSFIX_H

#ifndef FILENAMEWINDOW_H
#define FILENAMEWINDOW_H

#include <QDialog>
#include <QMessageBox>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerjavascript.h>



namespace Ui
{
	class FilenameWindow;
}



class FilenameWindow : public QDialog
{
	Q_OBJECT
	
	public:
		explicit FilenameWindow(QString value = "", QWidget *parent = 0);
		~FilenameWindow();

	public slots:
		void on_lineClassic_textChanged(QString);
		void on_buttonHelpClassic_clicked();
		void on_buttonHelpJavascript_clicked();
		void send();

	signals:
		void validated(QString);

	private:
		Ui::FilenameWindow *ui;
		QsciScintilla *m_scintilla;
};

#endif // FILENAMEWINDOW_H

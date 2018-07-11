#ifndef FILENAME_WINDOW_H
#define FILENAME_WINDOW_H

#include <QCloseEvent>
#include <QDialog>

#if defined(USE_QSCINTILLA)
	#include <Qsci/qscilexerjavascript.h>
	#include <Qsci/qsciscintilla.h>
#else
	#include <QTextEdit>
#endif


namespace Ui
{
	class FilenameWindow;
}


class Profile;

class FilenameWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit FilenameWindow(Profile *profile, QString value = "", QWidget *parent = nullptr);
		~FilenameWindow() override;
		QString format() const;

	public slots:
		void on_lineClassic_textChanged(QString);
		void on_buttonHelpClassic_clicked();
		void on_buttonHelpJavascript_clicked();
		void send();
		void done(int r) override;

	signals:
		void validated(const QString &format);

	private:
		Ui::FilenameWindow *ui;
		Profile *m_profile;
		#if defined(USE_QSCINTILLA)
			QsciScintilla *m_scintilla;
		#else
			QTextEdit *m_scintilla;
		#endif
};

#endif // FILENAME_WINDOW_H

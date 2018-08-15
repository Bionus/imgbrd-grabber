#include "settings/filename-window.h"
#include <QDesktopServices>
#include <QMessageBox>
#include <QRegExp>
#include <QRegularExpression>
#include <ui_filename-window.h>
#include "models/filename.h"
#include "models/image.h"
#include "models/profile.h"
#include "models/site.h"


FilenameWindow::FilenameWindow(Profile *profile, QString value, QWidget *parent)
	: QDialog(parent), ui(new Ui::FilenameWindow), m_profile(profile)
{
	ui->setupUi(this);

	#if defined(USE_QSCINTILLA)
		m_scintilla = new QsciScintilla(this);
		QsciLexerJavaScript *lexer = new QsciLexerJavaScript(this);
		m_scintilla->setLexer(lexer);
	#else
		m_scintilla = new QTextEdit(this);
	#endif

	connect(ui->radioJavascript, &QRadioButton::toggled, m_scintilla, &QWidget::setEnabled);
	ui->verticalLayout->insertWidget(ui->verticalLayout->count() - 1, m_scintilla);

	if (value.startsWith("javascript:"))
	{
		value = value.right(value.length() - 11);
		m_scintilla->setText(value);
		ui->lineClassic->setEnabled(false);
		ui->radioJavascript->toggle();
	}
	else
	{
		ui->lineClassic->setText(value);
		m_scintilla->setEnabled(false);
		ui->radioClassic->toggle();
	}

	connect(this, &QDialog::accepted, this, &FilenameWindow::send);
}

FilenameWindow::~FilenameWindow()
{
	delete ui;
}

void FilenameWindow::on_lineClassic_textChanged(QString text)
{
	QString message;
	Filename fn(text);
	fn.isValid(m_profile, &message);
	ui->labelValidator->setText(message);

	text = text.replace("\\", "\\\\").replace("'", "\\'");

	QRegExp date("%date:format=([^%]+)%");
	int pos = 0;
	while ((pos = date.indexIn(text, pos)) != -1)
	{
		QString cap = date.cap(1);
		QString format;
		for (const QChar &c : cap)
		{
			if (c == 'Y')
			{ format += "' + date.getFullYear() + '"; }
			else if (c == 'M')
			{ format += "' + date.getMonth() + '"; }
			else if (c == 'd')
			{ format += "' + date.getDate() + '"; }
			else if (c == 'h')
			{ format += "' + date.getHours() + '"; }
			else if (c == 'm')
			{ format += "' + date.getMinutes() + '"; }
			else if (c == 's')
			{ format += "' + date.getSeconds() + '"; }
			else
			{ format += c; }
		}

		text = text.left(pos) + format + text.mid(pos + date.matchedLength());
		pos += date.matchedLength();
	}

	QString value = "'" + text.replace(QRegularExpression("%([^%]+)%"), "' + \\1 + '").remove(" + '' + ").trimmed() + "'";
	if (value.startsWith("' + "))
	{ value = value.right(value.length() - 4); }
	if (value.startsWith("'' + "))
	{ value = value.right(value.length() - 5); }
	if (value.endsWith(" + '"))
	{ value = value.left(value.length() - 4); }
	if (value.endsWith(" + ''"))
	{ value = value.left(value.length() - 5); }

	m_scintilla->setText(value);
}

void FilenameWindow::on_buttonHelpClassic_clicked()
{
	QDesktopServices::openUrl(QUrl(QString(PROJECT_GITHUB_URL) + "/wiki/Filename"));
}
void FilenameWindow::on_buttonHelpJavascript_clicked()
{
	QDesktopServices::openUrl(QUrl(QString(PROJECT_GITHUB_URL) + "/wiki/Filename#javascript"));
}

QString FilenameWindow::format() const
{
	if (ui->radioJavascript->isChecked())
	{
		#if defined(USE_QSCINTILLA)
			return "javascript:" + m_scintilla->text();
		#else
			return "javascript:" + m_scintilla->toPlainText();
		#endif
	}

	return ui->lineClassic->text();
}

void FilenameWindow::done(int r)
{
	QMap<QString, Site*> sites = m_profile->getSites();

	if (QDialog::Accepted == r && ui->radioJavascript->isChecked() && !sites.isEmpty())
	{
		Site *site = sites.first();

		QMap<QString, QString> info;
		info.insert("tags_general", "general_1 general_2");
		info.insert("tags_artist", "artist_1 artist_2");
		info.insert("tags_model", "model_1 model_2");
		info.insert("tags_character", "character_1 character_2");
		info.insert("tags_copyright", "copyright_1 copyright_2");

		Image image(site, info, m_profile);
		QStringList det = image.path(format(), QString());

		if (det.isEmpty())
		{
			const int reply = QMessageBox::question(this, tr("Warning"), tr("You script contains error, are you sure you want to save it?"), QMessageBox::Yes | QMessageBox::Cancel);
			if (reply == QMessageBox::Cancel)
			{
				return;
			}
		}
	}

	QDialog::done(r);
}

void FilenameWindow::send()
{
	emit validated(format());
}

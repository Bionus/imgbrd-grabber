#include "functions.h"
#include "filenamewindow.h"
#include "ui_filenamewindow.h"
#include "models/image.h"
#include "models/site.h"
#include "models/filename.h"


FilenameWindow::FilenameWindow(QString value, QWidget *parent) : QDialog(parent), ui(new Ui::FilenameWindow)
{
	ui->setupUi(this);

	#if USE_QSCINTILLA
		m_scintilla = new QsciScintilla(this);
		QsciLexerJavaScript *lexer = new QsciLexerJavaScript(this);
		m_scintilla->setLexer(lexer);
	#else
		m_scintilla = new QTextEdit(this);
	#endif

	connect(ui->radioJavascript, SIGNAL(toggled(bool)), m_scintilla, SLOT(setEnabled(bool)));
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
	fn.isValid(&message);
	ui->labelValidator->setText(message);

	QRegExp date("%date:format=([^%]+)%");
	int pos = 0;
	text = text.replace("\\", "\\\\").replace("'", "\\'");

	while ((pos = date.indexIn(text, pos)) != -1)
	{
		QString cap = date.cap(1);
		QString format = "";
		for (int i = 0; i < cap.length(); ++i)
		{
			QChar c = cap.at(i);
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

	QString value = "'"+text.replace(QRegExp("%([^%]+)%"), "' + \\1 + '").remove(" + '' + ").trimmed()+"'";
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
	QMessageBox::information(this, tr("Aide"), tr(
		"Symboles disponibles : <i>%artist%</i>, <i>%general%</i>, <i>%copyright%</i>, <i>%character%</i>, <i>%all%</i>, <i>%filename%</i>, <i>%ext%</i>, <i>%rating%</i>, <i>%website%</i>, <i>%md5%</i>.<br/><br/>"
		"<i>%artist%</i> : tags de nom d'artiste<br/>"
		"<i>%general%</i> : tags d'image génériques<br/>"
		"<i>%copyright%</i> : tags de copyright, en général le nom de l'anime/manga<br/>"
		"<i>%character%</i> : tags indiquant quels personnages sont présents sur l'image<br/>"
		"<i>%all%</i> : tous les tags<br/>"
		"<i>%filename%</i> : nom du fichier sur le serveur, en général une chaîne alphanumérique<br/>"
		"<i>%ext%</i> : extension de l'image<br/>"
		"<i>%rating%</i> : Questionable, Safe ou Explicit<br/>"
		"<i>%score%</i> : le score de l'image<br/>"
		"<i>%website%</i> : url du site de l'image<br/>"
		"<i>%websitename%</i> : nom du site de l'image<br/>"
		"<i>%md5%</i> : code unique de l'image, composé de 32 caractères alphanumériques<br/>"
		"<i>%id%</i> : identifiant de l'image sur un site donné<br/>"
		"<i>%search%</i> : tags de la recherche<br/>"
		"<i>%search_n%</i> : n-ième tag de la recherche<br/>"
		"<i>%date%</i> : date d'ajout de l'image au format dd-MM-yyyy HH.mm<br/>"
		"<i>%date:format%</i> : date d'ajout de l'image au format donné<br/><br/>"
		"Vous pouvez aussi utiliser les structures conditionnelles. Pour plus d'informations, cliquez <a href=\"%s\">ici</a>.").arg("https://github.com/Bionus/imgbrd-grabber/wiki/Filename")
	);
}

void FilenameWindow::on_buttonHelpJavascript_clicked()
{
	QMessageBox::information(this, tr("Aide"), tr(
		"Variables disponibles : <i>artist</i>, <i>general</i>, <i>copyright</i>, <i>character</i>, <i>all</i>, <i>filename</i>, <i>ext</i>, <i>rating</i>, <i>website</i>, <i>md5</i>.<br/><br/>"
		"<i>artist</i> : tags de nom d'artiste<br/>"
		"<i>general</i> : tags d'image génériques<br/>"
		"<i>copyright</i> : tags de copyright, en général le nom de l'anime/manga<br/>"
		"<i>character</i> : tags indiquant quels personnages sont présents sur l'image<br/>"
		"<i>all</i> : tous les tags<br/>"
		"<i>filename</i> : nom du fichier sur le serveur, en général une chaîne alphanumérique<br/>"
		"<i>ext</i> : extension de l'image<br/>"
		"<i>rating</i> : Questionable, Safe ou Explicit<br/>"
		"<i>score</i> : le score de l'image<br/>"
		"<i>website</i> : url du site de l'image<br/>"
		"<i>websitename</i> : nom du site de l'image<br/>"
		"<i>md5</i> : code unique de l'image, composé de 32 caractères alphanumériques<br/>"
		"<i>id</i> : identifiant de l'image sur un site donné<br/>"
		"<i>search</i> : tags de la recherche<br/>"
		"<i>search_n</i> : n-ième tag de la recherche<br/>"
		"<i>date</i> : date d'ajout de l'image au format dd-MM-yyyy HH.mm<br/><br/>"
		"Pour plus d'informations sur le nommage Javascript, cliquez <a href=\"%s\">ici</a>.").arg("https://github.com/Bionus/imgbrd-grabber/wiki/Filename#javascript")
	);
}

QString FilenameWindow::format()
{
	if (ui->radioJavascript->isChecked())
	{
		#if USE_QSCINTILLA
			return "javascript:" + m_scintilla->text();
		#else
			return "javascript:" + m_scintilla->toPlainText();
		#endif
	}

	return ui->lineClassic->text();
}

void FilenameWindow::done(int r)
{
	QMap<QString, Site*> *sites = Site::getAllSites();

	if (QDialog::Accepted == r && ui->radioJavascript->isChecked() && !sites->isEmpty())
	{
		Site *site = sites->value(sites->keys().first());

		QMap<QString, QString> info;
		info.insert("site", QString::number((qintptr)site));
		info.insert("tags_general", "general_1 general_2");
		info.insert("tags_artist", "artist_1 artist_2");
		info.insert("tags_model", "model_1 model_2");
		info.insert("tags_character", "character_1 character_2");
		info.insert("tags_copyright", "copyright_1 copyright_2");

		Image image(site, info);
		QStringList det = image.path(format(), "");

		if (det.isEmpty())
		{
			QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Attention"), tr("Votre script contient des erreurs, êtes-vous sûr de vouloir l'enregistrer ?"), QMessageBox::Yes | QMessageBox::Cancel);
			if (reply == QMessageBox::Cancel)
			{
				return;
			}
		}
	}

	QDialog::done(r);
	return;
}

void FilenameWindow::send()
{
	emit validated(format());
}

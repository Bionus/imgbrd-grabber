#include "functions.h"
#include "filenamewindow.h"
#include "ui_filenamewindow.h"

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
		ui->radioJavascript->toggle();
	}
	else
	{
		ui->lineClassic->setText(value);
		ui->radioClassic->toggle();
	}

	connect(this, SIGNAL(accepted()), this, SLOT(send()));
}

FilenameWindow::~FilenameWindow()
{
	delete ui;
}

void FilenameWindow::on_lineClassic_textChanged(QString text)
{
	ui->labelValidator->setText(validateFilename(text));

	QString value = "'"+text.replace("\\", "\\\\").replace("'", "\\'").replace(QRegExp("%([^%]+)%"), "' + \\1 + '").remove(" + '' + ").trimmed()+"'";
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
	QMessageBox::information(this, tr("Grabber") + " - " + tr("Aide"), tr(
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
		"Vous pouvez aussi utiliser les structures conditionnelles. Pour plus d'informations, cliquez <a href=\"http://code.google.com/p/imgbrd-grabber/wiki/Filename\">ici</a>.")
	);
}

void FilenameWindow::on_buttonHelpJavascript_clicked()
{
	QMessageBox::information(this, tr("Grabber") + " - " + tr("Aide"), tr(
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
		"Pour plus d'informations sur le nommage Javascript, cliquez <a href=\"http://code.google.com/p/imgbrd-grabber/wiki/Filename#Javascript\">ici</a>.")
	);
}

void FilenameWindow::send()
{
	#if USE_QSCINTILLA
		QString text = m_scintilla->text();
	#else
		QString text = m_scintilla->toPlainText();
	#endif
	emit validated(ui->radioJavascript->isChecked() ? "javascript:"+text : text);
}

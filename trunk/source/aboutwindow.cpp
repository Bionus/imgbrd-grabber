#include "aboutwindow.h"
#include "ui_aboutwindow.h"



int version2int(QString version)
{
	version = version.replace(".", "");
	int beta = version.endsWith("a") ? 1 : (version.endsWith("b") ? 2 : 9);
	return version.replace("a", "").replace("b", "").toInt() * 10 + beta;
}

aboutWindow::aboutWindow(QString version, QWidget *parent) : QDialog(parent), ui(new Ui::aboutWindow)
{
	ui->setupUi(this);

	ui->labelCurrent->setText(version);
	m_version = version2int(version);

	QNetworkAccessManager *m = new QNetworkAccessManager();
	connect(m, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
	m->get(QNetworkRequest(QUrl("http://imgbrd-grabber.googlecode.com/svn/trunk/VERSION")));

	setFixedSize(400, 228);
}

aboutWindow::~aboutWindow()
{
	delete ui;
}

void aboutWindow::finished(QNetworkReply *r)
{
	QString l = r->readAll(), last = l;
	int latest = version2int(last);
	if (latest <= m_version)
	{ ui->labelMessage->setText("<p style=\"font-size:8pt; font-style:italic; color:#808080;\">"+tr("Grabber est à jour")+"</p>"); }
	else
	
	{ ui->labelMessage->setText("<p style=\"font-size:8pt; font-style:italic; color:#808080;\">"+tr("Une nouvelle version est disponible : %1").arg(l)+"</p>"); }
}

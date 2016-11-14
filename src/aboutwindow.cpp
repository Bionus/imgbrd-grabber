#include <QtGlobal>
#include "aboutwindow.h"
#include "ui_aboutwindow.h"



int version2int(QString version)
{
	version = version.replace(".", "");
	int beta = version.endsWith("a") ? 1 : (version.endsWith("b") ? 2 : 9);
	return version.replace("a", "").replace("b", "").toInt() * 10 + beta;
}

AboutWindow::AboutWindow(QString version, QWidget *parent) : QDialog(parent), ui(new Ui::AboutWindow)
{
	ui->setupUi(this);

	ui->labelCurrent->setText(version);
	m_version = version2int(version);

	QNetworkAccessManager *m = new QNetworkAccessManager();
	connect(m, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
	m->get(QNetworkRequest(QUrl("https://api.github.com/repos/Bionus/imgbrd-grabber/tags")));

	setFixedSize(400, 228);
}

AboutWindow::~AboutWindow()
{
	delete ui;
}

void AboutWindow::finished(QNetworkReply *r)
{
	QString last = r->readAll();

	QRegExp rx("\"name\":\\s*\"v([^\"]+)\"");
	QList<int> list;
	int pos = 0;
	while ((pos = rx.indexIn(last, pos)) != -1)
	{
		list << version2int(rx.cap(1));
		pos += rx.matchedLength();
	}
	qSort(list);

	int latest = list.empty() ? 0 : list.last();
	QString msg = latest <= m_version ? tr("Grabber is up to date") : tr("A new version is available: %1").arg(last);
	ui->labelMessage->setText("<p style=\"font-size:8pt; font-style:italic; color:#808080;\">" + msg + "</p>");
}

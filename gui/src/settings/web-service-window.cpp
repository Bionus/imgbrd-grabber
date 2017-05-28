#include "web-service-window.h"
#include "ui_web-service-window.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QFile>
#include "functions.h"


WebServiceWindow::WebServiceWindow(int index, const ReverseSearchEngine *webService, QWidget *parent)
	: QDialog(parent), ui(new Ui::WebServiceWindow), m_index(index)
{
	ui->setupUi(this);

	if (webService != nullptr)
	{
		ui->lineName->setText(webService->name());
		ui->lineUrl->setText(webService->tpl());
	}

	connect(this, SIGNAL(accepted()), this, SLOT(getFavicon()));
}

WebServiceWindow::~WebServiceWindow()
{
	delete ui;
}


void WebServiceWindow::getFavicon()
{
	QUrl url(ui->lineUrl->text());
	QString favicon = url.scheme() + "://" + url.authority() + "/favicon.ico";

	auto nam = new QNetworkAccessManager(this);
	m_faviconReply = nam->get(QNetworkRequest(QUrl(favicon)));
	connect(m_faviconReply, &QNetworkReply::finished, this, &WebServiceWindow::save);
}

void WebServiceWindow::save()
{
	// Save favicon contents
	QByteArray data;
	if (m_faviconReply->error() == QNetworkReply::NoError)
	{
		data = m_faviconReply->readAll();
		m_faviconReply->deleteLater();
	}

	// Emit the success signal
	QString name = ui->lineName->text();
	QString url = ui->lineUrl->text();
	emit validated(m_index, ReverseSearchEngine("", name, url), data);
}

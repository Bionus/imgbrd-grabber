#include "settings/web-service-window.h"
#include <QNetworkRequest>
#include <ui_web-service-window.h>
#include "functions.h"
#include "network/network-manager.h"
#include "network/network-reply.h"
#include "reverse-search/reverse-search-engine.h"


WebServiceWindow::WebServiceWindow(const ReverseSearchEngine *webService, QWidget *parent)
	: QDialog(parent), ui(new Ui::WebServiceWindow), m_webService(webService), m_faviconReply(nullptr)
{
	ui->setupUi(this);

	m_networkAccessManager = new NetworkManager(this);

	if (webService != nullptr) {
		ui->lineName->setText(webService->name());
		ui->lineUrl->setText(webService->tpl());
	}

	connect(this, &QDialog::accepted, this, &WebServiceWindow::getFavicon);
}

WebServiceWindow::~WebServiceWindow()
{
	delete ui;
}


void WebServiceWindow::getFavicon()
{
	QUrl url(ui->lineUrl->text());
	const QString favicon = url.scheme() + "://" + url.authority() + "/favicon.ico";

	m_faviconReply = m_networkAccessManager->get(QNetworkRequest(QUrl(favicon)));
	connect(m_faviconReply, &NetworkReply::finished, this, &WebServiceWindow::faviconReceived);
}

void WebServiceWindow::faviconReceived()
{
	// Check redirection
	QUrl redirection = m_faviconReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redirection.isEmpty()) {
		m_faviconReply = m_networkAccessManager->get(QNetworkRequest(QUrl(redirection)));
		connect(m_faviconReply, &NetworkReply::finished, this, &WebServiceWindow::faviconReceived);
		return;
	}

	save();
}

void WebServiceWindow::save()
{
	int id = -1, order = 0;
	if (m_webService != nullptr) {
		id = m_webService->id();
		order = m_webService->order();
	}

	// Save favicon contents
	QByteArray faviconData;
	if (m_faviconReply->error() == NetworkReply::NetworkError::NoError) {
		faviconData = m_faviconReply->readAll();
		m_faviconReply->deleteLater();
	}

	// Emit the success signal
	const QString name = ui->lineName->text();
	const QString url = ui->lineUrl->text();
	emit validated(ReverseSearchEngine(id, QString(), name, url, order), faviconData);

	deleteLater();
}

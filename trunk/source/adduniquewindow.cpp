#include "adduniquewindow.h"
#include "functions.h"
#include "json.h"
#include <QtXml>

AddUniqueWindow::AddUniqueWindow(QMap<QString,QStringList> sites, mainWindow *parent) : QWidget(parent), m_parent(parent), m_sites(sites)
{
	QVBoxLayout *layout = new QVBoxLayout;
		QFormLayout *formLayout = new QFormLayout;
			m_comboSite = new QComboBox;
				m_comboSite->addItems(m_sites.keys());
				formLayout->addRow(tr("&Site"), m_comboSite);
			m_lineId = new QLineEdit;
				formLayout->addRow(tr("&Id"), m_lineId);
			m_lineMd5 = new QLineEdit;
				formLayout->addRow(tr("&Md5"), m_lineMd5);
			layout->addLayout(formLayout);
		QHBoxLayout *layoutButtons = new QHBoxLayout;
			QPushButton *buttonOk = new QPushButton(tr("Ok"));
				connect(buttonOk, SIGNAL(clicked()), this, SLOT(ok()));
				layoutButtons->addWidget(buttonOk);
			QPushButton *buttonClose = new QPushButton(tr("Fermer"));
				connect(buttonClose, SIGNAL(clicked()), this, SLOT(close()));
				layoutButtons->addWidget(buttonClose);
			layout->addLayout(layoutButtons);
	this->setLayout(layout);
	this->setWindowIcon(QIcon(":/images/icon.ico"));
	this->setWindowTitle(tr("Grabber")+" - "+tr("Ajouter image"));
	this->setWindowFlags(Qt::Window);
	this->resize(QSize(200, 0));
}

void AddUniqueWindow::ok()
{
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
	QString url = m_sites[m_comboSite->currentText()].at(2);
	url.replace("{page}", "1");
	url.replace("{tags}", (m_lineId->text().isEmpty() ? "md5:"+m_lineMd5->text() : "id:"+m_lineId->text()));
	url.replace("{limit}", "1");
	manager->get(QNetworkRequest(QUrl(url)));
}
void AddUniqueWindow::replyFinished(QNetworkReply *r)
{
	QStringList infos = QStringList() << "id" << "md5" << "rating" << "tags" << "file_url";
	QMap<QString,QString> values;
	QString site = m_comboSite->currentText(), source = r->readAll();
	if (m_sites[site].at(0) == "xml")
	{
		QDomDocument doc;
		QString errorMsg;
		int errorLine, errorColumn;
		if (!doc.setContent(source, false, &errorMsg, &errorLine, &errorColumn))
		{ error(this, tr("Erreur lors de l'analyse du fichier XML : %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn))); }
		QDomElement docElem = doc.documentElement();
		QDomNodeList nodeList = docElem.elementsByTagName("post");
		if (nodeList.count() > 0)
		{
			for (int i = 0; i < infos.count(); i++)
			{ values[infos.at(i)] = nodeList.at(0).attributes().namedItem(infos.at(i)).nodeValue(); }
			values["site"] = site;
		}
	}
	else if (m_sites[site].at(0) == "json")
	{
		QVariant src = Json::parse(source);
		QMap<QString, QVariant> sc;
		if (!src.isNull())
		{
			QList<QVariant> sourc = src.toList();
			sc = sourc.at(0).toMap();
			for (int i = 0; i < infos.count(); i++)
			{ values[infos.at(i)] = sc.value(infos.at(i)).toString(); }
			values["site"] = site;
		}
	}
	else if (m_sites[site].at(0) == "regex")
	{
		QRegExp rx(m_sites[site].at(6));
		QStringList order = m_sites[site].at(7).split('|');
		rx.setMinimal(true);
		rx.indexIn(source, 0);
		for (int i = 0; i < order.size(); i++)
		{ values[order.at(i)] = rx.cap(i+1); }
		values["file_url"] = values["preview_url"];
			values["file_url"].remove("preview/");
		values["site"] = site;
	}
	m_parent->batchAddUnique(values);
	this->close();
}

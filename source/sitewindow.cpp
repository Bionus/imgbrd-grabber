#include "sitewindow.h"
#include "ui_sitewindow.h"



siteWindow::siteWindow(QStringMapMap *sites, QWidget *parent) : QDialog(parent), ui(new Ui::siteWindow), m_sites(sites)
{
	ui->setupUi(this);

	QStringList types;
	QString name;
	for (int i = 0; i < sites->count(); i++)
	{
		name = sites->value(sites->keys().at(i))["Name"];
		if (!types.contains(name))
		{
			types.append(name);
			ui->comboBox->addItem(QIcon(savePath("sites/"+name.toLower()+"/icon.png")), name);
		}
	}

	ui->comboBox->setDisabled(true);
	ui->checkBox->setChecked(true);
}

siteWindow::~siteWindow()
{
	delete ui;
}

void siteWindow::accept()
{
	QString url = ui->lineEdit->text();
	if (url.startsWith("http://"))
	{ url.remove("http://"); }
	if (url.endsWith("/"))
	{ url = url.left(url.size()-1); }

	QString type, name;
	QStringList types;
	if (ui->checkBox->isChecked())
	{
		QNetworkAccessManager *manager = new QNetworkAccessManager(this);
		for (int i = 0; i < m_sites->count(); i++)
		{
			name = m_sites->value(m_sites->keys().at(i))["Name"].toLower();
			if (!types.contains(name))
			{
				QStringMap map = m_sites->value(m_sites->keys().at(i));
				types.append(name);
				QString curr = map["Selected"];
				curr[0] = curr[0].toUpper();
				QNetworkReply *reply = manager->get(QNetworkRequest("http://"+url+map["Urls/"+curr+"/Tags"]));
				QEventLoop loop;
					connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
				loop.exec();
				if (reply->error() == 0)
				{
					type = name;
					break;
				}
			}
		}
		if (type.isEmpty())
		{
			error(this, tr("Impossible de deviner le type du site. Êtes-vous sûr de l'url ?"));
			ui->comboBox->setDisabled(false);
			ui->checkBox->setChecked(false);
			return;
		}
	}
	else
	{ type = ui->comboBox->currentText().toLower(); }

	QFile f(savePath("sites/"+type+"/sites.txt"));
	f.open(QIODevice::ReadOnly);
		QString sites = f.readAll();
	f.close();
	sites.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
	QStringList stes = sites.split("\r\n");
	stes.append(url);
	stes.removeDuplicates();
	stes.sort();
	f.open(QIODevice::WriteOnly);
		f.write(stes.join("\r\n").toAscii());
	f.close();

	for (int i = 0; i < m_sites->count(); i++)
	{
		name = m_sites->value(m_sites->keys().at(i))["Name"].toLower();
		if (name == type)
		{
			QStringMap map = m_sites->value(m_sites->keys().at(i));
			QString curr = map["Selected"];
			curr[0] = curr[0].toUpper();
			map["Urls/Selected/Tags"] = "http://"+url+map["Urls/"+curr+"/Tags"];
			map["Urls/Selected/Popular"] = "http://"+url+map["Urls/"+curr+"/Popular"];
			m_sites->insert(url, map);
			break;
		}
	}

	emit accepted();
	close();
}

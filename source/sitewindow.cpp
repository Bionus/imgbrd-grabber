#include "sitewindow.h"
#include "ui_sitewindow.h"
#include "mainwindow.h"

extern mainWindow *_mainwindow;



siteWindow::siteWindow(QMap<QString,Site*> *sites, QWidget *parent) : QDialog(parent), ui(new Ui::siteWindow), m_sites(sites)
{
	ui->setupUi(this);

	QStringList types;
	QString type;
	for (int i = 0; i < sites->count(); i++)
	{
		type = sites->value(sites->keys().at(i))->type();
		if (!types.contains(type))
		{
			types.append(type);
			ui->comboBox->addItem(QIcon(savePath("sites/"+type+"/icon.png")), type);
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
			name = m_sites->value(m_sites->keys().at(i))->type();
			if (!types.contains(name))
			{
				Site *map = m_sites->value(m_sites->keys().at(i));
				types.append(name);
				QString curr = map->value("Selected");
				curr[0] = curr[0].toUpper();
				QNetworkReply *reply = manager->get(QNetworkRequest("http://"+url+map->value("Urls/"+curr+"/Tags")));
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
		name = m_sites->value(m_sites->keys().at(i))->value("Name");
		if (name == type)
		{
			Site *map = m_sites->value(m_sites->keys().at(i));
			QString curr = map->value("Selected");
			curr[0] = curr[0].toUpper();
			map->insert("Urls/Selected/Tags", "http://"+url+map->value("Urls/"+curr+"/Tags"));
			map->insert("Urls/Selected/Popular", "http://"+url+map->value("Urls/"+curr+"/Popular"));
			m_sites->insert(url, map);
			break;
		}
	}

	_mainwindow->loadSites();

	emit accepted();
	close();
}

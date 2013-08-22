#include "sitewindow.h"
#include "ui_sitewindow.h"
#include "mainwindow.h"

extern mainWindow *_mainwindow;



siteWindow::siteWindow(QMap<QString,Site*> *sites, QWidget *parent) : QDialog(parent), ui(new Ui::siteWindow), m_sites(sites)
{
	ui->setupUi(this);
	ui->progressBar->hide();

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
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

	QString url = ui->lineEdit->text();
	if (url.startsWith("http://"))
	{ url.remove("http://"); }
	if (url.endsWith("/"))
	{ url = url.left(url.size()-1); }

	QString type, name;
	QStringList checked;
	if (ui->checkBox->isChecked())
	{
		for (int i = 0; i < m_sites->count(); i++)
		{
			name = m_sites->value(m_sites->keys().at(i))->type();
			if (!checked.contains(name))
			{ checked.append(name); }
		}
		ui->progressBar->setValue(0);
		ui->progressBar->setMaximum(checked.count());
		ui->progressBar->show();
		checked.clear();
		QNetworkAccessManager *manager = new QNetworkAccessManager(this);
		for (int i = 0; i < m_sites->count(); i++)
		{
			name = m_sites->value(m_sites->keys().at(i))->type();
			if (!checked.contains(name))
			{
				Site *map = m_sites->value(m_sites->keys().at(i));
				checked.append(name);
				if (map->contains("Check/Url") && map->contains("Check/Regex"))
				{
					QString curr = map->value("Selected");
					curr[0] = curr[0].toUpper();
					QNetworkReply *reply = manager->get(QNetworkRequest("http://"+url+map->value("Check/Url")));
					QEventLoop loop;
						connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
					loop.exec();
					QString source = reply->readAll();
					if (reply->error() == 0)
					{
						QRegExp rx(map->value("Check/Regex"));
						if (rx.indexIn(source) != -1)
						{
							type = name;
							break;
						}
					}
				}
				ui->progressBar->setValue(checked.size());
			}
		}
		if (type.isEmpty())
		{
			error(this, tr("Impossible de deviner le type du site. Êtes-vous sûr de l'url ?"));
			ui->comboBox->setDisabled(false);
			ui->checkBox->setChecked(false);
			ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
			ui->progressBar->hide();
			return;
		}
		ui->progressBar->hide();
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
		f.write(stes.join("\r\n").toLatin1());
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

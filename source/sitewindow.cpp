#include "sitewindow.h"
#include "ui_sitewindow.h"
#include "mainwindow.h"
#include "functions.h"

extern mainWindow *_mainwindow;



siteWindow::siteWindow(QMap<QString,Site*> *sites, QWidget *parent)
	: QDialog(parent), ui(new Ui::siteWindow)
{
	Q_UNUSED(sites);

	ui->setupUi(this);
	ui->progressBar->hide();
	m_models = new QList<Site*>();

	QStringList dir = QDir(savePath("sites")).entryList(QDir::Dirs | QDir::NoDotAndDotDot);

	for (int i = 0; i < dir.count(); i++)
	{
		QFile file(savePath("sites/"+dir.at(i)+"/model.xml"));
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QString source = file.readAll();
			QDomDocument doc;
			QString errorMsg;
			int errorLine, errorColumn;
			if (!doc.setContent(source, false, &errorMsg, &errorLine, &errorColumn))
			{ log(tr("Erreur lors de l'analyse du fichier XML : %1 (%2 - %3).").arg(errorMsg, QString::number(errorLine), QString::number(errorColumn)), Error); }
			else
			{
				QDomElement docElem = doc.documentElement();
				QMap<QString,QString> detals = domToMap(docElem);
				detals["Model"] = dir[i];
				Site *site = new Site(dir[i], dir[i], detals);
				m_models->append(site);
			}
			file.close();
		}
	}

	QStringList types;
	for (Site *site : *m_models)
	{
		if (!types.contains(site->type()))
		{
			types.append(site->type());
			ui->comboBox->addItem(QIcon(savePath("sites/"+site->type()+"/icon.png")), site->type());
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
		ui->progressBar->setValue(0);
		ui->progressBar->setMaximum(m_models->count());
		ui->progressBar->show();

		for (Site *map : *m_models)
		{
			if (!checked.contains(map->type()))
			{
				checked.append(map->type());
				if (map->contains("Check/Url") && map->contains("Check/Regex"))
				{
					QString curr = map->value("Selected");
					curr[0] = curr[0].toUpper();
					QNetworkReply *reply = map->get("http://"+url+map->value("Check/Url"));
					QEventLoop loop;
						connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
					loop.exec();
					QString source = reply->readAll();
					if (reply->error() == 0)
					{
						QRegExp rx(map->value("Check/Regex"));
						if (rx.indexIn(source) != -1)
						{
							type = map->type();
							break;
						}
					}
					else
					{ log(tr("Erreur lors de la récupération de la page de test : %1.").arg(reply->errorString()), Error); }
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

	_mainwindow->loadSites();

	emit accepted();
	close();
}

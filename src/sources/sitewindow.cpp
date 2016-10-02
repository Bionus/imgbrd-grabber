#include "sitewindow.h"
#include "ui_sitewindow.h"
#include "mainwindow.h"
#include "functions.h"
#include "models/source.h"

extern mainWindow *_mainwindow;



siteWindow::siteWindow(QMap<QString,Site*> *sites, QWidget *parent)
	: QDialog(parent), ui(new Ui::siteWindow)
{
	Q_UNUSED(sites);

	ui->setupUi(this);
	ui->progressBar->hide();

	m_sources = Source::getAllSources();
	for (Source *source : *m_sources)
		ui->comboBox->addItem(QIcon(savePath("sites/" + source->getName() + "/icon.png")), source->getName());

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
	if (url.startsWith("https://"))
	{ url.remove("https://"); }
	if (url.endsWith("/"))
	{ url = url.left(url.size()-1); }

	QString type, name;
	QStringList checked;
	if (ui->checkBox->isChecked())
	{
		ui->progressBar->setValue(0);
		ui->progressBar->setMaximum(m_sources->count());
		ui->progressBar->show();

		for (Source *source : *m_sources)
		{
			if (!checked.contains(source->getName()) && !source->getSites().isEmpty())
			{
				checked.append(source->getName());
				Site *map = source->getSites().first();
				if (map->contains("Check/Url") && map->contains("Check/Regex"))
				{
					QString curr = map->value("Selected");
					curr[0] = curr[0].toUpper();

					QUrl getUrl("http://" + url + map->value("Check/Url"));
					QNetworkReply *reply;
					do
					{
						reply = map->get(getUrl);
						QEventLoop loop;
							connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
						loop.exec();

						getUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
					} while (!getUrl.isEmpty());
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
	QStringList stes = sites.split("\r\n", QString::SkipEmptyParts);
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

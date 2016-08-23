#include <QSettings>
#include <QDir>
#include <QDirIterator>
#include <QCryptographicHash>
#include <QMessageBox>
#include "blacklist-fix-1.h"
#include "blacklist-fix-2.h"
#include "functions.h"
#include "ui_blacklist-fix-1.h"
#include "models/page.h"



BlacklistFix1::BlacklistFix1(QMap<QString,Site*> sites, QWidget *parent) : QDialog(parent), ui(new Ui::BlacklistFix1), m_sites(sites)
{
	ui->setupUi(this);

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	ui->lineFolder->setText(settings.value("Save/path").toString());
	ui->lineFilename->setText(settings.value("Save/filename").toString());
	ui->lineBlacklist->setText(settings.value("blacklistedtags").toString());
	ui->comboSource->addItems(m_sites.keys());
	ui->progressBar->hide();

	resize(size().width(), 0);
}

BlacklistFix1::~BlacklistFix1()
{
	delete ui;
}

void BlacklistFix1::on_buttonCancel_clicked()
{
	emit rejected();
	close();
}

void BlacklistFix1::on_buttonContinue_clicked()
{
	ui->buttonContinue->setEnabled(false);
	m_details.clear();

	// Check that directory exists
	QDir dir(ui->lineFolder->text());
	if (!dir.exists())
	{
		error(this, tr("Ce dossier n'existe pas."));
		ui->buttonContinue->setEnabled(true);
		return;
	}

	// Make sure the input is valid
	if (!ui->radioForce->isChecked() && !ui->lineFilename->text().contains("%md5%"))
	{
		error(this, tr("Si vous voulez récupérer le MD5 depuis le nom de fichier, vous devez include le token %md5% dans celui-ci."));
		ui->buttonContinue->setEnabled(true);
		return;
	}

	// Get all files from the destination directory
	typedef QPair<QString,QString> QStringPair;
	QVector<QStringPair> files = QVector<QStringPair>();
	QDirIterator it(dir, QDirIterator::Subdirectories);
	while (it.hasNext())
	{
		it.next();
		if (!it.fileInfo().isDir())
		{
			int len = it.filePath().length() - dir.absolutePath().length() - 1;
			files.append(QPair<QString,QString>(it.filePath().right(len), it.filePath()));
		}
	}

	// Parse all files
	for (QStringPair file : files)
	{
		QString md5 = "";
		if (ui->radioForce->isChecked())
		{
			QFile fle(file.second);
			fle.open(QFile::ReadOnly);
			md5 = QCryptographicHash::hash(fle.readAll(), QCryptographicHash::Md5).toHex();
		}
		else
		{
			QRegExp regx("%([^%]*)%");
			QString reg = QRegExp::escape(ui->lineFilename->text());
			int pos = 0, cur = 0, id = -1;
			while ((pos = regx.indexIn(ui->lineFilename->text(), pos)) != -1)
			{
				pos += regx.matchedLength();
				reg.replace(regx.cap(0), "(.+)");
				if (regx.cap(1) == "md5")
				{ id = cur; }
				cur++;
			}
			QRegExp rx(reg);
			rx.setMinimal(true);
			pos = 0;
			while ((pos = rx.indexIn(file.first, pos)) != -1)
			{
				pos += rx.matchedLength();
				md5 = rx.cap(id + 1);
			}
		}

		QMap<QString,QString> det;
		det.insert("md5", md5);
		det.insert("path", file.first);
		det.insert("path_full", file.second);
		m_details.append(det);
	}

	int reponse = QMessageBox::question(this, tr("Réparateur de liste noire"), tr("Vous vous apprêtez à télécharger les informations de %n image(s). Êtes-vous sûr de vouloir continuer ?", "", m_details.size()), QMessageBox::Yes | QMessageBox::No);
	if (reponse == QMessageBox::Yes)
	{
		// Show progresss bar
		ui->progressBar->setValue(0);
		ui->progressBar->setMaximum(files.size());
		ui->progressBar->show();

		getAll();
	}
}

void BlacklistFix1::getAll(Page *p)
{
	if (p != nullptr && p->images().size() > 0)
	{
		Image *img = p->images().at(0);
		m_getAll[img->md5()].insert("tags", img->tagsString().join(" "));
		ui->progressBar->setValue(ui->progressBar->value() + 1);
	}

	if (!m_details.empty())
	{
		QMap<QString, QString> det = m_details.takeFirst();
		m_getAll.insert(det.value("md5"), det);

		Page *page = new Page(m_sites.value(ui->comboSource->currentText()), m_sites.values(), QStringList("md5:" + det.value("md5")), 1, 1);
		connect(page, SIGNAL(finishedLoading(Page*)), this, SLOT(getAll(Page*)));
		page->load();
	}
	else
	{
		BlacklistFix2 *bf2 = new BlacklistFix2(m_getAll.values(), ui->lineBlacklist->text().split(" "));
		close();
		bf2->show();
	}
}

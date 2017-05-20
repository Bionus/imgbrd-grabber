#include <QSettings>
#include <QDir>
#include <QDirIterator>
#include <QCryptographicHash>
#include <QMessageBox>
#include "rename-existing-1.h"
#include "rename-existing-2.h"
#include "ui_rename-existing-1.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/image.h"
#include "helpers.h"


RenameExisting1::RenameExisting1(Profile *profile, QMap<QString,Site*> sites, QWidget *parent)
	: QDialog(parent), ui(new Ui::RenameExisting1), m_profile(profile), m_sites(sites)
{
	ui->setupUi(this);

	QSettings *settings = profile->getSettings();
	ui->lineFolder->setText(settings->value("Save/path").toString());
	ui->lineFilenameOrigin->setText(settings->value("Save/filename").toString());
	ui->lineFilenameDestination->setText(settings->value("Save/filename").toString());
	ui->comboSource->addItems(m_sites.keys());
	ui->progressBar->hide();

	resize(size().width(), 0);
}

RenameExisting1::~RenameExisting1()
{
	delete ui;
}

void RenameExisting1::on_buttonCancel_clicked()
{
	emit rejected();
	close();
}

void RenameExisting1::on_buttonContinue_clicked()
{
	ui->buttonContinue->setEnabled(false);
	m_details.clear();
	m_getTags.clear();

	// Check that directory exists
	QDir dir(ui->lineFolder->text());
	if (!dir.exists())
	{
		error(this, tr("This folder does not exist."));
		ui->buttonContinue->setEnabled(true);
		return;
	}

	// Make sure the input is valid
	if (!ui->radioForce->isChecked() && !ui->lineFilenameOrigin->text().contains("%md5%"))
	{
		error(this, tr("If you want to get the MD5 from the filename, you have to include the %md5% token in it."));
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
			QString reg = QRegExp::escape(ui->lineFilenameOrigin->text());
			int pos = 0, cur = 0, id = -1;
			while ((pos = regx.indexIn(ui->lineFilenameOrigin->text(), pos)) != -1)
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

		if (!md5.isEmpty())
		{
			QMap<QString,QString> det;
			det.insert("md5", md5);
			det.insert("path", QDir::toNativeSeparators(file.first));
			det.insert("path_full", QDir::toNativeSeparators(file.second));
			m_details.append(det);
		}
	}

	// Check if filename requires details
	m_filename.setFormat(ui->lineFilenameDestination->text());
	m_needDetails = m_filename.needExactTags(m_sites.value(ui->comboSource->currentText()));

	int reponse = QMessageBox::question(this, tr("Rename existing images"), tr("You are about to download information from %n image(s). Are you sure you want to continue?", "", m_details.size()), QMessageBox::Yes | QMessageBox::No);
	if (reponse == QMessageBox::Yes)
	{
		// Show progresss bar
		ui->progressBar->setValue(0);
		ui->progressBar->setMaximum(m_details.size());
		ui->progressBar->show();

		loadNext();
	}
	else
	{
		ui->buttonContinue->setEnabled(true);
	}
}

void RenameExisting1::getAll(Page *p)
{
	if (!p->images().isEmpty())
	{
		QSharedPointer<Image> img = p->images().at(0);

		if (m_needDetails)
		{
			m_getTags.append(img);
		}
		else
		{
			m_getAll[img->md5()].second = img->path(ui->lineFilenameDestination->text(), ui->lineFolder->text(), 0, true, false, true, true, true).first();
			ui->progressBar->setValue(ui->progressBar->value() + 1);
		}
	}
	else
	{
		ui->progressBar->setValue(ui->progressBar->value() + 1);
	}

	loadNext();
}

void RenameExisting1::getTags()
{
	Image *img = dynamic_cast<Image*>(sender());

	m_getAll[img->md5()].second = img->path(ui->lineFilenameDestination->text(), ui->lineFolder->text(), 0, true, false, true, true, true).first();
	ui->progressBar->setValue(ui->progressBar->value() + 1);

	loadNext();
}

void RenameExisting1::loadNext()
{
	if (!m_details.isEmpty())
	{
		QMap<QString,QString> det = m_details.takeFirst();
		m_getAll.insert(det.value("md5"), QPair<QString,QString>(det.value("path_full"), ""));

		Page *page = new Page(m_profile, m_sites.value(ui->comboSource->currentText()), m_sites.values(), QStringList("md5:" + det.value("md5")), 1, 1);
		connect(page, &Page::finishedLoading, this, &RenameExisting1::getAll);
		page->load();

		return;
	}

	if (!m_getTags.isEmpty())
	{
		QSharedPointer<Image> img = m_getTags.takeFirst();
		connect(img.data(), &Image::finishedLoadingTags, this, &RenameExisting1::getTags);
		img->loadDetails();

		return;
	}

	RenameExisting2 *nextStep = new RenameExisting2(m_getAll.values(), ui->lineFolder->text(), parentWidget());
	close();
	nextStep->show();
}

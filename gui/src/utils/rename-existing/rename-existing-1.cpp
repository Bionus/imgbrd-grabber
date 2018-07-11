#include "utils/rename-existing/rename-existing-1.h"
#include <QCryptographicHash>
#include <QDir>
#include <QDirIterator>
#include <QMessageBox>
#include <QSettings>
#include <ui_rename-existing-1.h>
#include "functions.h"
#include "helpers.h"
#include "models/image.h"
#include "models/page.h"
#include "models/profile.h"
#include "utils/rename-existing/rename-existing-2.h"


RenameExisting1::RenameExisting1(Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::RenameExisting1), m_profile(profile), m_sites(profile->getSites()), m_needDetails(false)
{
	ui->setupUi(this);

	QSettings *settings = profile->getSettings();
	ui->lineFolder->setText(settings->value("Save/path").toString());
	ui->lineFilenameOrigin->setText(settings->value("Save/filename").toString());
	ui->lineSuffixes->setText(getExternalLogFilesSuffixes(profile->getSettings()).join(", "));
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
	if (!ui->radioForce->isChecked() && !ui->lineFilenameOrigin->text().contains(QStringLiteral("%md5%")))
	{
		error(this, tr("If you want to get the MD5 from the filename, you have to include the %md5% token in it."));
		ui->buttonContinue->setEnabled(true);
		return;
	}

	// Suffixes
	QStringList suffixes = ui->lineSuffixes->text().split(',');
	for (QString &suffix : suffixes)
		suffix = suffix.trimmed();

	// Get all files from the destination directory
	auto files = listFilesFromDirectory(dir, suffixes);

	// Parse all files
	for (const auto &file : files)
	{
		const QString fileName = file.first;
		const QString path = dir.absoluteFilePath(fileName);

		QString md5;
		if (ui->radioForce->isChecked())
		{
			QFile fle(path);
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
				reg.replace(regx.cap(0), QStringLiteral("(.+)"));
				if (regx.cap(1) == QLatin1String("md5"))
				{ id = cur; }
				cur++;
			}
			QRegExp rx(reg);
			rx.setMinimal(true);
			pos = 0;
			while ((pos = rx.indexIn(fileName, pos)) != -1)
			{
				pos += rx.matchedLength();
				md5 = rx.cap(id + 1);
			}
		}

		if (!md5.isEmpty())
		{
			RenameExistingFile det;
			det.md5 = md5;
			det.path = QDir::toNativeSeparators(path);
			if (!file.second.isEmpty()) {
				QStringList children;
				children.reserve(file.second.count());
				for (const QString &child : file.second)
				{ children.append(QDir::toNativeSeparators(dir.absoluteFilePath(child))); }
				det.children = children;
			}
			m_details.append(det);
		}
	}

	// Check if filename requires details
	m_filename.setFormat(ui->lineFilenameDestination->text());
	m_needDetails = m_filename.needExactTags(m_sites.value(ui->comboSource->currentText()));

	const int response = QMessageBox::question(this, tr("Rename existing images"), tr("You are about to download information from %n image(s). Are you sure you want to continue?", "", m_details.size()), QMessageBox::Yes | QMessageBox::No);
	if (response == QMessageBox::Yes)
	{
		// Show progress bar
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
		const QSharedPointer<Image> img = p->images().at(0);

		if (m_needDetails)
		{
			m_getTags.append(img);
		}
		else
		{
			m_getAll[img->md5()].newPath = img->path(ui->lineFilenameDestination->text(), ui->lineFolder->text(), 0, true, false, true, true, true).first();
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
	auto *img = dynamic_cast<Image*>(sender());

	m_getAll[img->md5()].newPath = img->path(ui->lineFilenameDestination->text(), ui->lineFolder->text(), 0, true, false, true, true, true).first();
	ui->progressBar->setValue(ui->progressBar->value() + 1);

	loadNext();
}

void RenameExisting1::loadNext()
{
	if (!m_details.isEmpty())
	{
		const RenameExistingFile det = m_details.takeFirst();
		m_getAll.insert(det.md5, det);

		Page *page = new Page(m_profile, m_sites.value(ui->comboSource->currentText()), m_sites.values(), QStringList("md5:" + det.md5), 1, 1);
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

	RenameExisting2 *nextStep = new RenameExisting2(m_getAll.values(), QDir::toNativeSeparators(ui->lineFolder->text()), parentWidget());
	close();
	nextStep->show();
}

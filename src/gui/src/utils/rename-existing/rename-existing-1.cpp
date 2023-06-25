#include "utils/rename-existing/rename-existing-1.h"
#include <QCryptographicHash>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>
#include <ui_rename-existing-1.h>
#include "filename/filename.h"
#include "functions.h"
#include "helpers.h"
#include "loader/token.h"
#include "logger.h"
#include "models/api/api.h"
#include "models/image.h"
#include "models/page.h"
#include "models/profile.h"
#include "models/site.h"
#include "network/network-reply.h"
#include "utils/rename-existing/rename-existing-2.h"


RenameExisting1::RenameExisting1(Site *selected, Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::RenameExisting1), m_profile(profile), m_sites(profile->getSites()), m_needDetails(0), m_useIdKey(false)
{
	ui->setupUi(this);

	QStringList keys = m_sites.keys();
	ui->comboSource->addItems(keys);
	ui->comboSource->setCurrentIndex(keys.indexOf(selected->url()));

	QSettings *settings = profile->getSettings();
	ui->lineFolder->setText(settings->value("Save/path").toString());
	ui->lineFilenameOrigin->setText(settings->value("Save/filename").toString());
	ui->lineSuffixes->setText(getExternalLogFilesSuffixes(profile->getSettings()).join(", "));
	ui->lineFilenameDestination->setText(settings->value("Save/filename").toString());
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

	// Check that directory exists
	QDir dir(fixFilename("", ui->lineFolder->text()));
	if (!dir.exists()) {
		error(this, tr("This folder does not exist."));
		ui->buttonContinue->setEnabled(true);
		return;
	}

	// Make sure the input is valid
	if (ui->radioFilenameMd5->isChecked() && !ui->lineFilenameOrigin->text().contains(QStringLiteral("%md5%"))) {
		error(this, tr("If you want to get the MD5 from the filename, you have to include the %md5% token in it."));
		ui->buttonContinue->setEnabled(true);
		return;
	}
	if (ui->radioFilenameId->isChecked() && !ui->lineFilenameOrigin->text().contains(QStringLiteral("%id%"))) {
		error(this, tr("If you want to get the ID from the filename, you have to include the %id% token in it."));
		ui->buttonContinue->setEnabled(true);
		return;
	}
	m_useIdKey = ui->radioFilenameId->isChecked();

	// Suffixes
	QStringList suffixes = ui->lineSuffixes->text().split(',');
	for (QString &suffix : suffixes) {
		suffix = suffix.trimmed();
	}

	// Get all files from the destination directory
	auto files = listFilesFromDirectory(dir, suffixes);

	// Parse all files
	for (const auto &file : files) {
		const QString fileName = file.first;
		const QString path = dir.absoluteFilePath(fileName);

		QString key;
		if (m_useIdKey) {
			key = getFilenameId(fileName, ui->lineFilenameOrigin->text());
		} else {
			key = ui->radioForceMd5->isChecked()
				? getFileMd5(path)
				: getFilenameMd5(fileName, ui->lineFilenameOrigin->text());
		}

		if (!key.isEmpty()) {
			RenameExistingFile det;
			det.key = key;
			det.path = QDir::toNativeSeparators(path);
			if (!file.second.isEmpty()) {
				QStringList children;
				children.reserve(file.second.count());
				for (const QString &child : file.second) {
					children.append(QDir::toNativeSeparators(dir.absoluteFilePath(child)));
				}
				det.children = children;
			}
			m_details.append(det);
		}
	}

	// Check if filename requires details
	m_filename.setFormat(ui->lineFilenameDestination->text());
	m_needDetails = m_filename.needExactTags(m_sites.value(ui->comboSource->currentText()), m_profile->getSettings());

	const int response = QMessageBox::question(this, tr("Rename existing files"), tr("You are about to download information from %n image(s). Are you sure you want to continue?", "", m_details.size()), QMessageBox::Yes | QMessageBox::No);
	if (response == QMessageBox::Yes) {
		// Show progress bar
		ui->progressBar->setValue(0);
		ui->progressBar->setMaximum(m_details.size());
		ui->progressBar->show();

		loadNext();
	} else {
		ui->buttonContinue->setEnabled(true);
	}
}

void RenameExisting1::getAll(Page *p)
{
	if (p->images().isEmpty()) {
		log(tr("No image found when renaming image '%1'").arg(p->search().join(' ')), Logger::Warning);
		ui->progressBar->setValue(ui->progressBar->value() + 1);
		loadNext();
		return;
	}

	const QSharedPointer<Image> img = p->images().at(0);
	if (m_needDetails == 2 || (m_needDetails == 1 && img->hasUnknownTag())) {
		connect(img.data(), &Image::finishedLoadingTags, this, &RenameExisting1::getTags);
		img->loadDetails();
	} else {
		setImageResult(img.data());
	}
}

void RenameExisting1::getTags()
{
	auto *img = dynamic_cast<Image*>(sender());
	setImageResult(img);
}

void RenameExisting1::fullDetailsFinished()
{
	auto *reply = qobject_cast<NetworkReply*>(sender());

	// Network error
	if (reply->error()) {
		if (reply->error() != NetworkReply::NetworkError::OperationCanceledError) {
			log(QStringLiteral("Error loading full image details for '%1': %2").arg(reply->url().toString(), reply->errorString()), Logger::Error);
		}
		reply->deleteLater();
		loadNext();
		return;
	}

	// Extract data from reply and clean it up
	const QString source = QString::fromUtf8(reply->readAll());
	const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	reply->deleteLater();

	// Parse response
	Site *site = m_sites.value(ui->comboSource->currentText());
	Api *api = site->fullDetailsApi();
	ParsedDetails parsed = api->parseDetails(source, statusCode, site);

	// Parsing error
	if (!parsed.error.isEmpty()) {
		log(QStringLiteral("Error loading full image details: %1").arg(parsed.error), Logger::Error);
		loadNext();
		return;
	}

	setImageResult(parsed.image.data());
}

void RenameExisting1::setImageResult(Image *img)
{
	const QString &key = m_useIdKey ? QString::number(img->id()) : img->md5();
	const QString &dir = ui->lineFolder->text();

	if (!key.isEmpty()) {
		QFileInfo fi(m_getAll[key].path);
		auto tokens = img->tokens(m_profile);
		tokens.insert("old_directory", Token(fi.absolutePath().mid(dir.count() + (dir.endsWith("/") || dir.endsWith("\\") ? 0 : 1))));
		tokens.insert("old_filename", Token(fi.fileName()));

		Filename fn(ui->lineFilenameDestination->text());
		QStringList paths = fn.path(tokens, m_profile, dir, 0, Filename::Complex | Filename::Path);
		m_getAll[key].newPath = paths.first();
	} else {
		log(QStringLiteral("No key found for image, skipping"), Logger::Warning);
	}

	ui->progressBar->setValue(ui->progressBar->value() + 1);
	loadNext();
}

void RenameExisting1::loadNext()
{
	if (!m_details.isEmpty()) {
		const RenameExistingFile det = m_details.takeFirst();
		m_getAll.insert(det.key, det);

		Site *site = m_sites.value(ui->comboSource->currentText());

		// Try to load the image details from the full details API if available
		Api *api = site->fullDetailsApi();
		if (api != nullptr && m_useIdKey) {
			QString url = api->detailsUrl(m_useIdKey ? det.key.toLongLong() : 0, !m_useIdKey ? det.key : "", site).url;
			log(QStringLiteral("Loading full image details from `%1`").arg(url), Logger::Info);
			NetworkReply *reply = site->get(url, Site::QueryType::Details);
			connect(reply, &NetworkReply::finished, this, &RenameExisting1::fullDetailsFinished);
			return;
		}

		// Otherwise, try a "key:VAL" search using the listing API
		QStringList query(QString(m_useIdKey ? "id" : "md5") + ":" + det.key);
		Page *page = new Page(m_profile, site, m_sites.values(), query, 1, 1);
		connect(page, &Page::finishedLoading, this, &RenameExisting1::getAll);
		page->load();

		return;
	}

	auto *nextStep = new RenameExisting2(m_getAll.values(), QDir::toNativeSeparators(ui->lineFolder->text()), parentWidget());
	close();
	nextStep->show();
}

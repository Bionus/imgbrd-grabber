#include "tabs/image-preview.h"
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QMenu>
#include <QMovie>
#include <QRandomGenerator>
#include <QSettings>
#include <QtMath>
#include <QUrl>
#include <QVBoxLayout>
#include <QWidget>
#include "downloader/download-queue.h"
#include "downloader/image-downloader.h"
#include "functions.h"
#include "helpers.h"
#include "image-context-menu.h"
#include "logger.h"
#include "models/image.h"
#include "models/profile.h"
#include "models/site.h"
#include "network/network-reply.h"
#include "ui/QAffiche.h"
#include "ui/QBouton.h"


QMovie *ImagePreview::m_loadingMovie = nullptr;

ImagePreview::ImagePreview(QSharedPointer<Image> image, QWidget *container, Profile *profile, DownloadQueue *downloadQueue, MainWindow *mainWindow, QObject *parent)
	: QObject(parent), m_image(image), m_container(container), m_profile(profile), m_downloadQueue(downloadQueue), m_mainWindow(mainWindow)
{
	if (m_profile->getSettings()->value("thumbnailSmartSize", true).toBool()) {
		const qreal upscale = m_profile->getSettings()->value("thumbnailUpscale", 1.0).toDouble();
		const int imageSize = qFloor(150 * upscale);
		m_thumbnailUrl = image->mediaForSize(QSize(imageSize, imageSize)).url;
	} else {
		m_thumbnailUrl = image->url(Image::Size::Thumbnail);
	}

	m_name = image->name();
	m_counter = image->counter();

	auto *layout = new QVBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);
	container->setLayout(layout);

	container->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(container, &QWidget::customContextMenuRequested, this, &ImagePreview::customContextMenuRequested);
}

ImagePreview::~ImagePreview()
{
	m_reply->deleteLater();
	m_reply = nullptr;

	// We don't own the button, but it will likely be deleted soon as well
	m_bouton = nullptr;
}


void ImagePreview::showLoadingMessage()
{
	if (m_loadingMovie == nullptr) {
		auto *loadingMovie = new QMovie(":/images/loading.gif");
		if (m_loadingMovie == nullptr) {
			m_loadingMovie = loadingMovie;
			m_loadingMovie->start();
		} else {
			loadingMovie->deleteLater();
		}
	}

	auto *loadingLabel = new QLabel();
	loadingLabel->setMovie(m_loadingMovie);
	loadingLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	loadingLabel->setScaledContents(true);

	auto *layout = m_container->layout();
	layout->addWidget(loadingLabel);
}

void ImagePreview::load()
{
	if (m_thumbnailUrl.isValid()) {
		if (m_reply != nullptr) {
			m_reply->deleteLater();
		} else {
			showLoadingMessage();
		}

		Site *site = m_image->parentSite();
		m_reply = site->get(site->fixUrl(m_thumbnailUrl.toString()), Site::QueryType::Thumbnail, m_image->parentUrl(), "preview");
		connect(m_reply, &NetworkReply::finished, this, &ImagePreview::finishedLoadingPreview);
	} else {
		finishedLoading();
	}
}

void ImagePreview::abort()
{
	m_aborted = true;
	if (m_reply->isRunning()) {
		m_reply->abort();
	}
}

void ImagePreview::setChecked(bool checked)
{
	m_checked = checked;

	if (m_bouton != nullptr) {
		m_bouton->setChecked(checked);
	}
}

void ImagePreview::setDownloadProgress(qint64 v1, qint64 v2)
{
	if (m_bouton != nullptr) {
		m_bouton->setProgress(v1, v2);
	}
}


void ImagePreview::finishedLoadingPreview()
{
	if (m_aborted) {
		return;
	}

	// Aborted
	if (m_reply->error() == NetworkReply::NetworkError::OperationCanceledError) {
		return;
	}

	// Check redirection
	QUrl redirection = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redirection.isEmpty()) {
		m_thumbnailUrl = redirection;
		load();
		return;
	}

	// Loading error
	if (m_reply->error() != NetworkReply::NetworkError::NoError) {
		// Retry with JPG in case the original thumbnail had a weird extension
		const QString ext = getExtension(m_reply->url());
		if (!ext.isEmpty() && ext != "jpg") {
			log(QStringLiteral("Error loading thumbnail (%1), new try with extension JPG").arg(m_reply->errorString()), Logger::Warning);
			m_thumbnailUrl = setExtension(m_reply->url(), "jpg");
			load();
			return;
		}

		log(QStringLiteral("Error loading thumbnail (%1)").arg(m_reply->errorString()), Logger::Error);
		finishedLoading();
		return;
	}

	// Load preview from result
	QPixmap thumbnail;
	thumbnail.loadFromData(m_reply->readAll());
	if (thumbnail.isNull()) {
		log(QStringLiteral("One of the thumbnails is empty (`%1`).").arg(m_image->url(Image::Size::Thumbnail).toString()), Logger::Error);
		finishedLoading();
		return;
	}
	m_image->setPreviewImage(thumbnail);

	finishedLoading();
}

void ImagePreview::finishedLoading()
{
	auto *layout = m_container->layout();

	clearLayout(layout);

	if (m_reply != nullptr) {
		QSettings *settings = m_profile->getSettings();
		const bool resizeInsteadOfCropping = settings->value("resizeInsteadOfCropping", true).toBool();
		const bool resultsScrollArea = settings->value("resultsScrollArea", true).toBool();
		const int borderSize = settings->value("borders", 3).toInt();
		const qreal upscale = settings->value("thumbnailUpscale", 1.0).toDouble();
		const int imageSize = qFloor(150 * upscale);

		QBouton *l = new QBouton(0, resizeInsteadOfCropping, resultsScrollArea, borderSize, m_image->color(), m_container);
		l->setCheckable(true);
		l->setFlat(true);
		l->setChecked(m_checked);
		l->setInvertToggle(settings->value("invertToggle", false).toBool());
		l->setToolTip(m_image->tooltip());

		const QPixmap &thumbnail = m_image->previewImage();
		if (thumbnail.isNull()) {
			if (m_image->hasTag(QStringLiteral("flash"))) {
				l->scale(QPixmap(":/images/flash.png"), QSize(imageSize, imageSize));
			} else {
				l->scale(QPixmap(":/images/noimage.png"), QSize(imageSize, imageSize));
			}
		} else {
			l->scale(thumbnail, QSize(imageSize, imageSize));
		}
		if (!m_counter.isEmpty()) {
			l->setCounter(m_counter);
		}

		connect(l, SIGNAL(appui(int)), this, SIGNAL(clicked()));
		connect(l, SIGNAL(toggled(int,bool,bool)), this, SLOT(toggledWithId(int,bool,bool)));

		layout->addWidget(l);
		m_bouton = l;
	}

	if (!m_name.isEmpty()) {
		auto *label = new QAffiche(0);
		label->setText(m_name);
		connect(label, SIGNAL(clicked(int)), this, SIGNAL(clicked()));
		layout->addWidget(label);
	}

	emit finished();
}

void ImagePreview::toggledWithId(int id, bool toggle, bool range)
{
	Q_UNUSED(id);

	emit toggled(toggle, range);
}


QStringList getImageAlreadyExists(const QSharedPointer<Image> &img, Profile *profile)
{
	QSettings *settings = profile->getSettings();
	const QString path = settings->value("Save/path").toString().replace("\\", "/");
	const QString fn = settings->value("Save/filename").toString();

	if (Filename(fn).needExactTags(img->parentSite(), settings) == 0) {
		QStringList ret;
		QStringList files = img->paths(fn, path, 0);
		for (const QString &file : files) {
			if (QFile(file).exists()) {
				ret.append(file);
			}
		}
		if (!ret.isEmpty()) {
			return ret;
		}
	}

	return profile->md5Exists(img->md5());
}

void ImagePreview::customContextMenuRequested()
{
	QMenu *menu = new ImageContextMenu(m_profile->getSettings(), m_image, m_mainWindow, m_container);
	QAction *first = menu->actions().first();

	// Save image
	QAction *actionSave;
	if (!getImageAlreadyExists(m_image, m_profile).isEmpty()) {
		actionSave = new QAction(QIcon(":/images/status/error.png"), tr("Delete"), menu);
	} else {
		actionSave = new QAction(QIcon(":/images/icons/save.png"), tr("Save"), menu);
	}
	connect(actionSave, &QAction::triggered, this, &ImagePreview::contextSaveImage);
	menu->insertAction(first, actionSave);

	// Save image as...
	QAction *actionSaveAs = new QAction(QIcon(":/images/icons/save-as.png"), tr("Save as..."), menu);
	connect(actionSaveAs, &QAction::triggered, this, &ImagePreview::contextSaveImageAs);
	menu->insertAction(first, actionSaveAs);

	// Custom elements
	if (m_customContextMenu != nullptr) {
		m_customContextMenu(menu, m_image);
	}

	menu->insertSeparator(first);

	menu->exec(QCursor::pos());
}

void ImagePreview::contextSaveImage()
{
	QStringList already = getImageAlreadyExists(m_image, m_profile);
	if (!already.isEmpty()) {
		for (const QString &path : already) {
			QFile(path).remove();
			m_profile->removeMd5(m_image->md5(), path);
		}
	} else {
		QSettings *settings = m_profile->getSettings();
		const QString fn = settings->value("Save/filename").toString();
		const QString path = settings->value("Save/path").toString();

		auto downloader = new ImageDownloader(m_profile, m_image, fn, path, 1, true, true, m_downloadQueue);
		connect(downloader, &ImageDownloader::downloadProgress, this, &ImagePreview::contextSaveImageProgress);
		m_downloadQueue->add(DownloadQueue::Manual, downloader);
	}
}

void ImagePreview::contextSaveImageAs()
{
	QSettings *settings = m_profile->getSettings();

	Filename format(settings->value("Save/filename").toString());
	QString tmpPath;

	// If we need detailed tags for the filename, we first load them
	const int needTags = format.needExactTags(m_image->parentSite(), settings);
	if (needTags == 2 || (needTags == 1 && m_image->hasUnknownTag())) {
		QEventLoop loop;
		m_image->loadDetails();
		connect(m_image.data(), &Image::finishedLoadingTags, &loop, &QEventLoop::quit);
		loop.exec();
	}

	// If the MD5 is required for the filename, we first download the image
	if (format.needTemporaryFile(m_image->tokens(m_profile))) {
		tmpPath = QDir::temp().absoluteFilePath("grabber-saveAs-" + QString::number(QRandomGenerator::global()->generate(), 16));

		QEventLoop loop;
		ImageDownloader downloader(m_profile, m_image, { tmpPath }, 1, true, true, this);
		connect(&downloader, &ImageDownloader::saved, &loop, &QEventLoop::quit);
		downloader.save();
		loop.exec();
	}

	const QStringList filenames = format.path(*m_image, m_profile);
	const QString filename = filenames.first().section(QDir::separator(), -1);
	const QString lastDir = settings->value("Viewer/lastDir").toString();

	QString path = QFileDialog::getSaveFileName(m_container, tr("Save image"), QDir::toNativeSeparators(lastDir + "/" + filename), "Images (*.png *.gif *.jpg *.jpeg)");
	if (!path.isEmpty()) {
		path = QDir::toNativeSeparators(path);
		settings->setValue("Viewer/lastDir", path.section(QDir::separator(), 0, -2));

		if (!tmpPath.isEmpty()) {
			QFile::rename(tmpPath, path);
		} else {
			auto downloader = new ImageDownloader(m_profile, m_image, { path }, 1, true, true, this, true, false, Image::Size::Unknown, true, true);
			connect(downloader, &ImageDownloader::downloadProgress, this, &ImagePreview::contextSaveImageProgress);
			m_downloadQueue->add(DownloadQueue::Manual, downloader);
		}
	} else if (!tmpPath.isEmpty()) {
		QFile::remove(tmpPath);
	}
}

void ImagePreview::contextSaveImageProgress(const QSharedPointer<Image> &img, qint64 v1, qint64 v2)
{
	Q_UNUSED(img);
	setDownloadProgress(v1, v2);
}

void ImagePreview::setCustomContextMenu(std::function<void (QMenu *, const QSharedPointer<Image> &)> customContextMenu)
{
	m_customContextMenu = customContextMenu;
}

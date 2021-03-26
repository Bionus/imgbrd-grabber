#include "viewer/zoom-window.h"
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QMovie>
#include <QPainter>
#include <QScreen>
#include <QScrollBar>
#include <QShortcut>
#include <QUrl>
#include <QVideoWidget>
#include <QWheelEvent>
#include <ui_zoom-window.h>
#include "downloader/image-downloader.h"
#include "functions.h"
#include "helpers.h"
#include "image-context-menu.h"
#include "logger.h"
#include "main-window.h"
#include "models/filename.h"
#include "models/filtering/post-filter.h"
#include "models/page.h"
#include "models/pool.h"
#include "models/profile.h"
#include "models/site.h"
#include "settings/options-window.h"
#include "tabs/search-tab.h"
#include "tag-context-menu.h"
#include "tags/tag.h"
#include "tags/tag-stylist.h"
#include "threads/image-loader.h"
#include "threads/image-loader-queue.h"
#include "ui/QAffiche.h"
#include "viewer/details-window.h"
#include "viewer/players/gif-player.h"
#include "viewer/players/video-player.h"


ZoomWindow::ZoomWindow(QList<QSharedPointer<Image>> images, const QSharedPointer<Image> &image, Site *site, Profile *profile, MainWindow *parent, SearchTab *tab)
	: QWidget(nullptr, Qt::Window), m_parent(parent), m_tab(tab), m_profile(profile), m_favorites(profile->getFavorites()), m_viewItLater(profile->getKeptForLater()), m_ignore(profile->getIgnored()), m_settings(profile->getSettings()), ui(new Ui::ZoomWindow), m_site(site), m_timeout(300), m_tooBig(false), m_loadedImage(false), m_loadedDetails(false), m_finished(false), m_size(0), m_isFullscreen(false), m_isSlideshowRunning(false), m_images(std::move(images)), m_displayImage(QPixmap()), m_labelImageScaled(false)
{
	setAttribute(Qt::WA_DeleteOnClose);
	connect(parent, &MainWindow::destroyed, this, &QWidget::deleteLater);
	ui->setupUi(this);

	m_pendingAction = PendingNothing;
	m_pendingClose = false;

	restoreGeometry(m_settings->value("Zoom/geometry").toByteArray());
	ui->buttonPlus->setChecked(m_settings->value("Zoom/plus", false).toBool());
	ui->progressBarDownload->hide();

	m_settings->beginGroup("Zoom/Shortcuts");
		QShortcut *quit = new QShortcut(getKeySequence(m_settings, "keyQuit", Qt::Key_Escape), this);
			connect(quit, &QShortcut::activated, this, &ZoomWindow::close);
		QShortcut *prev = new QShortcut(getKeySequence(m_settings, "keyPrev", Qt::Key_Left), this);
			connect(prev, &QShortcut::activated, this, &ZoomWindow::previous);
		QShortcut *next = new QShortcut(getKeySequence(m_settings, "keyNext", Qt::Key_Right), this);
			connect(next, &QShortcut::activated, this, &ZoomWindow::next);

		QShortcut *details = new QShortcut(getKeySequence(m_settings, "keyDetails", Qt::Key_D), this);
			connect(details, &QShortcut::activated, this, &ZoomWindow::showDetails);
		QShortcut *saveAs = new QShortcut(getKeySequence(m_settings, "keySaveAs", QKeySequence::SaveAs, Qt::CTRL + Qt::SHIFT + Qt::Key_S), this);
			connect(saveAs, &QShortcut::activated, this, &ZoomWindow::saveImageAs);

		QShortcut *save = new QShortcut(getKeySequence(m_settings, "keySave", QKeySequence::Save, Qt::CTRL + Qt::Key_S), this);
			connect(save, SIGNAL(activated()), this, SLOT(saveImage()));
		QShortcut *saveNQuit = new QShortcut(getKeySequence(m_settings, "keySaveNQuit", Qt::CTRL + Qt::Key_W), this);
			connect(saveNQuit, SIGNAL(activated()), this, SLOT(saveNQuit()));
		QShortcut *open = new QShortcut(getKeySequence(m_settings, "keyOpen", Qt::CTRL + Qt::Key_O), this);
			connect(open, SIGNAL(activated()), this, SLOT(openSaveDirFav()));

		QShortcut *saveFav = new QShortcut(getKeySequence(m_settings, "keySaveFav", Qt::CTRL + Qt::ALT + Qt::Key_S), this);
			connect(saveFav, SIGNAL(activated()), this, SLOT(saveImageFav()));
		QShortcut *saveNQuitFav = new QShortcut(getKeySequence(m_settings, "keySaveNQuitFav", Qt::CTRL + Qt::ALT + Qt::Key_W), this);
			connect(saveNQuitFav, SIGNAL(activated()), this, SLOT(saveNQuitFav()));
		QShortcut *openFav = new QShortcut(getKeySequence(m_settings, "keyOpenFav", Qt::CTRL + Qt::ALT + Qt::Key_O), this);
			connect(openFav, SIGNAL(activated()), this, SLOT(openSaveDirFav()));

		QShortcut *toggleFullscreen = new QShortcut(getKeySequence(m_settings, "keyToggleFullscreen", QKeySequence::FullScreen, Qt::Key_F11), this);
			connect(toggleFullscreen, &QShortcut::activated, this, &ZoomWindow::toggleFullScreen);
		QShortcut *copyDataToClipboard = new QShortcut(getKeySequence(m_settings, "keyDataToClipboard", QKeySequence::Copy, Qt::CTRL + Qt::Key_C), this);
			connect(copyDataToClipboard, &QShortcut::activated, this, &ZoomWindow::copyImageDataToClipboard);
	m_settings->endGroup();

	m_labelTagsLeft = new QAffiche(QVariant(), 0, QColor(), this);
		m_labelTagsLeft->setContextMenuPolicy(Qt::CustomContextMenu);
		m_labelTagsLeft->setTextInteractionFlags(Qt::TextBrowserInteraction);
		connect(m_labelTagsLeft, &QAffiche::customContextMenuRequested, this, &ZoomWindow::contextMenu);
		connect(m_labelTagsLeft, &QAffiche::linkActivated, this, &ZoomWindow::openUrl);
		connect(m_labelTagsLeft, &QAffiche::linkHovered, this, &ZoomWindow::linkHovered);
		connect(m_labelTagsLeft, SIGNAL(middleClicked()), this, SLOT(openInNewTab()));
		ui->scrollAreaWidgetContents->layout()->addWidget(m_labelTagsLeft);

	m_labelTagsTop = new QAffiche(QVariant(), 0, QColor(), this);
		m_labelTagsTop->setWordWrap(true);
		m_labelTagsTop->setContextMenuPolicy(Qt::CustomContextMenu);
		m_labelTagsTop->setTextInteractionFlags(Qt::TextBrowserInteraction);
		connect(m_labelTagsTop, &QAffiche::customContextMenuRequested, this, &ZoomWindow::contextMenu);
		connect(m_labelTagsTop, &QAffiche::linkActivated, this, &ZoomWindow::openUrl);
		connect(m_labelTagsTop, &QAffiche::linkHovered, this, &ZoomWindow::linkHovered);
		connect(m_labelTagsTop, SIGNAL(middleClicked()), this, SLOT(openInNewTab()));
		ui->verticalLayout->insertWidget(0, m_labelTagsTop, 0);

	// Automatically re-color if some settings change
	connect(m_profile, &Profile::favoritesChanged, this, &ZoomWindow::colore);
	connect(m_profile, &Profile::keptForLaterChanged, this, &ZoomWindow::colore);
	connect(m_profile, &Profile::ignoredChanged, this, &ZoomWindow::colore);
	connect(m_profile, &Profile::blacklistChanged, this, &ZoomWindow::colore);

	m_stackedWidget = new QStackedWidget(this);
		ui->verticalLayout->insertWidget(1, m_stackedWidget, 1);
	m_labelImage = new QAffiche(QVariant(), 0, QColor(), this);
		m_labelImage->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
		m_labelImage->setAlignment(getAlignments("imagePositionImage"));
		connect(m_labelImage, SIGNAL(doubleClicked()), this, SLOT(openFile()));
		m_stackedWidget->addWidget(m_labelImage);

	m_gifPlayer = new GifPlayer(m_settings->value("Zoom/showGifPlayerControls", true).toBool(), getAlignments("imagePositionAnimation"), this);
	m_stackedWidget->addWidget(m_gifPlayer);

	if (m_settings->value("Zoom/useVideoPlayer", true).toBool()) {
		// getAlignments("imagePositionVideo")
		m_videoPlayer = new VideoPlayer(m_settings->value("Zoom/showVideoPlayerControls", true).toBool(), this);
		m_stackedWidget->addWidget(m_videoPlayer);
	}

	connect(ui->buttonDetails, &QPushButton::clicked, this, &ZoomWindow::showDetails);
	connect(ui->buttonPlus, &QPushButton::toggled, this, &ZoomWindow::updateButtonPlus);

	m_labelImage->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_labelImage, &QAffiche::customContextMenuRequested, this, &ZoomWindow::imageContextMenu);

	m_slideshow.setSingleShot(true);
	connect(&m_slideshow, &QTimer::timeout, this, &ZoomWindow::next);

	// Overlay progressbar and image
	ui->overlayLayout->removeWidget(ui->progressBarDownload);
	ui->overlayLayout->addWidget(ui->progressBarDownload, 0, 0, Qt::AlignBottom);
	ui->progressBarDownload->raise();

	// Threads
	m_imageLoaderThread.setObjectName("Image loader thread");
	m_imageLoader = new ImageLoader();
	m_imageLoader->moveToThread(&m_imageLoaderThread);
	m_imageLoaderQueueThread.setObjectName("Image loader queue thread");
	m_imageLoaderQueue = new ImageLoaderQueue(m_imageLoader);
	m_imageLoaderQueue->moveToThread(&m_imageLoaderQueueThread);
	connect(&m_imageLoaderThread, &QThread::finished, m_imageLoader, &QObject::deleteLater);
	connect(&m_imageLoaderQueueThread, &QThread::finished, m_imageLoaderQueue, &QObject::deleteLater);
	connect(this, &ZoomWindow::loadImage, m_imageLoaderQueue, &ImageLoaderQueue::load);
	connect(this, &ZoomWindow::clearLoadQueue, m_imageLoaderQueue, &ImageLoaderQueue::clear);
	connect(m_imageLoaderQueue, &ImageLoaderQueue::finished, this, &ZoomWindow::display);
	m_imageLoaderQueueThread.start();
	m_imageLoaderThread.start();

	// Background color
	QString bg = m_settings->value("imageBackgroundColor", "").toString();
	if (!bg.isEmpty()) {
		setStyleSheet("#ZoomWindow { background-color:" + bg + "; }");
	}

	load(image);
}
void ZoomWindow::go()
{
	ui->labelPools->hide();
	bool whitelisted = false;
	if (!m_settings->value("whitelistedtags").toString().isEmpty()) {
		QStringList whitelist = m_settings->value("whitelistedtags").toString().split(" ", Qt::SkipEmptyParts);
		for (const Tag &t : m_image->tags()) {
			if (whitelist.contains(t.text())) {
				whitelisted = true;
				break;
			}
		}
	}
	if (m_settings->value("autodownload", false).toBool() || (whitelisted && m_settings->value("whitelist_download", "image").toString() == "image")) {
		saveImage();
	}

	auto *timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(update()));
		timer->setSingleShot(true);
		m_resizeTimer = timer;

	QString pos = m_settings->value("tagsposition", "top").toString();
	if (pos == QLatin1String("auto")) {
		if (!m_image->size().isEmpty()) {
			if (static_cast<double>(m_image->width()) / static_cast<double>(m_image->height()) >= 4.0 / 3.0) {
				pos = QStringLiteral("top");
			} else {
				pos = QStringLiteral("left");
			}
		} else {
			pos = QStringLiteral("top");
		}
	}

	if (pos == QLatin1String("top")) {
		ui->widgetLeft->hide();
		m_labelTagsTop->show();
	} else {
		m_labelTagsTop->hide();
		m_labelTagsLeft->show();
		ui->widgetLeft->show();
	}

	m_detailsWindow = new DetailsWindow(m_profile, this);
	colore();

	// Load image details (exact tags & co)
	connect(m_image.data(), &Image::finishedLoadingTags, this, &ZoomWindow::replyFinishedDetails, Qt::UniqueConnection);
	m_image->loadDetails();

	if (!m_isFullscreen) {
		activateWindow();
	}
}

/**
 * Destructor of the zoomWindow class
 */
ZoomWindow::~ZoomWindow()
{
	m_labelTagsTop->deleteLater();
	m_labelTagsLeft->deleteLater();
	m_detailsWindow->deleteLater();

	m_gifPlayer->deleteLater();
	m_videoPlayer->deleteLater();

	// Quit threads
	m_imageLoaderQueueThread.quit();
	m_imageLoaderQueueThread.wait(1000);
	m_imageLoaderThread.quit();
	m_imageLoaderThread.wait(1000);

	delete ui;
}

void ZoomWindow::imageContextMenu()
{
	QMenu *menu = new ImageContextMenu(m_settings, m_image, m_parent, this);

	// Reload action
	QAction *reloadImageAction = new QAction(QIcon(":/images/icons/update.png"), tr("Reload"), menu);
	connect(reloadImageAction, &QAction::triggered, this, &ZoomWindow::reloadImage);

	// Copy actions
	QAction *copyImageAction = new QAction(QIcon(":/images/icons/copy.png"), tr("Copy file"), menu);
	connect(copyImageAction, &QAction::triggered, this, &ZoomWindow::copyImageFileToClipboard);
	QAction *copyDataAction = new QAction(QIcon(":/images/icons/document-binary.png"), tr("Copy data"), menu);
	connect(copyDataAction, &QAction::triggered, this, &ZoomWindow::copyImageDataToClipboard);

	// Insert actions at the beginning
	QAction *first = menu->actions().first();
	menu->insertAction(first, reloadImageAction);
	menu->insertSeparator(first);
	menu->insertAction(first, copyImageAction);
	menu->insertAction(first, copyDataAction);
	menu->insertSeparator(first);

	menu->exec(QCursor::pos());
}
void ZoomWindow::reloadImage()
{
	load(true);
}
void ZoomWindow::copyImageFileToClipboard()
{
	auto *mimeData = new QMimeData();
	mimeData->setUrls({ QUrl::fromLocalFile(m_imagePath) });
	QApplication::clipboard()->setMimeData(mimeData);
}
void ZoomWindow::copyImageDataToClipboard()
{
	QApplication::clipboard()->setPixmap(m_displayImage);
}

void ZoomWindow::showDetails()
{
	m_detailsWindow->setImage(m_image);
	m_detailsWindow->show();
}

void ZoomWindow::openUrl(const QString &url)
{ emit linkClicked(QUrl::fromPercentEncoding(url.toUtf8())); }
void ZoomWindow::openPool(const QString &url)
{
	if (url.startsWith(QLatin1String("pool:"))) {
		emit poolClicked(url.rightRef(url.length() - 5).toInt(), m_image->parentSite()->url());
	} else {
		Page *p = new Page(m_profile, m_image->parentSite(), m_profile->getSites().values(), QStringList { "id:" + url }, 1, 1, QStringList(), false, this);
		connect(p, &Page::finishedLoading, this, &ZoomWindow::openPoolId);
		p->load();
	}
}
void ZoomWindow::openPoolId(Page *p)
{
	if (p->images().empty()) {
		p->deleteLater();
		return;
	}

	m_image = p->images().at(0);
	m_timeout = 300;
	m_loadedDetails = false;
	m_loadedImage = false;
	m_finished = false;
	m_size = 0;
	m_labelImage->hide();
	ui->verticalLayout->removeWidget(m_labelImage);

	go();
}

void ZoomWindow::openSaveDir(bool fav)
{
	// If the file was already saved, we focus on it
	if (!m_source.isEmpty()) {
		showInGraphicalShell(m_source);
	} else {
		const QString path = m_settings->value("Save/path" + QString(fav ? "_favorites" : "")).toString();
		const QString fn = m_settings->value("Save/filename" + QString(fav ? "_favorites" : "")).toString();

		const QStringList files = m_image->paths(fn, path, 0);
		const QString url = !files.empty() ? files.first() : path;

		QDir dir = QFileInfo(url).dir();
		if (dir.exists()) {
			showInGraphicalShell(url);
		} else {
			const int reply = QMessageBox::question(this, tr("Folder does not exist"), tr("The save folder does not exist yet. Create it?"), QMessageBox::Yes | QMessageBox::No);
			if (reply == QMessageBox::Yes) {
				QDir rootDir(path);
				if (!rootDir.mkpath(dir.path())) {
					error(this, tr("Error creating folder.\n%1").arg(url));
				}
				showInGraphicalShell(url);
			}
		}
	}
}
void ZoomWindow::openSaveDirFav()
{ openSaveDir(true); }

void ZoomWindow::linkHovered(const QString &url)
{ m_link = QUrl::fromPercentEncoding(url.toUtf8()); }
void ZoomWindow::contextMenu(const QPoint &pos)
{
	Q_UNUSED(pos)

	if (m_link.isEmpty()) {
		return;
	}

	Page page(m_profile, m_site, { m_site }, QStringList { m_link });
	auto *menu = new TagContextMenu(m_link, m_image->tags(), page.friendlyUrl(), m_profile, { m_site }, true, this);
	connect(menu, &TagContextMenu::openNewTab, this, &ZoomWindow::openInNewTab);
	connect(menu, &TagContextMenu::setFavoriteImage, this, &ZoomWindow::setfavorite);
	menu->exec(QCursor::pos());
}

void ZoomWindow::openInNewTab()
{
	if (!m_link.isEmpty()) {
		m_parent->addTab(m_link, false, true, m_tab);
	}
}
void ZoomWindow::setfavorite()
{
	Favorite fav(m_link);
	const int pos = m_favorites.indexOf(fav);
	if (pos >= 0) {
		m_favorites[pos].setImage(m_loadedImage ? m_displayImage : m_image->previewImage());
	} else {
		fav.setImage(m_loadedImage ? m_displayImage : m_image->previewImage());
		m_favorites.append(fav);
	}

	m_profile->emitFavorite();
}

void ZoomWindow::load(bool force)
{
	const Image::Size size = m_image->preferredDisplaySize();
	log(QStringLiteral("Loading image from `%1`").arg(m_image->url(size).toString()));

	m_source.clear();

	ui->progressBarDownload->setMaximum(100);
	ui->progressBarDownload->setValue(0);
	ui->progressBarDownload->show();

	ImageDownloader *dwl = m_imageDownloaders.value(m_image, nullptr);
	if (dwl == nullptr) {
		const Filename fn = Filename(QUuid::createUuid().toString().mid(1, 36) + ".%ext%");
		const QStringList paths = fn.path(*m_image.data(), m_profile, m_profile->tempPath(), 1, Filename::ExpandConditionals | Filename::Path);
		dwl = new ImageDownloader(m_profile, m_image, paths, 1, false, false, this, true, force, size, false, true);
		m_imageDownloaders.insert(m_image, dwl);
	}
	connect(dwl, &ImageDownloader::downloadProgress, this, &ZoomWindow::downloadProgress, Qt::UniqueConnection);
	connect(dwl, &ImageDownloader::saved, this, &ZoomWindow::replyFinishedZoom, Qt::UniqueConnection);

	m_imageTime.start();

	if (!dwl->isRunning()) {
		dwl->save();
	}
}

#define PERCENT 0.05
#define TIME 500
void ZoomWindow::downloadProgress(QSharedPointer<Image> img, qint64 bytesReceived, qint64 bytesTotal)
{
	Q_UNUSED(img)

	ui->progressBarDownload->setMaximum(bytesTotal);
	ui->progressBarDownload->setValue(bytesReceived);

	const bool isAnimated = m_image->isVideo() || !m_isAnimated.isEmpty();
	if (!isAnimated && (m_imageTime.elapsed() > TIME || (bytesTotal > 0 && static_cast<double>(bytesReceived) / bytesTotal > PERCENT))) {
		m_imageTime.restart();
		// FIXME: should read the file contents now that the image is not loaded in RAM anymore
		// emit loadImage(m_image->data());
	}
}
void ZoomWindow::display(const QPixmap &pix, int size)
{
	if (!pix.size().isEmpty() && size >= m_size) {
		m_size = size;
		m_displayImage = pix;
		update(!m_finished);

		updateWindowTitle();
	}
}

void ZoomWindow::replyFinishedDetails()
{
	disconnect(m_image.data(), &Image::finishedLoadingTags, this, &ZoomWindow::replyFinishedDetails);

	m_loadedDetails = true;
	colore();

	// Show pool information
	if (!m_image->pools().isEmpty()) {
		auto imgPools = m_image->pools();
		QStringList pools;
		pools.reserve(imgPools.count());
		for (const Pool &p : imgPools) {
			pools.append((p.previous() != 0 ? "<a href=\"" + QString::number(p.previous()) + "\">&lt;</a> " : "") + "<a href=\"pool:" + QString::number(p.id()) + "\">" + p.name() + "</a>" + (p.next() != 0 ? " <a href=\"" + QString::number(p.next()) + "\">&gt;</a>" : ""));
		}
		ui->labelPools->setText(pools.join(QStringLiteral("<br />")));
		ui->labelPools->show();
	}

	m_isAnimated = m_image->isAnimated();

	const QString path1 = m_settings->value("Save/path").toString().replace("\\", "/");
	const QStringList pth1s = m_image->paths(m_settings->value("Save/filename").toString(), path1, 0);
	QString source1;
	for (const QString &pth1 : pth1s) {
		QFile file(pth1);
		if (file.exists()) {
			source1 = file.fileName();
		}
	}

	const QString path2 = m_settings->value("Save/path_favorites").toString().replace("\\", "/");
	const QStringList pth2s = m_image->paths(m_settings->value("Save/filename_favorites").toString(), path2, 0);
	QString source2;
	for (const QString &pth2 : pth2s) {
		QFile file(pth2);
		if (file.exists()) {
			source2 = file.fileName();
		}
	}

	QPair<QString, QString> md5ActionPair = m_profile->md5Action(m_image->md5(), !source1.isEmpty() ? source1 : source2);
	QString md5Action = md5ActionPair.first;
	QString md5Exists = md5ActionPair.second;

	QString imagePath = m_image->savePath();

	// If the file already exists, we directly display it
	if (!md5Exists.isEmpty() || !source1.isEmpty() || !source2.isEmpty()) {
		imagePath = !md5Exists.isEmpty() ? md5Exists : (!source1.isEmpty() ? source1 : source2);

		// Update save button state
		const SaveButtonState md5State = !md5Exists.isEmpty() && md5Action != "save" ? SaveButtonState::ExistsMd5 : SaveButtonState::Save;
		setButtonState(false, !source1.isEmpty() ? SaveButtonState::ExistsDisk : md5State);
		setButtonState(true, !source2.isEmpty() ? SaveButtonState::ExistsDisk : md5State);

		// Fix extension when it should be guessed
		const QString fext = imagePath.section('.', -1);
		m_image->setFileExtension(fext);

		// Ensure the file actually exists before trying to set it as the source
		if (QFile::exists(imagePath)) {
			m_source = imagePath;
			m_image->setSavePath(m_source);
		}
	}

	// If a file to load the image from was found
	if (!imagePath.isEmpty() && QFile::exists(imagePath)) {
		m_imagePath = imagePath;
		log(QStringLiteral("Image loaded from the file `%1`").arg(imagePath));

		m_finished = true;
		m_loadedImage = true;
		pendingUpdate();

		draw();
	}
	// If the file does not exist, we have to load it
	else {
		load();
	}

	updateWindowTitle();
}
void ZoomWindow::colore()
{
	QStringList t = TagStylist(m_profile).stylished(m_image->tags(), m_settings->value("Zoom/showTagCount", false).toBool(), false, m_settings->value("Zoom/tagOrder", "type").toString());
	const QString tags = t.join(' ');

	if (ui->widgetLeft->isHidden()) {
		m_labelTagsTop->setText(tags);
	} else {
		m_labelTagsLeft->setText(t.join(QStringLiteral("<br/>")));
		ui->scrollArea->setMinimumWidth(m_labelTagsLeft->sizeHint().width() + ui->scrollArea->verticalScrollBar()->sizeHint().width());
	}

	m_detailsWindow->setImage(m_image);
}
void ZoomWindow::setButtonState(bool fav, SaveButtonState state)
{
	// Update state
	if (fav) {
		m_saveButonStateFav = state;
	} else {
		m_saveButonState = state;
	}

	// Update actual button label
	QPushButton *button = fav ? ui->buttonSaveFav : ui->buttonSave;
	button->setToolTip(QString());
	switch (state)
	{
		case SaveButtonState::Save:
			button->setText(fav ? tr("Save (fav)") : tr("Save"));
			break;

		case SaveButtonState::Saving:
			button->setText(fav ? tr("Saving... (fav)") : tr("Saving..."));
			break;

		case SaveButtonState::Saved:
			button->setText(fav ? tr("Saved! (fav)") : tr("Saved!"));
			break;

		case SaveButtonState::Copied:
			button->setText(fav ? tr("Copied! (fav)") : tr("Copied!"));
			break;

		case SaveButtonState::Moved:
			button->setText(fav ? tr("Moved! (fav)") : tr("Moved!"));
			break;

		case SaveButtonState::Linked:
			button->setText(fav ? tr("Link created! (fav)") : tr("Link created!"));
			break;

		case SaveButtonState::ExistsMd5:
			button->setToolTip(m_imagePath);
			button->setText(fav ? tr("MD5 already exists (fav)") : tr("MD5 already exists"));
			break;

		case SaveButtonState::ExistsDisk:
			button->setToolTip(m_imagePath);
			button->setText(fav ? tr("Already exists (fav)") : tr("Already exists"));
			break;

		case SaveButtonState::Delete:
			button->setText(fav ? tr("Delete (fav)") : tr("Delete"));
			break;
	}

	// Update "Save a close" button label
	QPushButton *saveQuit = fav ? ui->buttonSaveNQuitFav : ui->buttonSaveNQuit;
	switch (state)
	{
		case SaveButtonState::Save:
			saveQuit->setText(fav ? tr("Save and close (fav)") : tr("Save and close"));
			break;

		case SaveButtonState::Saving:
			saveQuit->setText(fav ? tr("Saving... (fav)") : tr("Saving..."));
			break;

		default:
			saveQuit->setText(fav ? tr("Close (fav)") : tr("Close"));
			break;
	}
}

void ZoomWindow::replyFinishedZoom(const QSharedPointer<Image> &img, const QList<ImageSaveResult> &result)
{
	ImageSaveResult res = result.first();
	log(QStringLiteral("Image received from `%1`").arg(img->url(res.size).toString()));

	ui->progressBarDownload->hide();
	m_finished = true;

	if (res.result == 500) {
		m_tooBig = true;
		if (!m_image->isVideo()) {
			error(this, tr("File is too big to be displayed.\n%1").arg(m_image->url().toString()));
		}
	} else if (res.result == Image::SaveResult::NotFound) {
		showLoadingError("Image not found.");
	} else if (res.result == Image::SaveResult::NetworkError) {
		showLoadingError("Error loading the image.");
	} else if (res.result == Image::SaveResult::Error) {
		showLoadingError("Error saving the image.");
	} else {
		m_imagePath = res.path;
		m_loadedImage = true;

		img->setTemporaryPath(m_imagePath, res.size);

		updateWindowTitle();
		pendingUpdate();
		draw();
	}
}

void ZoomWindow::showLoadingError(const QString &message)
{
	log(message);
	ui->labelLoadingError->setText(message);
	ui->labelLoadingError->show();
}

void ZoomWindow::pendingUpdate()
{
	// If we don't want to save, nothing to do
	if (m_pendingAction == PendingNothing) {
		return;
	}

	// If the image is not even loaded, we cannot save it (unless it's a big file)
	if (!m_loadedImage && !m_tooBig) {
		return;
	}

	// If the image is loaded but we need their tags and we don't have them, we wait
	if (m_pendingAction != PendingSaveAs) {
		const bool fav = m_pendingAction == PendingSaveFav;
		Filename fn(m_settings->value("Save/path" + QString(fav ? "_favorites" : "")).toString());

		if (!m_loadedDetails && fn.needExactTags(m_site, m_settings) != 0) {
			return;
		}
	}

	switch (m_pendingAction)
	{
		case PendingSave:
		case PendingSaveFav:
		case PendingSaveAs:
			saveImageNow();
			break;

		case PendingOpen:
			openFile(true);
			break;

		default:
			return;
	}
}

void ZoomWindow::draw()
{
	// Videos don't get drawn
	if (m_image->isVideo() && !m_settings->value("Zoom/useVideoPlayer", true).toBool()) {
		return;
	}

	// Clear active players
	if (m_gifPlayer != nullptr) {
		m_gifPlayer->unload();
	}
	if (m_videoPlayer != nullptr) {
		m_videoPlayer->unload();
	}
	m_displayImage = QPixmap();

	// GIF (using QLabel support for QMovie)
	if (!m_isAnimated.isEmpty()) {
		m_stackedWidget->setCurrentWidget(m_gifPlayer);
		m_gifPlayer->load(m_imagePath);
	}
	// Videos (using a media player)
	else if (m_image->isVideo()) {
		m_stackedWidget->setCurrentWidget(m_videoPlayer);
		m_videoPlayer->load(m_imagePath);
	}
	// Images
	else {
		m_displayImage.load(m_imagePath);

		updateWindowTitle();
		update();
	}
}



/**
 * Updates the image label to use the current image.
 * @param onlySize true to update the image quickly
 */
void ZoomWindow::update(bool onlySize, bool force)
{
	// Only used for images
	if (m_displayImage.isNull()) {
		return;
	}

	const bool needScaling = m_settings->value("Zoom/scaleUp", false).toBool()
		|| m_displayImage.width() > m_labelImage->width()
		|| m_displayImage.height() > m_labelImage->height();
	if (needScaling && (onlySize || m_loadedImage || force)) {
		const Qt::TransformationMode mode = onlySize ? Qt::FastTransformation : Qt::SmoothTransformation;
		m_labelImage->setImage(m_displayImage.scaled(m_labelImage->width(), m_labelImage->height(), Qt::KeepAspectRatio, mode));
		m_labelImageScaled = true;
	} else if (m_loadedImage || force || (m_labelImageScaled && !needScaling)) {
		m_labelImage->setImage(m_displayImage);
		m_labelImageScaled = false;
	}

	m_stackedWidget->setCurrentWidget(m_labelImage);
}

Qt::Alignment ZoomWindow::getAlignments(const QString &type)
{
	const QString vertical = m_settings->value(type + "V", "center").toString();
	const QString horizontal = m_settings->value(type + "H", "left").toString();

	const Qt::Alignment vAlign = vertical == "top" ? Qt::AlignTop : (vertical == "bottom" ? Qt::AlignBottom : Qt::AlignVCenter);
	const Qt::Alignment hAlign = horizontal == "left" ? Qt::AlignLeft : (horizontal == "right" ? Qt::AlignRight : Qt::AlignHCenter);

	return vAlign | hAlign;
}

void ZoomWindow::saveNQuit()
{
	if (!m_source.isEmpty()) {
		close();
		return;
	}

	setButtonState(false, SaveButtonState::Saving);
	m_pendingAction = PendingSave;
	m_pendingClose = true;
	pendingUpdate();
}
void ZoomWindow::saveNQuitFav()
{
	if (!m_source.isEmpty()) {
		close();
		return;
	}

	setButtonState(true, SaveButtonState::Saving);
	m_pendingAction = PendingSaveFav;
	m_pendingClose = true;
	pendingUpdate();
}

void ZoomWindow::saveImage(bool fav)
{
	const SaveButtonState state = fav ? m_saveButonStateFav : m_saveButonState;
	switch (state)
	{
		case SaveButtonState::Save:
			setButtonState(fav, SaveButtonState::Saving);
			m_pendingAction = fav ? PendingSaveFav : PendingSave;
			pendingUpdate();
			break;

		case SaveButtonState::Saving:
			return;

		case SaveButtonState::Delete:
		{
			if (m_imagePath.isEmpty() || m_imagePath == m_source) {
				m_imagePath = m_profile->tempPath() + QDir::separator() + QUuid::createUuid().toString().mid(1, 36) + "." + m_image->extension();
			}
			if (QFile::exists(m_imagePath)) {
				QFile::remove(m_source);
			} else {
				QFile::rename(m_source, m_imagePath);
			}
			m_image->setTemporaryPath(m_imagePath);
			m_source = "";
			setButtonState(fav, SaveButtonState::Save);
			m_profile->removeMd5(m_image->md5());
			break;
		}

		default:
			setButtonState(fav, SaveButtonState::Delete);
	}
}
void ZoomWindow::saveImageFav()
{ saveImage(true); }
void ZoomWindow::saveImageNow()
{
	if (m_pendingAction == PendingSaveAs) {
		if (QFile::exists(m_saveAsPending)) {
			QFile::remove(m_saveAsPending);
		}
		bool ok = QFile(m_imagePath).copy(m_saveAsPending);
		auto result = ok ? Image::SaveResult::Saved : Image::SaveResult::Error;
		const Image::Size size = Image::Size::Full; // FIXME: depends on the size of m_imagePath
		m_image->postSave(m_saveAsPending, size, result, true, true, 1);
		saveImageNowSaved(m_image, QList<ImageSaveResult> {{ m_saveAsPending, size, result }});
		return;
	}

	const bool fav = m_pendingAction == PendingSaveFav;
	QString fn = m_settings->value("Save/filename" + QString(fav ? "_favorites" : "")).toString();
	QString pth = m_settings->value("Save/path" + QString(fav ? "_favorites" : "")).toString().replace("\\", "/");
	if (pth.right(1) == "/") {
		pth = pth.left(pth.length() - 1);
	}

	if (pth.isEmpty() || fn.isEmpty()) {
		int reply;
		if (pth.isEmpty()) {
			reply = QMessageBox::question(this, tr("Error"), tr("You did not specified a save folder! Do you want to open the options window?"), QMessageBox::Yes | QMessageBox::No);
		} else {
			reply = QMessageBox::question(this, tr("Error"), tr("You did not specified a save format! Do you want to open the options window?"), QMessageBox::Yes | QMessageBox::No);
		}
		if (reply == QMessageBox::Yes) {
			auto *options = new OptionsWindow(m_profile, parentWidget());
			// options->onglets->setCurrentIndex(3);
			options->setWindowModality(Qt::ApplicationModal);
			options->show();
			connect(options, SIGNAL(closed()), this, SLOT(saveImage()));
		}
		return;
	}

	auto downloader = new ImageDownloader(m_profile, m_image, fn, pth, 1, true, true, this, false);
	connect(downloader, &ImageDownloader::saved, this, &ZoomWindow::saveImageNowSaved);
	connect(downloader, &ImageDownloader::saved, downloader, &ImageDownloader::deleteLater);
	downloader->save();
}
void ZoomWindow::saveImageNowSaved(QSharedPointer<Image> img, const QList<ImageSaveResult> &result)
{
	Q_UNUSED(img)

	const bool fav = m_pendingAction == PendingSaveFav;

	for (const ImageSaveResult &res : result) {
		m_source = res.path;

		switch (res.result)
		{
			case Image::SaveResult::Saved:
				setButtonState(fav, SaveButtonState::Saved);
				break;

			case Image::SaveResult::Copied:
				setButtonState(fav, SaveButtonState::Copied);
				break;

			case Image::SaveResult::Moved:
				setButtonState(fav, SaveButtonState::Moved);
				break;

			case Image::SaveResult::Shortcut:
			case Image::SaveResult::Linked:
				setButtonState(fav, SaveButtonState::Linked);
				break;

			case Image::SaveResult::AlreadyExistsMd5:
				setButtonState(fav, SaveButtonState::ExistsMd5);
				m_source = m_profile->md5Exists(m_image->md5()).first();
				break;

			case Image::SaveResult::AlreadyExistsDeletedMd5:
				setButtonState(fav, SaveButtonState::ExistsMd5);
				break;

			case Image::SaveResult::AlreadyExistsDisk:
				setButtonState(fav, SaveButtonState::ExistsDisk);
				break;

			default:
				error(this, tr("Error saving image."));
				setButtonState(fav, SaveButtonState::Save);
				m_pendingAction = PendingNothing;
				m_pendingClose = false;
				return;
		}
	}

	if (m_pendingClose) {
		close();
	}

	m_pendingAction = PendingNothing;
	m_pendingClose = false;
}

void ZoomWindow::saveImageAs()
{
	const Filename format(m_settings->value("Save/filename").toString());
	const QStringList filenames = format.path(*m_image, m_profile);
	const QString filename = filenames.first().section(QDir::separator(), -1);
	const QString lastDir = m_settings->value("Zoom/lastDir", "").toString();

	QString path = QFileDialog::getSaveFileName(this, tr("Save image"), QDir::toNativeSeparators(lastDir + "/" + filename), "Images (*.png *.gif *.jpg *.jpeg)");
	if (!path.isEmpty()) {
		path = QDir::toNativeSeparators(path);
		m_settings->setValue("Zoom/lastDir", path.section(QDir::separator(), 0, -2));

		m_saveAsPending = path;
		m_pendingAction = PendingSaveAs;
		pendingUpdate();
	}
}


void ZoomWindow::toggleFullScreen()
{
	if (m_isFullscreen) {
		unfullScreen();
	} else {
		fullScreen();
	}
}

void ZoomWindow::fullScreen()
{
	m_fullScreen = new QWidget(this);
	m_fullScreen->setStyleSheet("background-color: black");
	m_fullScreen->setWindowFlags(Qt::Window);
	m_fullScreen->showFullScreen();

	auto *layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(m_stackedWidget);
	m_fullScreen->setLayout(layout);

	m_isFullscreen = true;
	prepareNextSlide();

	m_settings->beginGroup("Zoom/Shortcuts");	// Could probably just use the variables already initialised when this ZoomWindow was constructed.
		QShortcut *quit = new QShortcut(getKeySequence(m_settings, "keyQuit", Qt::Key_Escape), m_fullScreen);
			connect(quit, &QShortcut::activated, this, &ZoomWindow::unfullScreen);
		QShortcut *toggleFullscreen = new QShortcut(getKeySequence(m_settings, "keyToggleFullscreen", QKeySequence::FullScreen, Qt::Key_F11), m_fullScreen);
			connect(toggleFullscreen, &QShortcut::activated, this, &ZoomWindow::unfullScreen);
		QShortcut *prev = new QShortcut(getKeySequence(m_settings, "keyPrev", Qt::Key_Left), m_fullScreen);
			connect(prev, &QShortcut::activated, this, &ZoomWindow::previous);
		QShortcut *next = new QShortcut(getKeySequence(m_settings, "keyNext", Qt::Key_Right), m_fullScreen);
			connect(next, &QShortcut::activated, this, &ZoomWindow::next);
		QShortcut *toggleSlideshow = new QShortcut(getKeySequence(m_settings, "keyToggleSlideshow", Qt::Key_Space), m_fullScreen);
			connect(toggleSlideshow, &QShortcut::activated, this, &ZoomWindow::toggleSlideshow);
	m_settings->endGroup();

	m_fullScreen->setFocus();
}

void ZoomWindow::unfullScreen()
{
	m_slideshow.stop();

	if (m_fullScreen != nullptr) {
		ui->verticalLayout->insertWidget(1, m_stackedWidget, 1);

		m_fullScreen->close();
		m_fullScreen->deleteLater();
		m_fullScreen = nullptr;
	}

	m_isFullscreen = false;
	update(true);
}

void ZoomWindow::prepareNextSlide()
{
	// Slideshow is only enabled in fullscreen
	if (!m_isFullscreen) {
		return;
	}

	// If the slideshow is disabled
	const int interval = m_settings->value("slideshow", 0).toInt();
	if (interval <= 0) {
		return;
	}

	// We make sure to wait to see the whole displayed item
	const qint64 additionalInterval = !m_isAnimated.isEmpty()
		? m_gifPlayer->duration()
		: (m_image->isVideo() ? m_videoPlayer->duration() : 0);

	const qint64 totalInterval = interval * 1000 + additionalInterval;
	m_slideshow.start(totalInterval);
	m_isSlideshowRunning = true;
}

void ZoomWindow::toggleSlideshow()
{
	m_isSlideshowRunning = !m_isSlideshowRunning;

	if (!m_isSlideshowRunning) {
		m_slideshow.stop();
	} else {
		prepareNextSlide();
	}
}



void ZoomWindow::resizeEvent(QResizeEvent *e)
{
	if (!m_resizeTimer->isActive()) {
		m_timeout = qMin(500, qMax(50, (m_displayImage.width() * m_displayImage.height()) / 100000));
	}
	m_resizeTimer->stop();
	m_resizeTimer->start(m_timeout);
	update(true);

	QWidget::resizeEvent(e);
}

void ZoomWindow::closeEvent(QCloseEvent *e)
{
	m_settings->setValue("Zoom/geometry", saveGeometry());
	m_settings->setValue("Zoom/plus", ui->buttonPlus->isChecked());
	m_settings->sync();

	m_image->abortTags();

	for (auto it = m_imageDownloaders.constBegin(); it != m_imageDownloaders.constEnd(); ++it) {
		it.value()->abort();
		it.value()->deleteLater();
	}

	e->accept();
}

void ZoomWindow::showEvent(QShowEvent *e)
{
	Q_UNUSED(e)

	showThumbnail();
}

void ZoomWindow::showThumbnail()
{
	QSize size = m_image->size();
	if (size.isEmpty()) {
		size = m_image->previewImage().size() * 2 * m_settings->value("thumbnailUpscale", 1.0).toDouble();
	}

	// Videos get a static resizable overlay
	if (m_image->isVideo() && !m_settings->value("Zoom/useVideoPlayer", true).toBool()) {
		// A video thumbnail should not be upscaled to more than three times its size
		QSize maxSize = QSize(500, 500) * m_settings->value("thumbnailUpscale", 1.0).toDouble();
		if (size.width() > maxSize.width() || size.height() > maxSize.height()) {
			size.scale(maxSize, Qt::KeepAspectRatio);
		}

		const QPixmap &base = m_image->previewImage();
		QPixmap overlay = QPixmap(":/images/play-overlay.png");
		QPixmap result(size.width(), size.height());
		result.fill(Qt::transparent);
		{
			QPainter painter(&result);
			painter.drawPixmap(0, 0, size.width(), size.height(), base);
			painter.drawPixmap(qMax(0, (size.width() - overlay.width()) / 2), qMax(0, (size.height() - overlay.height()) / 2), overlay.width(), overlay.height(), overlay);
		}
		m_displayImage = result;
		update(false, true);
	}
	// Gifs get non-resizable thumbnails
	else if (!m_isAnimated.isEmpty() || m_image->isVideo()) {
		m_labelImage->setPixmap(m_image->previewImage().scaled(size, Qt::IgnoreAspectRatio, Qt::FastTransformation));
	}
	// Other images get a resizable thumbnail
	else if (m_displayImage.isNull()) {
		m_displayImage = m_image->previewImage().scaled(size, Qt::IgnoreAspectRatio, Qt::FastTransformation);
		update(false, true);
	}
}


void ZoomWindow::reuse(const QList<QSharedPointer<Image>> &images, const QSharedPointer<Image> &image, Site *site)
{
	m_images = images;
	m_site = site;

	load(image);
}

void ZoomWindow::load(const QSharedPointer<Image> &image)
{
	emit clearLoadQueue();

	m_displayImage = QPixmap();
	m_loadedImage = false;
	m_source = "";
	m_imagePath = "";
	m_image = image;
	m_isAnimated = image->isAnimated();
	m_size = 0;
	ui->labelLoadingError->hide();

	// Show the thumbnail if the image was not already preloaded
	if (isVisible()) {
		showThumbnail();
	}

	// Preload and abort next and previous images
	const int preload = m_settings->value("preload", 0).toInt();
	QSet<int> preloaded;
	const int index = m_images.indexOf(m_image);
	for (int i = index - preload - 1; i <= index + preload + 1; ++i) {
		bool forAbort = i == index - preload - 1 || i == index + preload + 1;
		int pos = (i + m_images.count()) % m_images.count();
		if (pos < 0 || pos == index || pos > m_images.count() || preloaded.contains(pos)) {
			continue;
		}

		QSharedPointer<Image> img = m_images[pos];
		bool downloaderExists = m_imageDownloaders.contains(img);
		if (downloaderExists && forAbort) {
			m_imageDownloaders[img]->abort();
		}
		if (downloaderExists || forAbort || (!img->savePath().isEmpty() && QFile::exists(img->savePath()))) {
			continue;
		}

		preloaded.insert(pos);
		log(QStringLiteral("Preloading data for image #%1").arg(pos));
		m_images[pos]->loadDetails();

		const Filename fn = Filename(QUuid::createUuid().toString().mid(1, 36) + ".%ext%");
		const QStringList paths = fn.path(*img.data(), m_profile, m_profile->tempPath(), 1, Filename::ExpandConditionals | Filename::Path);
		const Image::Size size = img->preferredDisplaySize();
		auto dwl = new ImageDownloader(m_profile, img, paths, 1, false, false, this, true, false, size, false, true);
		m_imageDownloaders.insert(img, dwl);
		dwl->save();
	}

	// Reset buttons
	setButtonState(false, SaveButtonState::Save);
	setButtonState(true, SaveButtonState::Save);

	// Window title
	updateWindowTitle();

	prepareNextSlide();
	go();
}

void ZoomWindow::updateWindowTitle()
{
	QStringList infos;

	// Extension
	infos.append(getExtension(m_image->fileUrl()).toUpper());

	// Filesize
	if (m_image->fileSize() != 0) {
		infos.append(formatFilesize(m_image->fileSize()));
	}

	// Image size
	if (!m_image->size().isEmpty()) {
		infos.append(QStringLiteral("%1 x %2").arg(m_image->size().width()).arg(m_image->size().height()));
	}

	// Update title if there are infos to show
	QString title;
	if (infos.isEmpty()) {
		title = tr("Image");
	} else {
		title = QString(tr("Image") + " (%1)").arg(infos.join(", "));
	}
	setWindowTitle(QStringLiteral("%1 - %2 (%3/%4)").arg(title, m_image->parentSite()->name(), QString::number(m_images.indexOf(m_image) + 1), QString::number(m_images.count())));
}

int ZoomWindow::firstNonBlacklisted(int direction)
{
	int index = m_images.indexOf(m_image);
	const int first = index;
	index = (index + m_images.count() + direction) % m_images.count();

	// Skip blacklisted images
	while (!m_profile->getBlacklist().match(m_images[index]->tokens(m_profile)).isEmpty() && index != first) {
		index = (index + m_images.count() + direction) % m_images.count();
	}

	return index;
}

void ZoomWindow::next()
{
	m_image->abortTags();

	const int index = firstNonBlacklisted(+1);
	load(m_images[index]);
}

void ZoomWindow::previous()
{
	m_image->abortTags();

	const int index = firstNonBlacklisted(-1);
	load(m_images[index]);
}

void ZoomWindow::updateButtonPlus()
{
	ui->buttonPlus->setText(QChar(ui->buttonPlus->isChecked() ? '-' : '+'));
}

void ZoomWindow::openFile(bool now)
{
	if (!now) {
		m_pendingAction = PendingOpen;
		pendingUpdate();
		return;
	}

	QDesktopServices::openUrl(QUrl::fromLocalFile(m_imagePath));
	m_pendingAction = PendingNothing;
}

void ZoomWindow::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() == Qt::MiddleButton && m_settings->value("imageCloseMiddleClick", true).toBool()) {
		close();
		return;
	}

	QWidget::mouseReleaseEvent(e);
}

void ZoomWindow::wheelEvent(QWheelEvent *e)
{
	if (m_settings->value("imageNavigateScroll", true).toBool()) {
		// Ignore events triggered when reaching the bottom of the tag list
		if (ui->scrollArea->underMouse()) {
			return;
		}

		// Ignore events if we already got one less than 500ms ago
		if (!m_lastWheelEvent.isNull() && m_lastWheelEvent.elapsed() <= 500) {
			e->ignore();
		}
		m_lastWheelEvent.start();

		const int angle = e->angleDelta().y();
		if (angle <= -120) { // Scroll down
			next();
			return;
		}
		if (angle >= 120) { // Scroll up
			previous();
			return;
		}
	}

	QWidget::wheelEvent(e);
}

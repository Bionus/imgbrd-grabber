#include "viewer/viewer-window.h"
#include <QApplication>
#include <QClipboard>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
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
#include <ui_viewer-window.h>
#include "custom-buttons.h"
#include "downloader/image-downloader.h"
#include "filename/filename.h"
#include "functions.h"
#include "helpers.h"
#include "image-context-menu.h"
#include "logger.h"
#include "main-window.h"
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


ViewerWindow::ViewerWindow(QList<QSharedPointer<Image>> images, const QSharedPointer<Image> &image, Site *site, Profile *profile, MainWindow *parent, SearchTab *tab)
	: QWidget(nullptr, Qt::Window), m_parent(parent), m_tab(tab), m_profile(profile), m_favorites(profile->getFavorites()), m_viewItLater(profile->getKeptForLater()), m_ignore(profile->getIgnored()), m_settings(profile->getSettings()), ui(new Ui::ViewerWindow), m_site(site), m_timeout(300), m_tooBig(false), m_loadedImage(false), m_loadedDetails(false), m_finished(false), m_size(0), m_isFullscreen(false), m_isSlideshowRunning(false), m_images(std::move(images)), m_displayImage(QPixmap()), m_labelImageScaled(false)
{
	setAttribute(Qt::WA_DeleteOnClose);
	connect(parent, &MainWindow::destroyed, this, &QWidget::deleteLater);

	ui->setupUi(this);

	m_pendingAction = PendingNothing;
	m_pendingClose = false;

	if (m_settings->value("Viewer/rememberGeometry", true).toBool()) {
		restoreGeometry(m_settings->value("Viewer/geometry").toByteArray());
	}
	
	ui->progressBarDownload->hide();

	m_settings->beginGroup("Viewer/Shortcuts");
		auto *quit = new QShortcut(getKeySequence(m_settings, "keyQuit", Qt::Key_Escape), this);
			connect(quit, &QShortcut::activated, this, &ViewerWindow::close);
		auto *prev = new QShortcut(getKeySequence(m_settings, "keyPrev", Qt::Key_Left), this);
			connect(prev, &QShortcut::activated, this, &ViewerWindow::previous);
		auto *next = new QShortcut(getKeySequence(m_settings, "keyNext", Qt::Key_Right), this);
			connect(next, &QShortcut::activated, this, &ViewerWindow::next);

		auto *details = new QShortcut(getKeySequence(m_settings, "keyDetails", Qt::Key_D), this);
			connect(details, &QShortcut::activated, this, &ViewerWindow::showDetails);
		auto *saveAs = new QShortcut(getKeySequence(m_settings, "keySaveAs", QKeySequence::SaveAs, Qt::CTRL | Qt::SHIFT | Qt::Key_S), this);
			connect(saveAs, &QShortcut::activated, this, &ViewerWindow::saveImageAs);

		auto *save = new QShortcut(getKeySequence(m_settings, "keySave", QKeySequence::Save, Qt::CTRL | Qt::Key_S), this);
			connect(save, SIGNAL(activated()), this, SLOT(saveImage()));
		auto *SNQ = new QShortcut(getKeySequence(m_settings, "keySaveNQuit", Qt::CTRL | Qt::Key_W), this);
			// Pointer name must not overlap with function name ("saveNQuit"...
			connect(SNQ, SIGNAL(activated()), this, SLOT(saveNQuit()));
		auto *open = new QShortcut(getKeySequence(m_settings, "keyOpen", Qt::CTRL | Qt::Key_O), this);
			connect(open, SIGNAL(activated()), this, SLOT(openSaveDir()));

		auto *saveFav = new QShortcut(getKeySequence(m_settings, "keySaveFav", Qt::CTRL | Qt::ALT | Qt::Key_S), this);
			connect(saveFav, &QShortcut::activated, this, [this]{saveImage(true);});
		auto *saveNQuitFav = new QShortcut(getKeySequence(m_settings, "keySaveNQuitFav", Qt::CTRL | Qt::ALT | Qt::Key_W), this);
			connect(saveNQuitFav, &QShortcut::activated, this, [this]{saveNQuit(true);});
		auto *openFav = new QShortcut(getKeySequence(m_settings, "keyOpenFav", Qt::CTRL | Qt::ALT | Qt::Key_O), this);
			connect(openFav, &QShortcut::activated, this, [this]{openSaveDir(true);});

		auto *toggleFullscreen = new QShortcut(getKeySequence(m_settings, "keyToggleFullscreen", QKeySequence::FullScreen, Qt::Key_F11), this);
			connect(toggleFullscreen, &QShortcut::activated, this, &ViewerWindow::toggleFullScreen);
		auto *copyDataToClipboard = new QShortcut(getKeySequence(m_settings, "keyDataToClipboard", QKeySequence::Copy, Qt::CTRL | Qt::SHIFT | Qt::Key_C), this);
			connect(copyDataToClipboard, &QShortcut::activated, this, &ViewerWindow::copyImageDataToClipboard);

		auto *openInBrowser = new QShortcut(getKeySequence(m_settings, "keyOpenInBrowser"), this);
			connect(openInBrowser, &QShortcut::activated, [this]() { QDesktopServices::openUrl(m_image->pageUrl()); });
	m_settings->endGroup();

	configureButtons();

	m_labelTagsLeft = new QAffiche(QVariant(), 0, QColor(), this);
		m_labelTagsLeft->setContextMenuPolicy(Qt::CustomContextMenu);
		m_labelTagsLeft->setTextInteractionFlags(Qt::TextBrowserInteraction);
		connect(m_labelTagsLeft, &QAffiche::customContextMenuRequested, this, &ViewerWindow::contextMenu);
		connect(m_labelTagsLeft, &QAffiche::linkActivated, this, &ViewerWindow::openUrl);
		connect(m_labelTagsLeft, &QAffiche::linkHovered, this, &ViewerWindow::linkHovered);
		connect(m_labelTagsLeft, SIGNAL(middleClicked()), this, SLOT(openInNewTab()));
		ui->scrollAreaWidgetContents->layout()->addWidget(m_labelTagsLeft);

	m_labelTagsTop = new QAffiche(QVariant(), 0, QColor(), this);
		m_labelTagsTop->setWordWrap(true);
		m_labelTagsTop->setContextMenuPolicy(Qt::CustomContextMenu);
		m_labelTagsTop->setTextInteractionFlags(Qt::TextBrowserInteraction);
		connect(m_labelTagsTop, &QAffiche::customContextMenuRequested, this, &ViewerWindow::contextMenu);
		connect(m_labelTagsTop, &QAffiche::linkActivated, this, &ViewerWindow::openUrl);
		connect(m_labelTagsTop, &QAffiche::linkHovered, this, &ViewerWindow::linkHovered);
		connect(m_labelTagsTop, SIGNAL(middleClicked()), this, SLOT(openInNewTab()));
		ui->verticalLayout->insertWidget(0, m_labelTagsTop, 0);

	// Automatically re-color if some settings change
	connect(m_profile, &Profile::favoritesChanged, this, &ViewerWindow::colore);
	connect(m_profile, &Profile::keptForLaterChanged, this, &ViewerWindow::colore);
	connect(m_profile, &Profile::ignoredChanged, this, &ViewerWindow::colore);
	connect(m_profile, &Profile::blacklistChanged, this, &ViewerWindow::colore);

	m_stackedWidget = new QStackedWidget(this);
		ui->verticalLayout->insertWidget(1, m_stackedWidget, 1);
	m_labelImage = new QAffiche(QVariant(), 0, QColor(), this);
		m_labelImage->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
		m_labelImage->setAlignment(getAlignments("imagePositionImage"));
		connect(m_labelImage, SIGNAL(doubleClicked()), this, SLOT(openFile()));
		m_stackedWidget->addWidget(m_labelImage);

	m_gifPlayer = new GifPlayer(m_settings->value("Viewer/showGifPlayerControls", true).toBool(), getAlignments("imagePositionAnimation"), this);
	m_stackedWidget->addWidget(m_gifPlayer);

	if (m_settings->value("Viewer/useVideoPlayer", true).toBool()) {
		// getAlignments("imagePositionVideo")
		m_videoPlayer = new VideoPlayer(m_settings->value("Viewer/showVideoPlayerControls", true).toBool(), this);
		m_stackedWidget->addWidget(m_videoPlayer);
	}

	m_labelImage->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_labelImage, &QAffiche::customContextMenuRequested, this, &ViewerWindow::imageContextMenu);

	m_slideshow.setSingleShot(true);
	connect(&m_slideshow, &QTimer::timeout, this, &ViewerWindow::next);

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
	connect(this, &ViewerWindow::loadImage, m_imageLoaderQueue, &ImageLoaderQueue::load);
	connect(this, &ViewerWindow::clearLoadQueue, m_imageLoaderQueue, &ImageLoaderQueue::clear);
	connect(m_imageLoaderQueue, &ImageLoaderQueue::finished, this, &ViewerWindow::display);
	m_imageLoaderQueueThread.start();
	m_imageLoaderThread.start();

	// Background color
	QString bg = m_settings->value("imageBackgroundColor", "").toString();
	if (!bg.isEmpty()) {
		setStyleSheet("#ViewerWindow { background-color:" + bg + "; }");
	}

	connect(&m_resizeTimer, SIGNAL(timeout()), this, SLOT(update()));
	m_resizeTimer.setSingleShot(true);

	load(image);
}
void ViewerWindow::go()
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
	connect(m_image.data(), &Image::finishedLoadingTags, this, &ViewerWindow::replyFinishedDetails, Qt::UniqueConnection);
	m_image->loadDetails();

	if (!m_isFullscreen) {
		activateWindow();
	}
}

/**
 * Destructor of the ViewerWindow class
 */
ViewerWindow::~ViewerWindow()
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

void ViewerWindow::configureButtons()
{
	log("+++configureButtons+++", Logger::Debug);

	bool drawerIsOpen = m_settings->value("Viewer/rememberDrawer", true).toBool() && m_settings->value("Viewer/plus", true).toBool(); 
	if (drawerIsOpen) {
		ui->buttonPlus->setText(QChar('-'));
	}

	unsigned short countOnShelf = 0;
	std::vector<short> maxColPos;
	std::vector<unsigned int> spanSum;
	std::vector<std::vector<unsigned short>> spans;
	ui->buttonsLayout->setOriginCorner(Qt::BottomLeftCorner);

	// Load button configuration from settings
	QList<ButtonSettings> bss = m_settings->value("Viewer/activeButtons").value<QList<ButtonSettings>>();
	for (auto &bs : bss) {
		auto *pushButton = new QPushButton(this);
		m_buttons.insert(std::pair<QString, ButtonInstance>(
			bs.name,
			ButtonInstance{bs.type, pushButton, bs.states}
		));

		unsigned short row;
		if (bs.isInDrawer) {
			m_drawerButtons.push_back(pushButton);
			row = 1;
			if (!drawerIsOpen) {
				pushButton->setVisible(false);
			}
		} else {
			countOnShelf++;
			row = 0;
		}

		// If the button is on a new row, initialize values
		if (maxColPos.size() <= row) {
			while (maxColPos.size() <= row) {
				maxColPos.push_back(-1);
				spanSum.push_back(0);
			}
			spans.resize(row + 1);
		}
		spans.at(row).push_back(bs.relativeWidth - 1); // Don't count starting position

		unsigned short effectivePosition = (bs.position > maxColPos.at(row) ? ++maxColPos.at(row) : bs.position) + spanSum.at(row);	// Make columns contiguous.
		if (bs.relativeWidth > 1) {
			spanSum.at(row) += bs.relativeWidth - 1;
		}

		log("Adding button to grid: " + QString::number(row) + "," + QString::number(effectivePosition) + ",1," + QString::number(bs.relativeWidth), Logger::Debug);
		ui->buttonsLayout->addWidget(pushButton, row, effectivePosition, 1, bs.relativeWidth);
	}

	unsigned short biggestMaxRow, biggestMaxColPos = 0;
	for (unsigned short i = 0; i < maxColPos.size(); i++) {
		unsigned short spanBack = spans.at(i).empty() ? 0 : spans.at(i).back();
		if (maxColPos.at(i) + spanBack > biggestMaxColPos) {
			biggestMaxRow = i;
			biggestMaxColPos = maxColPos.at(i) + spanBack;
			log(("biggestMaxColPos = " + std::to_string(maxColPos.at(i) + spanBack) + " = " + std::to_string(maxColPos.at(i)) + " + " + std::to_string(spanBack)).c_str(), Logger::Debug);
		}

		maxColPos.at(i) += spanBack;	// Redefine as end, rather than beginning, position.

		// Configure rows:
		ui->buttonsLayout->setRowStretch(i, 1);
	}

	// Give all columns a stretch factor of 1 to make sure they're all the same size
	for (int i = 0; i < ui->buttonsLayout->columnCount(); i++) {
		ui->buttonsLayout->setColumnStretch(i, 1);
	}

	//  If there are buttons in the drawer, we add the "+" button
	if (!m_drawerButtons.empty()) {
		log(("Adding buttonPlus to grid: " + std::to_string(maxColPos.size()) + "," + std::to_string(ui->buttonsLayout->columnCount()/2) + ",1," + std::to_string(ui->buttonsLayout->columnCount()%2 ? 1 : 2)).c_str(), Logger::Debug);
		ui->buttonsLayout->addWidget(ui->buttonPlus, maxColPos.size(), ui->buttonsLayout->columnCount()/2, 1, ui->buttonsLayout->columnCount()%2 ? 1 : 2);
		ui->buttonsLayout->setRowStretch(maxColPos.size(), 1);
		ui->buttonPlus->setChecked(drawerIsOpen);
		connect(ui->buttonPlus, &QPushButton::toggled, this, &ViewerWindow::updateButtonPlus);
	}
	// Otherwise, if there are other buttons, we only delete the "+" button
	else if (countOnShelf) {
		ui->buttonPlus->deleteLater();
	}
	// Otherwise, that means there are no buttons at all, and we delete the whole button layout and stop here
	else {
		ui->buttonsLayout->deleteLater();
		log("---configureButtons---", Logger::Debug);
		return;
	}

	for (unsigned short row = 0; row < spans.size(); row++) {
		spanSum.at(row) += spans.at(row).size(); // Make spanSum include count of initial column positions for each row.
	}

	/*bool biggestIsOdd = spans.at(biggestMaxRow).size()%2;
	std::vector<unsigned short> rescalingOffset (maxColPos.size(), 0);
	//std::vector<float> rescalingOffset (maxColPos.size(), 0);*/
	for (auto &it : m_buttons) {
		QPushButton *button = it.second.pointer;

		// Re-center each row based on flexible column count:
		int originRow, originCol, originRowSpan, originColSpan;
		ui->buttonsLayout->getItemPosition(ui->buttonsLayout->indexOf(button), &originRow, &originCol, &originRowSpan, &originColSpan);
		// Note: these spanSum values may not account for the width of the last button on each row. Not sure if important.
		unsigned short offset = ( spanSum.at(biggestMaxRow) - spanSum.at(originRow) ) / 2;	// Row content is centred-- offset using free space on left side.
		log(("Total diff from reference = " + std::to_string(spanSum.at(biggestMaxRow)) + " - " + std::to_string(spanSum.at(originRow))).c_str(), Logger::Debug);
		if (offset != 0) {
			//if (rescalingOffset.at(originRow)) log(("rescalingOffset.at(row) = " + std::to_string(rescalingOffset.at(originRow))).c_str(), Logger::Debug);
			unsigned short newCol = originCol + offset;
			log(("Repositioning button on row " + std::to_string(originRow) + " : " + std::to_string(originCol) + " -> " + std::to_string(newCol)).c_str(), Logger::Debug);
			//unsigned short newCol = originCol + offset + rescalingOffset.at(originRow);
			ui->buttonsLayout->addWidget(button, originRow, newCol, originRowSpan, originColSpan);
			/*if (biggestIsOdd ^ spans.at(originRow).size()%2) {	// Adjust spans and starting columns to compensate for mismatched numbers of buttons on rows.
				log("Rescaling due to even/odd mismatch with reference row.", Logger::Debug);
				ui->buttonsLayout->addWidget(button, originRow, newCol, originRowSpan, originColSpan + 1);	// Buttons aren't in order!
				rescalingOffset.at(originRow)++;
			} else ui->buttonsLayout->addWidget(button, originRow, newCol, originRowSpan, originColSpan);*/
		}


		// Generic button configuration:

			// Hard coded:
		button->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

			// From state:
		ButtonState *state = it.second.current = &(it.second.states.first()); // Consider using [].
		const ButtonState &defaultState = ViewerWindowButtons::DefaultStates.value(it.second.type);
		button->setText(QObject::tr((state->text.isEmpty() ? defaultState.text : state->text).toStdString().c_str()).replace("&", "&&"));
		button->setToolTip(QObject::tr((state->toolTip.isEmpty() ? defaultState.toolTip : state->toolTip).toStdString().c_str()));

			// Initialise state 0 functions. This should be eliminated if possible.
		switch (it.second.type) {
			case CustomButtons::IsButtonPrev:
				state->function = [this]{ previous(); };
				break;
			case CustomButtons::IsButtonNext:
				state->function = [this]{ next(); };
				break;
			case CustomButtons::IsButtonDetails:
				state->function = [this]{ showDetails(); };
				break;
			case CustomButtons::IsButtonSaveAs:
				state->function = [this]{ saveImageAs(); };
				break;
			case CustomButtons::IsButtonSave:
				state->function = [this]{ saveImage(false); };
				break;
			case CustomButtons::IsButtonSaveNQuit:
				state->function = [this]{ saveNQuit(false); };
				break;
			case CustomButtons::IsButtonOpen:
				state->function = [this]{ openSaveDir(false); };
				break;
			case CustomButtons::IsButtonSave | CustomButtons::IsFavoriteButton:
				state->function = [this]{ saveImage(true); };
				break;
			case CustomButtons::IsButtonSaveNQuit | CustomButtons::IsFavoriteButton:
				state->function = [this]{ saveNQuit(true); };
				break;
			case CustomButtons::IsButtonOpen | CustomButtons::IsFavoriteButton:
				state->function = [this]{ openSaveDir(true); };
				break;
			default :
				state->function = nullptr;
				log(QStringLiteral("Failed to set function for unknown button type '%1'").arg(it.second.type), Logger::Error);
				continue;
		}

		bool connected = connect(button, &QPushButton::clicked, state->function);
		log("Connection for button " + it.first + " " + (connected ? "succeeded" : "failed!"), Logger::Debug);
	}

	log("rowCount: " + QString::number(ui->buttonsLayout->rowCount()), Logger::Debug);
	log("columnCount: " + QString::number(ui->buttonsLayout->columnCount()), Logger::Debug);

	log("---configureButtons---", Logger::Debug);
}

void ViewerWindow::imageContextMenu()
{
	QMenu *menu = new ImageContextMenu(m_settings, m_image, m_parent, this);

	// Reload action
	auto *reloadImageAction = new QAction(QIcon(":/images/icons/update.png"), tr("Reload"), menu);
	connect(reloadImageAction, &QAction::triggered, this, &ViewerWindow::reloadImage);

	// Copy actions
	auto *copyImageAction = new QAction(QIcon(":/images/icons/copy.png"), tr("Copy file"), menu);
	connect(copyImageAction, &QAction::triggered, this, &ViewerWindow::copyImageFileToClipboard);
	auto *copyDataAction = new QAction(QIcon(":/images/icons/document-binary.png"), tr("Copy data"), menu);
	connect(copyDataAction, &QAction::triggered, this, &ViewerWindow::copyImageDataToClipboard);
	auto *copyLinkAction = new QAction(QIcon(":/images/icons/globe.png"), tr("Copy link"), menu);
	connect(copyLinkAction, &QAction::triggered, this, &ViewerWindow::copyImageLinkToClipboard);

	// Insert actions at the beginning
	QAction *first = menu->actions().first();
	menu->insertAction(first, reloadImageAction);
	menu->insertSeparator(first);
	menu->insertAction(first, copyImageAction);
	menu->insertAction(first, copyDataAction);
	menu->insertAction(first, copyLinkAction);
	menu->insertSeparator(first);

	menu->exec(QCursor::pos());
}

void ViewerWindow::reloadImage()
{
	load(true);
}
void ViewerWindow::copyImageFileToClipboard()
{
	auto *mimeData = new QMimeData();
	mimeData->setUrls({ QUrl::fromLocalFile(m_imagePath) });
	QApplication::clipboard()->setMimeData(mimeData);
}
void ViewerWindow::copyImageDataToClipboard()
{
	QApplication::clipboard()->setPixmap(m_displayImage);
}
void ViewerWindow::copyImageLinkToClipboard()
{
	QApplication::clipboard()->setText(m_image->fileUrl().toEncoded());
}

void ViewerWindow::showDetails()
{
	m_detailsWindow->setImage(m_image);
	m_detailsWindow->show();
}

void ViewerWindow::openUrl(const QString &url)
{ emit linkClicked(QUrl::fromPercentEncoding(url.toUtf8())); }
void ViewerWindow::openPool(const QString &url)
{
	if (url.startsWith(QLatin1String("pool:"))) {
		emit poolClicked(url.right(url.length() - 5).toInt(), m_image->parentSite()->url());
	} else {
		Page *p = new Page(m_profile, m_image->parentSite(), m_profile->getSites().values(), QStringList { "id:" + url }, 1, 1, QStringList(), false, this);
		connect(p, &Page::finishedLoading, this, &ViewerWindow::openPoolId);
		p->load();
	}
}
void ViewerWindow::openPoolId(Page *p)
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

void ViewerWindow::openSaveDir(bool fav)
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

void ViewerWindow::linkHovered(const QString &url)
{ m_link = QUrl::fromPercentEncoding(url.toUtf8()); }
void ViewerWindow::contextMenu(const QPoint &pos)
{
	Q_UNUSED(pos)

	if (m_link.isEmpty()) {
		return;
	}

	Page page(m_profile, m_site, { m_site }, QStringList { m_link });
	auto *menu = new TagContextMenu(m_link, m_image->tags(), page.friendlyUrl(), m_profile, { m_site }, true, this);
	connect(menu, &TagContextMenu::openNewTab, this, &ViewerWindow::openInNewTab);
	connect(menu, &TagContextMenu::setFavoriteImage, this, &ViewerWindow::setfavorite);
	menu->exec(QCursor::pos());
}

void ViewerWindow::openInNewTab()
{
	if (!m_link.isEmpty()) {
		m_parent->addTab(m_link, false, true, m_tab);
	}
}
void ViewerWindow::setfavorite()
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

void ViewerWindow::load(bool force)
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
	connect(dwl, &ImageDownloader::downloadProgress, this, &ViewerWindow::downloadProgress, Qt::UniqueConnection);
	connect(dwl, &ImageDownloader::saved, this, &ViewerWindow::replyFinishedImage, Qt::UniqueConnection);

	m_imageTime.start();

	if (!dwl->isRunning()) {
		dwl->save();
	}
}

#define PERCENT 0.05
#define TIME 500
void ViewerWindow::downloadProgress(QSharedPointer<Image> img, qint64 bytesReceived, qint64 bytesTotal)
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
void ViewerWindow::display(const QPixmap &pix, int size)
{
	if (!pix.size().isEmpty() && size >= m_size) {
		m_size = size;
		m_displayImage = pix;
		update(!m_finished);

		updateWindowTitle();
	}
}

void ViewerWindow::replyFinishedDetails()
{
	disconnect(m_image.data(), &Image::finishedLoadingTags, this, &ViewerWindow::replyFinishedDetails);

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

		// Fix extension when it should be guessed
		const QString fext = imagePath.section('.', -1);
		m_image->setFileExtension(fext);

		// Ensure the file actually exists before trying to set it as the source
		if (QFile::exists(imagePath)) {
			m_imagePath = imagePath;	// Moved before setButtonState so tool tips can reference it.

			// Update save button state
			const SaveButtonState md5State = !md5Exists.isEmpty() && md5Action != "save" ? SaveButtonState::ExistsMd5 : SaveButtonState::Save;
			setButtonState(false, !source1.isEmpty() ? SaveButtonState::ExistsDisk : md5State);
			setButtonState(true, !source2.isEmpty() ? SaveButtonState::ExistsDisk : md5State);

			m_source = imagePath;
			m_image->setSavePath(m_source);

			log(QStringLiteral("Image loaded from the file `%1`").arg(imagePath));
			m_finished = true;
			m_loadedImage = true;
			pendingUpdate();

			draw();
		}
	}
	// If the file does not exist, we have to load it
	else {
		load();
	}

	updateWindowTitle();
}
void ViewerWindow::colore()
{
	const QStringList t = TagStylist(m_profile).stylished(m_image->tags(), m_settings->value("Viewer/showTagCount", false).toBool(), false, m_settings->value("Viewer/tagOrder", "type").toString());
	const QString tags = t.join(' ');

	if (ui->widgetLeft->isHidden()) {
		m_labelTagsTop->setText(tags);
	} else {
		m_labelTagsLeft->setText(t.join(QStringLiteral("<br/>")));
		ui->scrollArea->setMinimumWidth(m_labelTagsLeft->sizeHint().width() + ui->scrollArea->verticalScrollBar()->sizeHint().width());
	}

	m_detailsWindow->setImage(m_image);
}

void ViewerWindow::setButtonState(bool fav, SaveButtonState state)
{
	constexpr unsigned short MaxSaveButtons = 2;
	std::unordered_map<QString, ButtonInstance>::iterator relevant[MaxSaveButtons];

	// Find all save state related buttons that should be updated when the state changes
	if (fav) {
		m_saveButtonStateFav = state;
		relevant[0] = m_buttons.find("SaveNQuitFav");
		relevant[1] = m_buttons.find("SaveFav");
	} else {
		m_saveButtonState = state;
		relevant[0] = m_buttons.find("SaveNQuit");
		relevant[1] = m_buttons.find("Save");
	}

	// Loop through all relevant buttons
	for (unsigned short i = 0; i != MaxSaveButtons; i++) {
		// If the button was not found in "buttons" (if disabled for example), ignore it
		if (relevant[i] == m_buttons.end()) {
			continue;
		}
		ButtonInstance *button = &(relevant[i]->second);

		// Find the next button state
		ButtonState *newState = button->states.size() <= state
			? &(button->states.back()) // Last state is used by default, like a switch case default
			: &(button->states[state]);
		button->current = newState;

		// Update button text
		const ButtonState &defaultState = ViewerWindowButtons::DefaultStates.value(button->type);
		button->pointer->setText(QObject::tr((newState->text.isEmpty() ? defaultState.text : newState->text).toStdString().c_str()).replace("&", "&&"));
		button->pointer->setToolTip(QObject::tr((newState->toolTip.isEmpty() ? defaultState.toolTip : newState->toolTip).toStdString().c_str()));

		// Connect button to its new action
		if (newState->function == nullptr) {
			newState->function = button->states.at(0).function;
		}
		disconnect(button->pointer, &QPushButton::clicked, nullptr, nullptr);
		connect(button->pointer, &QPushButton::clicked, newState->function);
	}
}

void ViewerWindow::replyFinishedImage(const QSharedPointer<Image> &img, const QList<ImageSaveResult> &result)
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
		showLoadingError(tr("File not found."));
	} else if (res.result == Image::SaveResult::NetworkError) {
		showLoadingError(tr("Error loading the image."));
	} else if (res.result == Image::SaveResult::DetailsLoadError) {
		showLoadingError(tr("Error loading the image's details."));
	} else if (res.result == Image::SaveResult::Error) {
		showLoadingError(tr("Error saving the image."));
	} else {
		m_imagePath = res.path;
		m_loadedImage = true;

		img->setTemporaryPath(m_imagePath, res.size);

		updateWindowTitle();
		pendingUpdate();
		draw();
	}
}

void ViewerWindow::showLoadingError(const QString &message)
{
	log(message);
	ui->labelLoadingError->setText(message);
	ui->labelLoadingError->show();
}

void ViewerWindow::pendingUpdate()
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

void ViewerWindow::draw()
{
	// Videos don't get drawn
	if (m_image->isVideo() && !m_settings->value("Viewer/useVideoPlayer", true).toBool()) {
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
void ViewerWindow::update(bool onlySize, bool force)
{
	// Only used for images
	if (m_displayImage.isNull()) {
		return;
	}

	const bool needScaling = m_settings->value("Viewer/scaleUp", false).toBool()
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

Qt::Alignment ViewerWindow::getAlignments(const QString &type)
{
	const QString vertical = m_settings->value(type + "V", "center").toString();
	const QString horizontal = m_settings->value(type + "H", "left").toString();

	const Qt::Alignment vAlign = vertical == "top" ? Qt::AlignTop : (vertical == "bottom" ? Qt::AlignBottom : Qt::AlignVCenter);
	const Qt::Alignment hAlign = horizontal == "left" ? Qt::AlignLeft : (horizontal == "right" ? Qt::AlignRight : Qt::AlignHCenter);

	return vAlign | hAlign;
}

void ViewerWindow::saveNQuit(bool fav)
{
	if (!m_source.isEmpty()) {
		close();
		return;
	}

	setButtonState(fav, SaveButtonState::Saving);
	m_pendingAction = fav ? PendingSaveFav : PendingSave;
	m_pendingClose = true;
	pendingUpdate();
}

void ViewerWindow::saveImage(bool fav)
{
	const SaveButtonState state = fav ? m_saveButtonStateFav : m_saveButtonState;
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
void ViewerWindow::saveImageNow()
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
	connect(downloader, &ImageDownloader::saved, this, &ViewerWindow::saveImageNowSaved);
	connect(downloader, &ImageDownloader::saved, downloader, &ImageDownloader::deleteLater);
	downloader->save();
}
void ViewerWindow::saveImageNowSaved(QSharedPointer<Image> img, const QList<ImageSaveResult> &result)
{
	Q_UNUSED(img)

	const bool fav = m_pendingAction == PendingSaveFav;

	for (const ImageSaveResult &res : result) {
		m_source = res.path;

		switch (res.result)
		{
			case Image::SaveResult::Saved:
				setButtonState(fav, SaveButtonState::Saved);
				if (img == m_image) {
					m_imagePath = res.path;
				}
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

void ViewerWindow::saveImageAs()
{
	const Filename format(m_settings->value("Save/filename").toString());
	const QStringList filenames = format.path(*m_image, m_profile);
	const QString filename = filenames.first().section(QDir::separator(), -1);
	const QString lastDir = m_settings->value("Viewer/lastDir", "").toString();

	QString path = QFileDialog::getSaveFileName(this, tr("Save image"), QDir::toNativeSeparators(lastDir + "/" + filename), "Images (*.png *.gif *.jpg *.jpeg)");
	if (!path.isEmpty()) {
		path = QDir::toNativeSeparators(path);
		m_settings->setValue("Viewer/lastDir", path.section(QDir::separator(), 0, -2));

		m_saveAsPending = path;
		m_pendingAction = PendingSaveAs;
		pendingUpdate();
	}
}


void ViewerWindow::toggleFullScreen()
{
	if (m_isFullscreen) {
		unfullScreen();
	} else {
		fullScreen();
	}
}

void ViewerWindow::fullScreen()
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

	m_settings->beginGroup("Viewer/Shortcuts"); // Could probably just use the variables already initialised when this ViewerWindow was constructed.
		auto *quit = new QShortcut(getKeySequence(m_settings, "keyQuit", Qt::Key_Escape), m_fullScreen);
			connect(quit, &QShortcut::activated, this, &ViewerWindow::unfullScreen);
		auto *toggleFullscreen = new QShortcut(getKeySequence(m_settings, "keyToggleFullscreen", QKeySequence::FullScreen, Qt::Key_F11), m_fullScreen);
			connect(toggleFullscreen, &QShortcut::activated, this, &ViewerWindow::unfullScreen);
		auto *prev = new QShortcut(getKeySequence(m_settings, "keyPrev", Qt::Key_Left), m_fullScreen);
			connect(prev, &QShortcut::activated, this, &ViewerWindow::previous);
		auto *next = new QShortcut(getKeySequence(m_settings, "keyNext", Qt::Key_Right), m_fullScreen);
			connect(next, &QShortcut::activated, this, &ViewerWindow::next);
		auto *toggleSlideshow = new QShortcut(getKeySequence(m_settings, "keyToggleSlideshow", Qt::Key_Space), m_fullScreen);
			connect(toggleSlideshow, &QShortcut::activated, this, &ViewerWindow::toggleSlideshow);
		auto *save = new QShortcut(getKeySequence(m_settings, "keySave", QKeySequence::Save, Qt::CTRL | Qt::Key_S), m_fullScreen);
			connect(save, SIGNAL(activated()), this, SLOT(saveImage()));
		auto *saveFav = new QShortcut(getKeySequence(m_settings, "keySaveFav", Qt::CTRL | Qt::ALT | Qt::Key_S), m_fullScreen);
			connect(saveFav, &QShortcut::activated, this, [this]{saveImage(true);});
	m_settings->endGroup();

	m_fullScreen->setFocus();
}

void ViewerWindow::unfullScreen()
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

void ViewerWindow::prepareNextSlide()
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

void ViewerWindow::toggleSlideshow()
{
	m_isSlideshowRunning = !m_isSlideshowRunning;

	if (!m_isSlideshowRunning) {
		m_slideshow.stop();
	} else {
		prepareNextSlide();
	}
}

void ViewerWindow::resizeEvent(QResizeEvent *e)
{
	if (!m_resizeTimer.isActive()) {
		m_timeout = qMin(500, qMax(50, (m_displayImage.width() * m_displayImage.height()) / 100000));
	}
	m_resizeTimer.stop();
	m_resizeTimer.start(m_timeout);
	update(true);

	QWidget::resizeEvent(e);
}

void ViewerWindow::closeEvent(QCloseEvent *e)
{
	m_settings->setValue("Viewer/geometry", saveGeometry());
	m_settings->setValue("Viewer/plus", m_drawerButtons.empty() ? false : ui->buttonPlus->isChecked());
	m_settings->sync();

	m_image->abortTags();

	for (auto it = m_imageDownloaders.constBegin(); it != m_imageDownloaders.constEnd(); ++it) {
		it.value()->abort();
		it.value()->deleteLater();
	}

	e->accept();
}

void ViewerWindow::showEvent(QShowEvent *e)
{
	Q_UNUSED(e)

	showThumbnail();
}

void ViewerWindow::showThumbnail()
{
	QSize size = m_image->size();
	if (size.isEmpty()) {
		size = m_image->previewImage().size() * 2 * m_settings->value("thumbnailUpscale", 1.0).toDouble();
	}

	// Videos get a static resizable overlay
	if (m_image->isVideo() && !m_settings->value("Viewer/useVideoPlayer", true).toBool()) {
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


void ViewerWindow::reuse(const QList<QSharedPointer<Image>> &images, const QSharedPointer<Image> &image, Site *site)
{
	m_images = images;
	m_site = site;

	load(image);
}

void ViewerWindow::load(const QSharedPointer<Image> &image)
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
	// Is this still necessary?
	setButtonState(false, SaveButtonState::Save);
	setButtonState(true, SaveButtonState::Save);

	// Window title
	updateWindowTitle();

	prepareNextSlide();
	go();
}

void ViewerWindow::updateWindowTitle()
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
	QString title = m_image->isVideo() ? tr("Video") : (!m_isAnimated.isEmpty() ? tr("Animation") : tr("Image"));
	if (!infos.isEmpty()) {
		title += QString(" (%1)").arg(infos.join(", "));
	}
	setWindowTitle(QStringLiteral("%1 - %2 (%3/%4)").arg(title, m_image->parentSite()->name(), QString::number(m_images.indexOf(m_image) + 1), QString::number(m_images.count())));
}

int ViewerWindow::firstNonBlacklisted(int direction)
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

void ViewerWindow::next()
{
	m_image->abortTags();

	const int index = firstNonBlacklisted(+1);
	load(m_images[index]);
}

void ViewerWindow::previous()
{
	m_image->abortTags();

	const int index = firstNonBlacklisted(-1);
	load(m_images[index]);
}

void ViewerWindow::updateButtonPlus()
{
	const bool isOpen = ui->buttonPlus->isChecked();
	ui->buttonPlus->setText(QChar(isOpen ? '-' : '+'));
	for (auto button : m_drawerButtons) {
		button->setVisible(isOpen);
	}
}

void ViewerWindow::openFile(bool now)
{
	if (!now) {
		m_pendingAction = PendingOpen;
		pendingUpdate();
		return;
	}

	QDesktopServices::openUrl(QUrl::fromLocalFile(m_imagePath));
	m_pendingAction = PendingNothing;
}

void ViewerWindow::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() == Qt::MiddleButton && m_settings->value("imageCloseMiddleClick", true).toBool()) {
		close();
		return;
	}

	QWidget::mouseReleaseEvent(e);
}

void ViewerWindow::wheelEvent(QWheelEvent *e)
{
	if (m_settings->value("imageNavigateScroll", true).toBool()) {
		// Ignore events triggered when reaching the bottom of the tag list
		if (ui->scrollArea->underMouse()) {
			return;
		}

		// Ignore events if we already got one less than 500ms ago
		if (m_lastWheelEvent.isValid() && m_lastWheelEvent.elapsed() <= 500) {
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

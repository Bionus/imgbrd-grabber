#include <QApplication>
#include <QtNetwork>
#include <QMessageBox>
#include <QShortcut>
#include <QMenu>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include "functions.h"
#include "settings/optionswindow.h"
#include "ui/QAffiche.h"
#include "zoomwindow.h"
#include "imagethread.h"
#include "ui_zoomwindow.h"
#include "models/filename.h"

#include <QMediaPlaylist>



zoomWindow::zoomWindow(Image *image, Site *site, QMap<QString,Site*> *sites, mainWindow *parent)
	: QDialog(0, Qt::Window), m_parent(parent), m_settings(parent->settings()), ui(new Ui::zoomWindow), m_site(site), timeout(300), m_loaded(false), m_loadedImage(false), m_loadedDetails(false), oldsize(0), image(NULL), movie(NULL), m_program(qApp->arguments().at(0)), m_reply(NULL), m_finished(false), m_thread(false), m_data(QByteArray()), m_size(0), m_sites(sites), m_source(), m_th(NULL), m_fullScreen(NULL)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);

	QList<Favorite> favorites = loadFavorites();
	for (Favorite fav : favorites)
		m_favorites.append(fav.getName());
	m_viewItLater = loadViewItLater();
	m_ignore = loadIgnored();
	m_image = new Image(site, image->details(), image->page());
	connect(m_image, &Image::urlChanged, this, &zoomWindow::urlChanged);

	m_mustSave = 0;

	restoreGeometry(m_settings->value("Zoom/geometry").toByteArray());
	ui->buttonPlus->setChecked(m_settings->value("Zoom/plus", false).toBool());

	QShortcut *escape = new QShortcut(QKeySequence(Qt::Key_Escape), this);
		connect(escape, SIGNAL(activated()), this, SLOT(close()));
	QShortcut *save = new QShortcut(QKeySequence::Save, this);
		connect(save, SIGNAL(activated()), this, SLOT(saveImage()));
	QShortcut *saveAs = new QShortcut(QKeySequence("Ctrl+Shift+S"), this);
		connect(saveAs, SIGNAL(activated()), this, SLOT(saveImageAs()));

	m_labelTagsLeft = new QAffiche(QVariant(), 0, QColor(), this);
		m_labelTagsLeft->setContextMenuPolicy(Qt::CustomContextMenu);
		m_labelTagsLeft->setTextInteractionFlags(Qt::TextBrowserInteraction);
		connect(m_labelTagsLeft, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
		connect(m_labelTagsLeft, SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));
		connect(m_labelTagsLeft, SIGNAL(linkHovered(QString)), this, SLOT(linkHovered(QString)));
		connect(m_labelTagsLeft, SIGNAL(middleClicked()), this, SLOT(openInNewTab()));
		ui->scrollAreaWidgetContents->layout()->addWidget(m_labelTagsLeft);

	m_labelTagsTop = new QAffiche(QVariant(), 0, QColor(), this);
		m_labelTagsTop->setWordWrap(true);
		m_labelTagsTop->setContextMenuPolicy(Qt::CustomContextMenu);
		m_labelTagsTop->setTextInteractionFlags(Qt::TextBrowserInteraction);
		connect(m_labelTagsTop, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
		connect(m_labelTagsTop, SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));
		connect(m_labelTagsTop, SIGNAL(linkHovered(QString)), this, SLOT(linkHovered(QString)));
		connect(m_labelTagsTop, SIGNAL(middleClicked()), this, SLOT(openInNewTab()));
		ui->verticalLayout->insertWidget(0, m_labelTagsTop, 0);

	m_imageTime = NULL;
	go();
}
void zoomWindow::go()
{
	ui->labelPools->hide();
	bool whitelisted = false;
	if (!m_settings->value("whitelistedtags").toString().isEmpty())
	{
		QStringList whitelist = m_settings->value("whitelistedtags").toString().split(" ");
		for (Tag t : m_image->tags())
		{
			if (whitelist.contains(t.text()))
			{
				whitelisted = true;
				break;
			}
		}
	}
	if (m_settings->value("autodownload", false).toBool() || (whitelisted && m_settings->value("whitelist_download", "image").toString() == "image"))
	{ saveImage(); }

	m_stackedWidget = new QStackedWidget(this);
		ui->verticalLayout->insertWidget(1, m_stackedWidget, 1);
	m_labelImage = new QAffiche(QVariant(), 0, QColor(), this);
		m_labelImage->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored));
		connect(m_labelImage, SIGNAL(doubleClicked()), this, SLOT(fullScreen()));
		m_stackedWidget->addWidget(m_labelImage);
	m_mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
		m_videoWidget = new QVideoWidget();
		m_stackedWidget->addWidget(m_videoWidget);
		m_mediaPlayer->setVideoOutput(m_videoWidget);

	QMap<QString, QString> assoc;
		assoc["s"] = tr("Safe");
		assoc["q"] = tr("Questionable");
		assoc["e"] = tr("Explicit");
	QString ext = getExtension(m_image->url());
	m_url = (!m_settings->value("Save/downloadoriginals", true).toBool() && !m_image->sampleUrl().isEmpty()) || ext == "zip"
			? m_image->sampleUrl().toString()
			: m_image->fileUrl().toString();

	m_format = m_url.section('.', -1).toUpper().toLatin1().data();

	QTimer *timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(update()));
		timer->setSingleShot(true);
		m_resizeTimer = timer;

	QString u = m_site->value("Urls/Html/Post");
		u.replace("{id}", QString::number(m_image->id()));

	connect(ui->buttonDetails, SIGNAL(clicked()), this, SLOT(showDetails()));

	QString pos = m_settings->value("tagsposition", "top").toString();
	if (pos == "auto")
	{
		if (!m_image->size().isEmpty())
		{
			if (float(m_image->width())/float(m_image->height()) >= 4./3.)
			{ pos = "top"; }
			else
			{ pos = "left"; }
		}
		else
		{ pos = "top"; }
	}
	if (pos == "top")
	{
		ui->widgetLeft->hide();
		m_labelTagsTop->setText(m_image->stylishedTags(m_ignore).join(" "));
	}
	else
	{
		m_labelTagsTop->hide();
		m_labelTagsLeft->setText(m_image->stylishedTags(m_ignore).join("<br/>"));
	}

	m_detailsWindow = new detailsWindow(m_image, this);

	// Load image details (exact tags & co)
	connect(m_image, SIGNAL(finishedLoadingTags(Image*)), this, SLOT(replyFinishedDetails(Image*)));
	m_image->loadDetails();

	activateWindow();
}

/**
 * Destructor of the zoomWindow class
 */
zoomWindow::~zoomWindow()
{
	/*if (m_imageTime != NULL)
		delete m_imageTime;*/
	if (image != nullptr)
		delete image;
	if (movie != NULL)
		movie->deleteLater();

	m_labelTagsTop->deleteLater();
	m_labelTagsLeft->deleteLater();
	m_image->deleteLater();

	delete ui;
}

void zoomWindow::showDetails()
{
	m_detailsWindow->setImage(m_image);
	m_detailsWindow->show();
}

void zoomWindow::openUrl(QString url)
{ emit linkClicked(url); }
void zoomWindow::openPool(QString url)
{
	if (url.startsWith("pool:"))
	{ emit poolClicked(url.right(url.length() - 5).toInt(), m_image->site()); }
	else
	{
		Page *p = new Page(m_sites->value(m_image->site()), m_sites->values(), QStringList() << "id:"+url, 1, 1, QStringList(), false, this);
		connect(p, SIGNAL(finishedLoading(Page*)), this, SLOT(openPoolId(Page*)));
		p->load();
	}
}
void zoomWindow::openPoolId(Page *p)
{
	if (p->images().size() < 1)
	{ return; }
	m_image = p->images().at(0);
	timeout = 300;
	m_loaded = 0;
	oldsize = 0;
	m_reply->deleteLater();
	m_reply = NULL;
	m_finished = false;
	m_size = 0;
	m_labelImage->hide();
	ui->verticalLayout->removeWidget(m_labelImage);
	go();
}

void zoomWindow::openSaveDir(bool fav)
{
	QString path = m_settings->value("Save/path"+QString(fav ? "_favorites" : "")).toString().replace("\\", "/"), fn = m_settings->value("Save/filename"+QString(fav ? "_favorites" : "")).toString();

	if (path.right(1) == "/")
	{ path = path.left(path.length()-1); }
	path = QDir::toNativeSeparators(path);

	QStringList files = m_image->path(fn, path);
	QString file = files.empty() ? "" : files.at(0);
	QString pth = file.section(QDir::toNativeSeparators("/"), 0, -2);
	QString url = path+QDir::toNativeSeparators("/")+pth;

	QDir dir(url);
	if (dir.exists())
	{ showInGraphicalShell(url); }
	else
	{
		int reply = QMessageBox::question(this, tr("Dossier inexistant"), tr("Le dossier de sauvegarde n'existe pas encore. Le creer ?"), QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			QDir dir(path);
			if (!dir.mkpath(pth))
			{ error(this, tr("Erreur lors de la création du dossier.\r\n%1").arg(url)); }
			showInGraphicalShell(url);
		}
	}
}
void zoomWindow::openSaveDirFav()
{ openSaveDir(true); }

void zoomWindow::linkHovered(QString url)
{ this->link = url; }
void zoomWindow::contextMenu(QPoint)
{
	QMenu *menu = new QMenu(this);
	if (!this->link.isEmpty())
	{
		// Favoris
		if (m_favorites.contains(link, Qt::CaseInsensitive))
		{
			menu->addAction(QIcon(":/images/icons/remove.png"), tr("Retirer des favoris"), this, SLOT(unfavorite()));
			menu->addAction(QIcon(":/images/icons/save.png"), tr("Choisir comme image"), this, SLOT(setfavorite()));
		}
		else
		{ menu->addAction(QIcon(":/images/icons/add.png"), tr("Ajouter aux favoris"), this, SLOT(favorite())); }
		// Garder pour plus tard
		if (m_viewItLater.contains(link, Qt::CaseInsensitive))
		{ menu->addAction(QIcon(":/images/icons/remove.png"), tr("Ne pas garder pour plus tard"), this, SLOT(unviewitlater())); }
		else
		{ menu->addAction(QIcon(":/images/icons/add.png"), tr("Garder pour plus tard"), this, SLOT(viewitlater())); }
		// Ignorer
		if (m_ignore.contains(link, Qt::CaseInsensitive))
		{ menu->addAction(QIcon(":/images/icons/showed.png"), tr("Ne plus ignorer"), this, SLOT(unignore())); }
		else
		{ menu->addAction(QIcon(":/images/icons/hidden.png"), tr("Ignorer"), this, SLOT(ignore())); }
		menu->addSeparator();
		// Onglets
		menu->addAction(QIcon(":/images/icons/tab-plus.png"), tr("Ouvrir dans un nouvel onglet"), this, SLOT(openInNewTab()));
		menu->addAction(QIcon(":/images/icons/window.png"), tr("Ouvrir dans une nouvelle fenêtre"), this, SLOT(openInNewWindow()));
		menu->addAction(QIcon(":/images/icons/browser.png"), tr("Ouvrir dans le navigateur"), this, SLOT(openInBrowser()));
	}
	menu->exec(QCursor::pos());
}
void zoomWindow::openInNewTab()
{ m_parent->addTab(link); }
void zoomWindow::openInNewWindow()
{
	QProcess myProcess;
	myProcess.startDetached(m_program, QStringList(link));
}
void zoomWindow::openInBrowser()
{
	Page *p = new Page(m_site, m_sites->values(), QStringList() << link, 1);
	QDesktopServices::openUrl(p->url());
	p->deleteLater();
}
void zoomWindow::favorite()
{
	m_favorites.append(link);

	QFile f(savePath("favorites.txt"));
		f.open(QIODevice::WriteOnly | QIODevice::Append);
		f.write(QString(link+"|50|"+QDateTime::currentDateTime().toString(Qt::ISODate)+"\r\n").toUtf8());
	f.close();

	setfavorite();
}
void zoomWindow::setfavorite()
{
	if (!QDir(savePath("thumbs")).exists())
	{ QDir(savePath()).mkdir("thumbs"); }

	if (image != nullptr)
	{
		Favorite fav(link, 50, QDateTime::currentDateTime());
		fav.setImage(*image);
	}

	m_parent->updateFavorites();
	m_parent->updateFavoritesDock();
}
void zoomWindow::unfavorite()
{
	m_favorites.removeAll(link);

	QFile f(savePath("favorites.txt"));
	f.open(QIODevice::ReadOnly);
		QString favs = f.readAll();
	f.close();

	favs.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
	QRegExp reg(QRegExp::escape(link) + "\\|(.+)\\r\\n");
	reg.setMinimal(true);
	favs.remove(reg);

	f.open(QIODevice::WriteOnly);
		f.write(favs.toUtf8());
	f.close();

	if (QFile::exists(savePath("thumbs/"+link+".png")))
	{ QFile::remove(savePath("thumbs/"+link+".png")); }

	m_parent->updateFavorites();
	m_parent->updateFavoritesDock();
}
void zoomWindow::viewitlater()
{
	m_viewItLater.append(link);
	QFile f(savePath("viewitlater.txt"));
	f.open(QIODevice::WriteOnly);
		f.write(m_viewItLater.join("\r\n").toUtf8());
	f.close();

	m_parent->updateKeepForLater();
}
void zoomWindow::unviewitlater()
{
	m_viewItLater.removeAll(link);
	QFile f(savePath("viewitlater.txt"));
	f.open(QIODevice::WriteOnly);
		f.write(m_viewItLater.join("\r\n").toUtf8());
	f.close();

	m_parent->updateKeepForLater();
}
void zoomWindow::ignore()
{
	m_ignore.append(link);
	QFile f(savePath("ignore.txt"));
	f.open(QIODevice::WriteOnly);
		f.write(m_ignore.join("\r\n").toUtf8());
	f.close();
	colore();
}
void zoomWindow::unignore()
{
	m_ignore.removeAll(link);
	QFile f(savePath("ignore.txt"));
	f.open(QIODevice::WriteOnly);
		f.write(m_ignore.join("\r\n").toUtf8());
	f.close();
	colore();
}

void zoomWindow::load()
{
	log(tr("Chargement de l'image depuis <a href=\"%1\">%1</a>").arg(m_url));
	m_data.clear();

	m_imageTime = new QTime();
	m_imageTime->start();

	m_reply = m_site->get(m_url, NULL, "image", m_image);
	connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
	connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinishedZoom()));
}
void zoomWindow::sslErrorHandler(QNetworkReply* qnr, QList<QSslError>)
{ qnr->ignoreSslErrors(); }

#define PERCENT 0.05f
#define TIME 500
void zoomWindow::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
	if (m_imageTime->elapsed() > TIME || (bytesTotal > 0 && bytesReceived / bytesTotal > PERCENT))
	{
		m_imageTime->restart();
		m_data.append(m_reply->readAll());
		m_thread = true;
		m_th = new ImageThread(m_data);
		connect(m_th, SIGNAL(finished(QImage, int)), this, SLOT(display(QImage, int)));
		connect(m_th, SIGNAL(finished()), m_th, SLOT(deleteLater()));
		m_th->start();
	}
}
void zoomWindow::display(QImage pix, int size)
{
	if (!pix.size().isEmpty() && size >= m_size)
	{
		m_size = size;
		delete image;
		image = new QPixmap(QPixmap::fromImage(pix));
		if (m_url.section('.', -1).toLower() == "gif")
		{ /*m_labelImage->setPixmap(*image);*/ }
		else
		{ update(!m_finished); }
		m_thread = false;

		if (m_fullScreen != NULL && m_fullScreen->isVisible())
		{ m_fullScreen->setImage(image->scaled(QApplication::desktop()->screenGeometry().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); }
	}
}

void zoomWindow::replyFinishedDetails(Image* img)
{
	log("details got");

	m_loadedDetails = true;
	m_image = img;
	colore();

	QStringList pools = QStringList();
	for (const Pool &p : img->pools())
	{ pools.append((p.previous() != 0 ? "<a href=\""+QString::number(p.previous())+"\">&lt;</a> " : "")+"<a href=\"pool:"+QString::number(p.id())+"\">"+p.name()+"</a>"+(p.next() != 0 ? " <a href=\""+QString::number(p.next())+"\">&gt;</a>" : "")); }
	if (!pools.isEmpty())
	{ ui->labelPools->show(); }
	ui->labelPools->setText(pools.join("<br />"));

	QString path1 = m_settings->value("Save/path").toString().replace("\\", "/");
	QStringList pth1s = m_image->path(m_settings->value("Save/filename").toString(), path1);
	QString source1;
	bool file1notexists = false;
	for (QString pth1 : pth1s)
	{
		if (path1.right(1) == "/")
		{ path1 = path1.left(path1.length()-1); }
		QFile file(path1+"/"+pth1);
		if (file.exists())
			source1 = file.fileName();
		else
			file1notexists = true;
	}

	QString path2 = m_settings->value("Save/path_favorites").toString().replace("\\", "/");
	QStringList pth2s = m_image->path(m_settings->value("Save/filename_favorites").toString(), path2);
	QString source2;
	bool file2notexists = false;
	for (QString pth2 : pth2s)
	{
		if (path2.right(1) == "/")
		{ path2 = path1.left(path2.length()-1); }
		QFile file(path2+"/"+pth2);
		if (file.exists())
			source2 = file.fileName();
		else
			file2notexists = true;
	}

	if (!file1notexists || !file2notexists)
	{
		if (!file1notexists)
		{
			ui->buttonSave->setText(tr("Fichier déjà existant"));
			ui->buttonSaveNQuit->setText(tr("Fermer"));
		}
		if (!file2notexists)
		{
			ui->buttonSaveFav->setText(tr("Fichier déjà existant (fav)"));
			ui->buttonSaveNQuitFav->setText(tr("Fermer (fav)"));
		}
		m_source = !file1notexists ? source1 : source2;
		log(tr("Image chargée depuis le fichier <a href=\"file:///%1\">%1</a>").arg(m_source));

		// Fix extension when it should be guessed
		QString fext = m_source.section('.', -1);
		m_url = m_url.section('.', 0, -2) + "." + fext;
		m_image->setFileExtension(fext);

		draw();
	}
	else
	{
		if (m_url.isEmpty())
		{ m_url = m_image->url(); }
		load();
		pendingUpdate();
	}
}
void zoomWindow::colore()
{
	QStringList t = m_image->stylishedTags(m_ignore);
	tags = t.join(" ");
	if (ui->widgetLeft->isHidden())
	{ m_labelTagsTop->setText(tags); }
	else
	{ m_labelTagsLeft->setText(t.join("<br/>")); }
	m_detailsWindow->setTags(tags);
}

void zoomWindow::replyFinishedZoom()
{
	log("zoom got");
	delete m_imageTime;

	// Check redirection
	QUrl redir = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
	if (!redir.isEmpty())
	{
		m_data.clear();
		m_url = redir.toString();
		m_image->setUrl(m_url);
		load();
		return;
	}

	log(tr("Image reçue depuis <a href=\"%1\">%1</a>").arg(m_url));
	m_finished = true;
	if (m_reply->error() == QNetworkReply::NoError)
	{
		m_data.append(m_reply->readAll());
		m_image->setData(m_data);

		draw();
		m_loadedImage = true;
		pendingUpdate();
	}
	else if (m_reply->error() == QNetworkReply::ContentNotFoundError && m_url.section('.', -1) != "mp4")
	{
		QString ext = m_url.section('.', -1);
		QMap<QString,QString> nextext;
		nextext["jpg"] = "png";
		nextext["png"] = "gif";
		nextext["gif"] = "jpeg";
		nextext["jpeg"] = "swf";
		nextext["swf"] = "webm";
		nextext["webm"] = "mp4";
		m_url = m_url.section('.', 0, -2)+"."+nextext[ext];
		m_image->setFileExtension(nextext[ext]);
		log(tr("Image non trouvée. Nouvel essai avec l'extension %1...").arg(nextext[ext]));
		load();
		return;
	}
	else if (m_reply->error() != QNetworkReply::OperationCanceledError)
	{ error(this, tr("Une erreur inattendue est survenue lors du chargement de l'image (%1).\r\n%2").arg(m_reply->error()).arg(m_reply->url().toString())); }

	m_reply->deleteLater();
	m_reply = NULL;
}

void zoomWindow::pendingUpdate()
{
	// If we don't want to save, nothing to do
	if (m_mustSave == 0)
		return;

	// If the image is not even loaded, we cannot save it
	if (!m_loadedImage)
		return;

	bool fav = (m_mustSave == 3 || m_mustSave == 4);
	Filename fn(m_settings->value("Save/path" + QString(fav ? "_favorites" : "")).toString());

	// If the image is loaded but we need their tags and we don't have them, we wait
	if (!m_loadedDetails && fn.needExactTags(m_site))
		return;

	m_loaded = true;

	switch (m_mustSave)
	{
		case 1:
			saveImageNow();
			break;

		case 2:
			if (!saveImageNow().isEmpty())
				close();
			break;

		case 3:
			saveImageNow(true);
			break;

		case 4:
			if (!saveImageNow(true).isEmpty())
				close();
			break;
	}
}

void zoomWindow::draw()
{
	QString ext = m_url.section('.', -1).toLower();
	bool isVideo = ext == "mp4" || ext == "webm" || ext == "flv";

	QString fn = m_url.section('/', -1).toLower();

	QString filename;
	if (m_source.isEmpty())
	{
		if (ext == "gif" || isVideo)
		{
			filename = QDir::temp().absoluteFilePath("grabber-" + fn);
			QFile f(filename);
			if (f.open(QFile::WriteOnly))
			{
				f.write(m_data);
				f.close();
			}
		}
	}
	else
	{ filename = m_source; }

	if (ext == "gif")
	{
		this->movie = new QMovie(filename, QByteArray(), this);
		m_labelImage->setMovie(this->movie);
		m_stackedWidget->setCurrentWidget(m_labelImage);
		this->movie->start();

		image = NULL;
	}
	else if (isVideo)
	{
		QMediaPlaylist *playlist = new QMediaPlaylist(this);
		playlist->addMedia(QUrl::fromLocalFile(filename));
		playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
		m_mediaPlayer->setPlaylist(playlist);
		m_stackedWidget->setCurrentWidget(m_videoWidget);
		m_mediaPlayer->play();
	}
	else
	{
		if (!m_source.isEmpty())
		{
			QPixmap *img = new QPixmap;
			img->load(m_source);
			this->image = img;

			m_loadedImage = true;
			update();
			pendingUpdate();
		}
		else
		{
			m_thread = true;
			m_th = new ImageThread(m_data);
			connect(m_th, SIGNAL(finished(QImage, int)), this, SLOT(display(QImage, int)));
			connect(m_th, SIGNAL(finished()), m_th, SLOT(deleteLater()));
			m_th->start();
		}
	}
}



/**
 * Updates the image label to use the current image.
 * @param onlysize true to update the image quickly
 */
void zoomWindow::update(bool onlysize)
{
	QString ext = m_url.section('.', -1).toLower();
	if (ext != "gif" && ext != "webm")
	{
		if (onlysize && (this->image->width() > m_labelImage->width() || this->image->height() > m_labelImage->height()))
		{ m_labelImage->setImage(this->image->scaled(m_labelImage->width(), m_labelImage->height(), Qt::KeepAspectRatio, Qt::FastTransformation)); }
		else if (m_loaded)
		{
			if (this->image->width() > m_labelImage->width() || this->image->height() > m_labelImage->height())
			{ m_labelImage->setImage(this->image->scaled(m_labelImage->width(), m_labelImage->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); }
			else
			{ m_labelImage->setImage(*this->image); }
		}
		m_stackedWidget->setCurrentWidget(m_labelImage);
	}
}

void zoomWindow::saveNQuit()
{
	ui->buttonSaveNQuit->setText(tr("Sauvegarde..."));
	m_mustSave = 2;
	pendingUpdate();
}
void zoomWindow::saveNQuitFav()
{
	ui->buttonSaveNQuitFav->setText(tr("Sauvegarde..."));
	m_mustSave = 4;
	pendingUpdate();
}

void zoomWindow::saveImage(bool fav)
{
	if (fav)
	{
		ui->buttonSaveFav->setText(tr("Sauvegarde... (fav)"));
		m_mustSave = 3;
	}
	else
	{
		ui->buttonSave->setText(tr("Sauvegarde..."));
		m_mustSave = 1;
	}
	pendingUpdate();
}
void zoomWindow::saveImageFav()
{ saveImage(true); }
QStringList zoomWindow::saveImageNow(bool fav)
{
	QString pth = m_settings->value("Save/path"+QString(fav ? "_favorites" : "")).toString().replace("\\", "/");
	if (pth.right(1) == "/")
	{ pth = pth.left(pth.length()-1); }

	if (pth.isEmpty() || m_settings->value("Save/filename").toString().isEmpty())
	{
		int reply;
		if (pth.isEmpty())
		{ reply = QMessageBox::question(this, tr("Erreur"), tr("Vous n'avez pas précisé de dossier de sauvegarde ! Voulez-vous ouvrir les options ?"), QMessageBox::Yes | QMessageBox::No); }
		else
		{ reply = QMessageBox::question(this, tr("Erreur"), tr("Vous n'avez pas précisé de format de sauvegarde ! Voulez-vous ouvrir les options ?"), QMessageBox::Yes | QMessageBox::No); }
		if (reply == QMessageBox::Yes)
		{
			optionsWindow *options = new optionsWindow(m_parent);
			//options->onglets->setCurrentIndex(3);
			options->setWindowModality(Qt::ApplicationModal);
			options->show();
			connect(options, SIGNAL(closed()), this, SLOT(saveImage()));
		}
		return QStringList();
	}

	QStringList paths;
	QMap<QString, Image::SaveResult> results = m_image->save(m_settings->value("Save/filename"+QString(fav ? "_favorites" : "")).toString(), pth);
	auto it = results.begin();
	while (it != results.end())
	{
		Image::SaveResult res = it.value();
		paths.append(it.key());

		QPushButton *button = fav ? ui->buttonSaveFav : ui->buttonSave;
		QPushButton *saveQuit = fav ? ui->buttonSaveNQuitFav : ui->buttonSaveNQuit;
		switch (res)
		{
			case Image::SaveResult::Error:
				error(this, tr("Erreur lors de la sauvegarde de l'image."));
				return QStringList();
				break;

			case Image::SaveResult::Saved:
				button->setText(fav ? tr("Sauvegardé ! (fav)") : tr("Sauvegardé !"));
				break;

			case Image::SaveResult::Copied:
				button->setText(fav ? tr("Copié ! (fav)") : tr("Copié !"));
				break;

			case Image::SaveResult::Moved:
				button->setText(fav ? tr("Déplacé ! (fav)") : tr("Déplacé !"));
				break;

			case Image::SaveResult::Ignored:
				button->setText(fav ? tr("Ignoré ! (fav)") : tr("Ignoré !"));
				break;

			case Image::SaveResult::AlreadyExists:
				button->setText(fav ? tr("Fichier déjà existant (fav)") : tr("Fichier déjà existant"));
				break;
		}
		saveQuit->setText(fav ? tr("Fermer (fav)") : tr("Fermer"));

		++it;
	}

	if (m_mustSave == 2 || m_mustSave == 4)
		close();

	m_mustSave = 0;
	return paths;
}

QString zoomWindow::saveImageAs()
{
	QString filename = m_image->fileUrl().toString().section('/', -1);
	QString lastDir = m_settings->value("Zoom/lastDir", "").toString();

	QString path = QFileDialog::getSaveFileName(this, tr("Enregistrer l'image"), QDir::toNativeSeparators(lastDir + "/" + filename), "Images (*.png *.gif *.jpg *.jpeg)");
	if (!path.isEmpty())
	{
		path = QDir::toNativeSeparators(path);
		m_settings->setValue("Zoom/lastDir", path.section(QDir::toNativeSeparators("/"), 0, -2));

		m_image->save(path, true, true);
	}

	return path;
}



void zoomWindow::fullScreen()
{
	if (image == NULL)
		return;

	QString ext = m_url.section('.', -1).toLower();
	bool isVideo = ext == "mp4" || ext == "webm" || ext == "flv";

	if (isVideo)
	{
		m_videoWidget->setFullScreen(true);
	}
	else
	{
		m_fullScreen = new QAffiche(QVariant(), 0, QColor());
		m_fullScreen->setStyleSheet("background-color: black");
		m_fullScreen->setAlignment(Qt::AlignCenter);
		if (ext == "gif")
		{ m_fullScreen->setMovie(movie); }
		else
		{ m_fullScreen->setImage(image->scaled(QApplication::desktop()->screenGeometry().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); }
		m_fullScreen->showFullScreen();

		connect(m_fullScreen, SIGNAL(doubleClicked()), m_fullScreen, SLOT(close()));
	}

	QShortcut *escape = new QShortcut(QKeySequence(Qt::Key_Escape), this);
	connect(escape, SIGNAL(activated()), this, SLOT(unfullScreen()));
}

void zoomWindow::unfullScreen()
{
	QString ext = m_url.section('.', -1).toLower();
	bool isVideo = ext == "mp4" || ext == "webm" || ext == "flv";

	if (isVideo)
	{
		m_videoWidget->setFullScreen(false);
	}
	else
	{
		m_fullScreen->close();
		m_fullScreen->deleteLater();
		m_fullScreen = nullptr;
	}
}



void zoomWindow::resizeEvent(QResizeEvent *e)
{
	if (m_loaded && m_finished && !m_thread)
	{
		if (!m_resizeTimer->isActive())
		{
			this->timeout = this->image->width()*this->image->height() / 100000;
			if (this->timeout < 50)		{ this->timeout = 50;	}
			if (this->timeout > 500)	{ this->timeout = 500;	}
		}
		m_resizeTimer->stop();
		m_resizeTimer->start(this->timeout);
		this->update(true);
	}
	QWidget::resizeEvent(e);
}

void zoomWindow::closeEvent(QCloseEvent *e)
{
	m_settings->setValue("Zoom/geometry", saveGeometry());
	m_settings->setValue("Zoom/plus", ui->buttonPlus->isChecked());
	m_settings->sync();

	//m_image->abortTags();
	/*if (m_thread && m_th->isRunning())
	{ m_th->quit(); }*/
	if (m_reply != NULL && m_reply->isRunning())
	{
		m_reply->abort();
		log(tr("Chargement de l'image stoppé."));
	}

	e->accept();
}

void zoomWindow::urlChanged(QString old, QString nouv)
{
	Q_UNUSED(old);
	m_url = nouv;
}

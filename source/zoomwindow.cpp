#include <QApplication>
#include <QtNetwork>
#include "functions.h"
#include "optionswindow.h"
#include "QAffiche.h"
#include "zoomwindow.h"
#include "imagethread.h"
#include "ui_zoomwindow.h"

extern mainWindow *_mainwindow;



zoomWindow::zoomWindow(Image *image, QMap<QString,QString> site, QMap<QString,QMap<QString,QString> > *sites, QWidget *parent) : QDialog(0, Qt::Window), ui(new Ui::zoomWindow), m_image(image), m_site(site), timeout(300), loaded(0), oldsize(0), m_labelTags(NULL), image(NULL), movie(NULL), m_program(qApp->arguments().at(0)), m_replyExists(false), m_finished(false), m_thread(false), m_data(QByteArray()), m_size(0), m_sites(sites), m_source(), m_th(NULL)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    m_favorites = loadFavorites().keys();
    m_viewItLater = loadViewItLater();
    m_ignore = loadIgnored();

    m_mustSave = 0;

    QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
    restoreGeometry(settings.value("Zoom/geometry").toByteArray());
    ui->buttonPlus->setChecked(settings.value("Zoom/plus", false).toBool());

	QShortcut *escape = new QShortcut(QKeySequence(Qt::Key_Escape), this);
		connect(escape, SIGNAL(activated()), this, SLOT(close()));
	QShortcut *save = new QShortcut(QKeySequence::Save, this);
		connect(save, SIGNAL(activated()), this, SLOT(saveImage()));
	QShortcut *saveAs = new QShortcut(QKeySequence("Ctrl+Shift+S"), this);
		connect(saveAs, SIGNAL(activated()), this, SLOT(saveImageAs()));

    go();
}
void zoomWindow::go()
{
    ui->labelPools->hide();
    QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
    bool whitelisted = false;
    QList<Tag> taglist = m_image->tags();
    if (!settings.value("whitelistedtags").toString().isEmpty())
    {
        QStringList whitelist = settings.value("whitelistedtags").toString().split(" ");
        for (int i = 0; i < taglist.size(); i++)
        {
            if (whitelist.contains(taglist[i].text()))
            { whitelisted = true; break; }
        }
    }
    if (settings.value("autodownload", false).toBool() || (whitelisted && settings.value("whitelist_download", "image").toString() == "image"))
    { saveImage(); }

    QAffiche *labelImage = new QAffiche(QVariant(), 0, QColor(), this);
        labelImage->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored));
        connect(labelImage, SIGNAL(doubleClicked()), this, SLOT(fullScreen()));
    ui->verticalLayout->insertWidget(1, labelImage, 1);
    this->labelImage = labelImage;

    QStringList hreftags;
    for (int i = 0; i < taglist.count(); i++)
    { hreftags.append("<a href=\""+taglist[i].text()+"\" style=\"text-decoration:none;color:#000000\">"+taglist[i].text()+"</a>"); }

    QMap<QString, QString> assoc;
        assoc["s"] = tr("Safe");
        assoc["q"] = tr("Questionable");
        assoc["e"] = tr("Explicit");
    m_url = settings.value("Save/downloadoriginals", true).toBool() ? m_image->fileUrl().toString() : (m_image->sampleUrl().isEmpty() ? m_image->fileUrl().toString() : m_image->sampleUrl().toString());

    m_format = m_url.section('.', -1).toUpper().toAscii().data();

    QTimer *timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(update()));
        timer->setSingleShot(true);
        this->timer = timer;

    QString u = m_site["Urls/Html/Post"];
        u.replace("{id}", QString::number(m_image->id()));
    m_detailsWindow = new detailsWindow(m_image, this);
    connect(ui->buttonDetails, SIGNAL(clicked()), m_detailsWindow, SLOT(show()));

    QString pos = settings.value("tagsposition", "top").toString();
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
        ui->labelTagsTop->setText(hreftags.join(" "));
    }
    else
    {
        ui->labelTagsTop->hide();
        ui->labelTagsLeft->setText(hreftags.join("<br/>"));
    }

    connect(m_image, SIGNAL(finishedLoadingTags(Image*)), this, SLOT(replyFinished(Image*)));
    m_image->loadDetails();
    activateWindow();
}

/**
 * Destructor of the zoomWindow class
 */
zoomWindow::~zoomWindow()
{
    if (movie != NULL)
    { movie->deleteLater(); }
    if (m_labelTags != NULL)
    { m_labelTags->deleteLater(); }
    if (image != NULL)
    { delete image; }
    delete ui;
}

void zoomWindow::openUrl(QString url)
{ emit linkClicked(url); }
void zoomWindow::openPool(QString url)
{
    if (url.startsWith("pool:"))
    { emit linkClicked(url); }
    else
    {
        Page *p = new Page(m_sites, m_image->site(), QStringList() << "id:"+url, 1, 1, QStringList(), this);
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
    loaded = 0;
    oldsize = 0;
    m_replyExists = false;
    m_finished = false;
    m_size = 0;
    labelImage->hide();
    ui->verticalLayout->removeWidget(labelImage);
    go();
}

void zoomWindow::openSaveDir(bool fav)
{
    QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
    QString path = settings.value("Save/path"+QString(fav ? "_favorites" : "")).toString().replace("\\", "/"), fn = settings.value("Save/filename"+QString(fav ? "_favorites" : "")).toString();

    if (path.right(1) == "/")
    { path = path.left(path.length()-1); }
    path = QDir::toNativeSeparators(path);
    QString file = m_image->path(fn, path), pth = file.section(QDir::toNativeSeparators("/"), 0, -2), url = path+QDir::toNativeSeparators("/")+pth;

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
void zoomWindow::contextMenu(QPoint point)
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
        menu->addAction(QIcon(":/images/icons/tab.png"), tr("Ouvrir dans un nouvel onglet"), this, SLOT(openInNewTab()));
        menu->addAction(QIcon(":/images/icons/window.png"), tr("Ouvrir dans une nouvelle fenêtre"), this, SLOT(openInNewWindow()));
    }
    menu->exec(QCursor::pos());
}
void zoomWindow::openInNewTab()
{ _mainwindow->addTab(link); }
void zoomWindow::openInNewWindow()
{
    QProcess myProcess;
    myProcess.startDetached(m_program, QStringList(link));
}
void zoomWindow::favorite()
{
    QString image = saveImage();
    QPixmap pix(image);
    pix.scaled(QSize(150, 150), Qt::KeepAspectRatio, Qt::SmoothTransformation).save(savePath("thumbs/"+link+".png"));

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

    if (image->width() > 150 || image->height() > 150)
    {
        QPixmap img = image->scaled(QSize(150,150), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        img.save(savePath("thumbs/"+link+".png"), "PNG");
    }
    else
    { image->save(savePath("thumbs/"+link+".png"), "PNG"); }

    _mainwindow->updateFavorites();
    _mainwindow->updateFavoritesDock();
}
void zoomWindow::unfavorite()
{
    m_favorites.removeAll(link);
    QFile f(savePath("favorites.txt"));
    f.open(QIODevice::ReadOnly);
        QString favs = f.readAll();
    f.close();
    favs.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
    QRegExp reg(link+"\\|(.+)\\r\\n");
    reg.setMinimal(true);
    favs.remove(reg);
    f.open(QIODevice::WriteOnly);
        f.write(favs.toUtf8());
    f.close();
    if (QFile::exists(savePath("thumbs/"+link+".png")))
    { QFile::remove(savePath("thumbs/"+link+".png")); }

    _mainwindow->updateFavorites();
    _mainwindow->updateFavoritesDock();
}
void zoomWindow::viewitlater()
{
    m_viewItLater.append(link);
    QFile f(savePath("viewitlater.txt"));
    f.open(QIODevice::WriteOnly);
        f.write(m_viewItLater.join("\r\n").toUtf8());
    f.close();

    _mainwindow->updateKeepForLater();
}
void zoomWindow::unviewitlater()
{
    m_viewItLater.removeAll(link);
    QFile f(savePath("viewitlater.txt"));
    f.open(QIODevice::WriteOnly);
        f.write(m_viewItLater.join("\r\n").toUtf8());
    f.close();

    _mainwindow->updateKeepForLater();
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

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        /*QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);
        diskCache->setCacheDirectory(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
		manager->setCache(diskCache);*/
	connect(manager, SIGNAL(sslErrors(QNetworkReply*, QList<QSslError>)), this, SLOT(sslErrorHandler(QNetworkReply*, QList<QSslError>)));

    QNetworkRequest request = QNetworkRequest(QUrl(m_url));
        request.setRawHeader("Referer", m_url.toAscii());
        //request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);

    m_reply = manager->get(request);
    connect(m_reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
    connect(m_reply, SIGNAL(finished()), this, SLOT(replyFinishedZoom()));

    m_replyExists = true;
}
void zoomWindow::sslErrorHandler(QNetworkReply* qnr, QList<QSslError>)
{ qnr->ignoreSslErrors(); }

#define UPDATES 16
void zoomWindow::downloadProgress(qint64 size, qint64 total)
{
    if (size - m_data.size() >= total/UPDATES)
    {
        m_data.append(m_reply->readAll());
        m_thread = true;
        m_th = new ImageThread(m_data);
        connect(m_th, SIGNAL(finished(QImage, int)), this, SLOT(display(QImage, int)));
        connect(m_th, SIGNAL(finished()), m_th, SLOT(deleteLater()));
        m_th->start();
        /*QPixmap image;
        image.loadFromData(m_data);
        display(&image, m_data.size());*/
    }
}
void zoomWindow::display(QPixmap *pix, int size)
{
    if (!pix->size().isEmpty() && size >= m_size)
    {
        m_size = size;
        image = pix;
        update(!m_finished);
    }
}
void zoomWindow::display(QPixmap pix, int size)
{
    if (!pix.size().isEmpty() && size >= m_size)
    {
        m_size = size;
        delete image;
        image = new QPixmap(pix);
        update(!m_finished);
        m_thread = false;
    }
}
void zoomWindow::display(QImage pix, int size)
{
    if (!pix.size().isEmpty() && size >= m_size)
    {
        m_size = size;
        delete image;
        image = new QPixmap(QPixmap::fromImage(pix));
        update(!m_finished);
        m_thread = false;
    }
}

void zoomWindow::replyFinished(Image* img)
{
    m_image = img;
    colore();

    QStringList pools = QStringList();
    for (int i = 0; i < img->pools().size(); i++)
    {
        Pool *p = img->pools().at(i);
        pools.append((p->previous() != 0 ? "<a href=\""+QString::number(p->previous())+"\">&lt;</a> " : "")+"<a href=\"pool:"+QString::number(p->id())+"\">"+p->name()+"</a>"+(p->next() != 0 ? " <a href=\""+QString::number(p->next())+"\">&gt;</a>" : ""));
    }
    if (!pools.isEmpty())
    { ui->labelPools->show(); }
    ui->labelPools->setText(pools.join("<br />"));

    QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
    QString path1 = settings.value("Save/path").toString().replace("\\", "/");
    QString pth1 = m_image->path(settings.value("Save/filename").toString(), path1);
    if (path1.right(1) == "/")
    { path1 = path1.left(path1.length()-1); }
    QFile file1(path1+"/"+pth1);

    QString path2 = settings.value("Save/path_favorites").toString().replace("\\", "/");
    QString pth2 = m_image->path(settings.value("Save/filename_favorites").toString(), path2);
    if (path2.right(1) == "/")
    { path2 = path2.left(path2.length()-1); }
    QFile file2(path2+"/"+pth2);

    if ((file1.exists() && !path1.isEmpty() && !pth1.isEmpty()) || (file2.exists() && !path2.isEmpty() && !pth2.isEmpty()))
    {
        if (file1.exists())
        {
            ui->buttonSave->setText(tr("Fichier déjà existant"));
            ui->buttonSaveNQuit->setText(tr("Fermer"));
        }
        if (file2.exists())
        {
            ui->buttonSaveFav->setText(tr("Fichier déjà existant (fav)"));
            ui->buttonSaveNQuitFav->setText(tr("Fermer (fav)"));
        }
        m_source = file1.exists() ? path1+"/"+pth1 : path2+"/"+pth2;
        if (m_url.section('.', -1).toUpper() == "GIF")
        {
            this->movie = new QMovie(m_source, QByteArray(), this);
            labelImage->setMovie(movie);
            movie->start();
        }
        else
        {
            QPixmap *img = new QPixmap;
            img->load(m_source);
            this->image = img;
            this->loaded = true;
            update();
        }
    }
    else
    { load(); }
}
void zoomWindow::colore()
{
    QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
    bool under = settings.value("Save/remplaceblanksbyunderscores", false).toBool();
    QStringList blacklistedtags(settings.value("blacklistedtags").toString().split(' '));

    QStringList tlist = QStringList() << "blacklisteds" << "ignoreds" << "artists" << "copyrights" << "characters" << "models" << "generals" << "favorites";
    QStringList defaults = QStringList() << "000000" << "999999" << "aa0000" << "aa00aa" << "00aa00" << "0000ee" << "000000" << "ffc0cb";
    QMap<QString,QString> styles;
    for (int i = 0; i < tlist.size(); i++)
    {
        QFont font;
        font.fromString(settings.value("Coloring/Fonts/"+tlist.at(i)).toString());
        styles[tlist.at(i)] = "color:"+settings.value("Coloring/Colors/"+tlist.at(i), "#"+defaults.at(i)).toString()+"; "+qfonttocss(font);
    }
    QStringList t;
    for (int i = 0; i < m_image->tags().size(); i++)
    {
        Tag tag = m_image->tags().at(i);
        QString normalized = tag.text().replace("\\", " ").replace("/", " ").replace(":", " ").replace("|", " ").replace("*", " ").replace("?", " ").replace("\"", " ").replace("<", " ").replace(">", " ").trimmed();
        if (under)
        { normalized.replace(' ', '_'); }
        this->details[tag.type()+"s"].append(normalized);
        this->details["alls"].append(normalized);
        QString type = blacklistedtags.contains(tag.text(), Qt::CaseInsensitive) ? "blacklisteds" : (m_ignore.contains(tag.text(), Qt::CaseInsensitive) ? "ignored" : tag.type());
        t.append("<a href=\""+tag.text()+"\" style=\""+(styles.contains(type+"s") ? styles[type+"s"] : styles["generals"])+"\">"+tag.text()+"</a>");
    }
    tags = t.join(" ");
    if (ui->widgetLeft->isHidden())
    { ui->labelTagsTop->setText(tags); }
    else
    { ui->labelTagsLeft->setText(t.join("<br/>")); }
    m_detailsWindow->setTags(tags);
}

void zoomWindow::replyFinishedZoom()
{
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
        if (m_url.section('.', -1).toUpper() == "GIF")
        {
            QTemporaryFile f;
            if (f.open())
            {
                f.write(m_data);
                f.close();
                this->movie = new QMovie(f.fileName(), QByteArray(), this);
                labelImage->setMovie(movie);
                movie->start();
            }
            else
            { error(this, tr("Une erreur inattendue est survenue lors du chargement de l'image GIF.\r\n%1").arg(m_reply->url().toString())); }
        }
        else
        {
            /*QPixmap *img = new QPixmap();
            img->loadFromData(m_data);
            this->image = img;
            this->update();*/
            /*if (m_thread && m_th->isRunning())
            { m_th->quit(); }*/
            m_thread = true;
            m_th = new ImageThread(m_data);
            connect(m_th, SIGNAL(finished(QImage, int)), this, SLOT(display(QImage, int)));
            connect(m_th, SIGNAL(finished()), m_th, SLOT(deleteLater()));
            m_th->start();
            this->loaded = true;
        }
        if (this->m_mustSave > 0)
        { this->saveImage(); }
    }
    else if (m_reply->error() == QNetworkReply::ContentNotFoundError && m_url.section('.', -1) != "jpeg")
    {
        QString ext = m_url.section('.', -1);
        QMap<QString,QString> nextext;
        nextext["jpg"] = "png";
        nextext["png"] = "gif";
        nextext["gif"] = "jpeg";
        m_url = m_url.section('.', 0, -2)+"."+nextext[ext];
        log(tr("Image non trouvée. Nouvel essai avec l'extension %1...").arg(nextext[ext]));
        load();
    }
    else if (m_reply->error() != QNetworkReply::OperationCanceledError)
    { error(this, tr("Une erreur inattendue est survenue lors du chargement de l'image.\r\n%1").arg(m_reply->url().toString())); }

    m_reply->deleteLater();
    m_replyExists = false;
}



void zoomWindow::update(bool onlysize)
{
    if (m_url.section('.', -1).toUpper() != "GIF")
    {
        if (onlysize && (this->image->width() > this->labelImage->width() || this->image->height() > this->labelImage->height()))
        { this->labelImage->setImage(this->image->scaled(this->labelImage->width(), this->labelImage->height(), Qt::KeepAspectRatio, Qt::FastTransformation)); }
        else if (this->loaded)
        {
            if (this->image->width() > this->labelImage->width() || this->image->height() > this->labelImage->height())
            { this->labelImage->setImage(this->image->scaled(this->labelImage->width(), this->labelImage->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); }
            else
            { this->labelImage->setImage(*this->image); }
        }
    }
}

void zoomWindow::saveNQuit()
{
    if (loaded)
    {
        if (!this->saveImage().isEmpty())
        { this->close(); }
    }
    else
    {
        ui->buttonSaveNQuit->setText(tr("Sauvegarde..."));
        m_mustSave = 2;
    }
}
void zoomWindow::saveNQuitFav()
{
    if (loaded)
    {
        if (!this->saveImageFav().isEmpty())
        { this->close(); }
    }
    else
    {
        ui->buttonSaveNQuitFav->setText(tr("Sauvegarde..."));
        m_mustSave = 4;
    }
}

QString zoomWindow::saveImage(bool fav)
{
    if (!loaded) // If image is still loading, we wait for it to finish
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
        return QString();
    }

    QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
    QString pth = settings.value("Save/path"+QString(fav ? "_favorites" : "")).toString().replace("\\", "/");
    if (pth.right(1) == "/")
    { pth = pth.left(pth.length()-1); }

    if (pth.isEmpty() || settings.value("Save/filename").toString().isEmpty())
    {
        int reply;
        if (pth.isEmpty())
        { reply = QMessageBox::question(this, tr("Erreur"), tr("Vous n'avez pas précisé de dossier de sauvegarde ! Voulez-vous ouvrir les options ?"), QMessageBox::Yes | QMessageBox::No); }
        else
        { reply = QMessageBox::question(this, tr("Erreur"), tr("Vous n'avez pas précisé de format de sauvegarde ! Voulez-vous ouvrir les options ?"), QMessageBox::Yes | QMessageBox::No); }
        if (reply == QMessageBox::Yes)
        {
            optionsWindow *options = new optionsWindow(_mainwindow);
            //options->onglets->setCurrentIndex(3);
            options->setWindowModality(Qt::ApplicationModal);
            options->show();
            connect(options, SIGNAL(closed()), this, SLOT(saveImage()));
        }
        return QString();
    }

    QString path = m_image->path(settings.value("Save/filename"+QString(fav ? "_favorites" : "")).toString(), pth);
    QString fp = QDir::toNativeSeparators(pth+"/"+path);
    QFile f(fp);
    if (!f.exists())
    {
        QDir path_to_file(fp.section(QDir::toNativeSeparators("/"), 0, -2));
        if (!path_to_file.exists())
        {
            QDir dir(pth);
            if (!dir.mkpath(path.section(QDir::toNativeSeparators("/"), 0, -2)))
            { error(this, tr("Erreur lors de la sauvegarde de l'image.\r\n%1").arg(fp)); return QString(); }
        }

        log(tr("Sauvegarde de l'image (%1) dans le fichier <a href=\"file:///%2\">%2</a>").arg(m_data.size()).arg(f.fileName()));
        if (m_source.isEmpty())
        {
            f.open(QIODevice::WriteOnly);
            f.write(m_data);
            f.close();
        }
        else
        { QFile::copy(m_source, f.fileName()); }

        // Commands: initialization
        QProcess *p = new QProcess(this);
        if (!settings.value("Exec/Group/init").toString().isEmpty())
        {
            log(tr("Execution de la commande d'initialisation' \"%1\"").arg(settings.value("Exec/Group/init").toString()));
            p->start(settings.value("Exec/Group/init").toString());
            if (!p->waitForStarted(10000))
            { log(tr("<b>Erreur :</b> %1").arg(tr("erreur lors de la commande d'initialisation : %1.").arg("timed out"))); }
        }
        // Commands: tags
        QMap<QString,int> types;
        types["general"] = 0;
        types["artist"] = 1;
        types["general"] = 2;
        types["copyright"] = 3;
        types["character"] = 4;
        types["model"] = 5;
        types["photo_set"] = 6;
        for (int i = 0; i < m_image->tags().count(); i++)
        {
            Tag tag = m_image->tags().at(i);
            QString original = tag.text().replace(" ", "_");
            if (!settings.value("Exec/tag").toString().isEmpty())
            {
                QString exec = settings.value("Exec/tag").toString()
                .replace("%tag%", original)
                .replace("%type%", tag.type())
                .replace("%number%", QString::number(types[tag.type()]));
                log(tr("Execution seule de \"%1\"").arg(exec));
                QProcess::execute(exec);
            }
            if (!settings.value("Exec/Group/tag").toString().isEmpty())
            {
                QString exec = settings.value("Exec/Group/tag").toString()
                .replace("%tag%", original)
                .replace("%type%", tag.type())
                .replace("%number%", QString::number(types[tag.type()]));
                log(tr("Execution groupée de \"%1\"").arg(exec));
                p->write(exec.toAscii());
            }
        }
        // Commands: image
        if (!settings.value("Exec/image").toString().isEmpty())
        {
            QString exec = m_image->path(settings.value("Exec/image").toString(), "", false);
            exec.replace("%path%", fp);
            exec.replace(" \\C ", " /C ");
            log(tr("Execution seule de \"%1\"").arg(exec));
            QProcess::execute(exec);
        }
        if (!settings.value("Exec/Group/image").toString().isEmpty())
        {
            QString exec = m_image->path(settings.value("Exec/Group/image").toString(), "", false);
            exec.replace("%path%", fp);
            log(tr("Execution groupée de \"%1\"").arg(exec));
            p->write(exec.toAscii());
        }

        log(tr("Sauvegardé <a href=\"file:///%1\">%1</a>").arg(pth+"/"+path));
        if (fav)
        {
            ui->buttonSaveFav->setText(tr("Sauvegardé ! (fav)"));
            ui->buttonSaveNQuitFav->setText(tr("Fermer (fav)"));
        }
        else
        {
            ui->buttonSave->setText(tr("Sauvegardé !"));
            ui->buttonSaveNQuit->setText(tr("Fermer"));
        }
        if (!settings.value("Exec/Group/init").toString().isEmpty())
        {
            p->closeWriteChannel();
            p->waitForFinished(1000);
            p->close();
        }
    }
    else
    {
        if (fav)
        { ui->buttonSaveFav->setText(tr("Fichier déjà existant (fav)")); }
        else
        { ui->buttonSave->setText(tr("Fichier déjà existant")); }
    }
    if (m_mustSave == 2 || m_mustSave == 4)
    { close(); }
    m_mustSave = 0;
    return pth+"/"+path;
}
QString zoomWindow::saveImageFav()
{ return saveImage(true); }

QString zoomWindow::saveImageAs()
{
    QString path = QFileDialog::getSaveFileName(this, tr("Enregistrer l'image"), m_image->fileUrl().toString().section('/', -1), "Images (*.png *.gif *.jpg *.jpeg)");
    QFile f(path);
    f.open(QIODevice::WriteOnly);
    f.write(m_data);
    f.close();
    return path;
}



void zoomWindow::fullScreen()
{
    QAffiche *label = new QAffiche(QVariant(), 0, QColor());
        label->setStyleSheet("background-color: black");
        label->setAlignment(Qt::AlignCenter);
        label->setImage(this->image->scaled(QApplication::desktop()->screenGeometry().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        label->showFullScreen();
    QShortcut *escape = new QShortcut(QKeySequence(Qt::Key_Escape), label);
        connect(escape, SIGNAL(activated()), label, SLOT(close()));
    connect(label, SIGNAL(doubleClicked()), label, SLOT(close()));
}



void zoomWindow::resizeEvent(QResizeEvent *e)
{
    if (this->loaded && m_finished && !m_thread)
    {
        if (!this->timer->isActive())
        {
            this->timeout = this->image->width()*this->image->height()/100000;
            if (this->timeout < 50)		{ this->timeout = 50;	}
            if (this->timeout > 500)	{ this->timeout = 500;	}
        }
        this->timer->stop();
        this->timer->start(this->timeout);
        this->update(true);
    }
    QWidget::resizeEvent(e);
}

void zoomWindow::closeEvent(QCloseEvent *e)
{
    QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
    settings.setValue("Zoom/geometry", saveGeometry());
    settings.setValue("Zoom/plus", ui->buttonPlus->isChecked());
    settings.sync();
    //m_image->abortTags();
    /*if (m_thread && m_th->isRunning())
    { m_th->quit(); }*/
    if (m_replyExists && m_reply->isRunning())
    {
        m_reply->abort();
        log(tr("Chargement de l'image stoppé."));
    }
    e->accept();
}

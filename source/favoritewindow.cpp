#include <QFile>
#include <QFileDialog>
#include <QSettings>
#include "favoritewindow.h"
#include "ui_favoritewindow.h"
#include "functions.h"



/**
 * Constructor of the favoriteWindow class, completing its window.
 * @param	tag			The favorite that will be edited
 * @param	note		Its note, from 0 to 100
 * @param	lastviewed	The date when the user last cliked on "Set as viewed"
 * @param	imagepath	Path to the favorite's image
 * @param	parent		The parent window
 */
favoriteWindow::favoriteWindow(Favorite fav, QWidget *parent) : QDialog(parent), ui(new Ui::favoriteWindow), favorite(fav)
{
	ui->setupUi(this);

	ui->tagLineEdit->setText(favorite.getName());
	ui->noteSpinBox->setValue(favorite.getNote());
	ui->lastViewedDateTimeEdit->setDateTime(favorite.getLastViewed());

	connect(this, SIGNAL(accepted()), this, SLOT(save()));
}

/**
 * Destructor of the favoriteWindow class
 */
favoriteWindow::~favoriteWindow()
{
	delete ui;
}

/**
 * Removes the favorite from the list and closes the window
 */
void favoriteWindow::on_buttonRemove_clicked()
{
	QFile f(savePath("favorites.txt"));
	f.open(QIODevice::ReadOnly);
		QString favs = f.readAll();
	f.close();

	favs.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
	QRegExp reg(QRegExp::escape(favorite.getName()) + "\\|(.+)\\r\\n");
	reg.setMinimal(true);
	favs.remove(reg);

	f.open(QIODevice::WriteOnly);
		f.write(favs.toUtf8());
	f.close();

	if (QFile::exists(savePath("thumbs/" + favorite.getName(true) + ".png")))
	{ QFile::remove(savePath("thumbs/" + favorite.getName(true) + ".png")); }
	emit favoritesChanged();

	close();
}

/**
 * Opens a window to choose an image to set the imagepath value.
 */
void favoriteWindow::on_openButton_clicked()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Choisir une image"), QSettings(savePath("settings.ini"), QSettings::IniFormat).value("Save/path").toString(), "Images (*.png *.gif *.jpg *.jpeg)");
	if (!file.isEmpty())
	{ ui->imageLineEdit->setText(file); }
}

/**
 * Update the local favorites file and add thumb if necessary.
 */
void favoriteWindow::save()
{
	Favorite oldFav = favorite;
	favorite = Favorite(oldFav.getId(), ui->tagLineEdit->text());
	favorite.setNote(ui->noteSpinBox->value());
	favorite.setLastViewed(ui->lastViewedDateTimeEdit->dateTime());

	if (QFile::exists(ui->imageLineEdit->text()))
	{
		QPixmap img(ui->imageLineEdit->text());
		if (!img.isNull())
		{ favorite.setImage(img); }
	}
	else if (oldFav.getName() != ui->tagLineEdit->text() && QFile::exists(savePath("thumbs/" + oldFav.getName(true) + ".png")))
	{ QFile::rename(savePath("thumbs/" + oldFav.getName(true) + ".png"), savePath("thumbs/" + favorite.getName(true) + ".png")); }

	QFile f(savePath("favorites.txt"));
	f.open(QIODevice::ReadOnly);
		QString favorites = f.readAll();
	f.close();

	favorites.replace("\r\n", "\n").replace("\r", "\n").replace("\n", "\r\n");
	favorites.remove(oldFav.getName() + "|" + QString::number(oldFav.getNote()) + "|" + oldFav.getLastViewed().toString(Qt::ISODate) + "\r\n");
	favorites += favorite.getName() + "|" + QString::number(favorite.getNote()) + "|" + favorite.getLastViewed().toString(Qt::ISODate) + "\r\n";

	f.open(QIODevice::WriteOnly);
		f.write(favorites.toUtf8());
	f.close();

	emit favoritesChanged();
}

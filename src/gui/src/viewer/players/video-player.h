#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include "player.h"
#include <QString>


namespace Ui
{
	class VideoPlayer;
}


class QAudioOutput;
class QMediaPlayer;
class QVideoWidget;
class QWidget;

class VideoPlayer : public Player
{
	Q_OBJECT

	public:
		explicit VideoPlayer(bool showControls, QWidget *parent = nullptr);
		~VideoPlayer();

		bool supports(const QString &file) override;
		void load(const QString &file) override;
		void unload() override;
		int duration() override;

	protected slots:
		void playPause();
		void durationChanged(qint64 duration);
		void positionChanged(qint64 pos);
		void seek(int position);

	private:
		Ui::VideoPlayer *ui;
		QStringList m_supportedFormats;
		QVideoWidget *m_videoWidget;
		QAudioOutput *m_audioOutput;
		QMediaPlayer *m_mediaPlayer;
		bool m_noSeek = false;
};

#endif // VIDEO_PLAYER_H

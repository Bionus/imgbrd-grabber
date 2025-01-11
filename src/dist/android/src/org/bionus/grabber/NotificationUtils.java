package org.bionus.grabber;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.BitmapFactory;
import android.app.NotificationChannel;

public class NotificationUtils
{
    public static void notify(Context context, String title, String message)
    {
        NotificationManager m_notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);

        Notification.Builder m_builder;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
            int importance = NotificationManager.IMPORTANCE_DEFAULT;
            NotificationChannel notificationChannel;
            notificationChannel = new NotificationChannel("org.bionus.grabber", "Grabber", importance);
            m_notificationManager.createNotificationChannel(notificationChannel);
            m_builder = new Notification.Builder(context, notificationChannel.getId());
        } else {
            m_builder = new Notification.Builder(context);
        }

        Bitmap icon = BitmapFactory.decodeResource(context.getResources(), R.drawable.icon);
        m_builder.setSmallIcon(R.drawable.icon)
            .setLargeIcon(icon)
            .setContentTitle(title)
            .setContentText(message)
            .setDefaults(Notification.DEFAULT_SOUND)
            .setColor(Color.GREEN)
            .setAutoCancel(true);

        m_notificationManager.notify(0, m_builder.build());
    }
}
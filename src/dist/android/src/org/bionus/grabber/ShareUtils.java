package org.bionus.grabber;

import java.io.File;
import java.lang.String;
import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;

import androidx.core.content.FileProvider;

public class ShareUtils
{
	private static String AUTHORITY = "org.bionus.grabber.fileprovider";

	protected ShareUtils()
	{}

	public static boolean share(Activity context, String text)
	{
		if (context == null) {
			return false;
		}

		Intent sendIntent = new Intent();
		sendIntent.setAction(Intent.ACTION_SEND);
		sendIntent.putExtra(Intent.EXTRA_TEXT, text);
		sendIntent.setType("text/plain");
		context.startActivity(sendIntent);

		return true;
	}

	public static boolean sendFile(Activity context, String path, String mimeType, String title)
	{
		if (context == null) {
			return false;
		}

		// Use a FileProvider to avoid "FileUriExposedException"
		File imageFileToShare = new File(path);
        Uri uri;
        try {
            uri = FileProvider.getUriForFile(context, AUTHORITY, imageFileToShare);
        } catch (IllegalArgumentException e) {
            Log.d("org.bionus.grabber.ShareUtils.sendFile", "Cannot share file: " + e.toString());
            return false;
        }
		Log.d("org.bionus.grabber.ShareUtils.sendFile", uri.toString());

		Intent sendIntent = new Intent();
		sendIntent.setAction(Intent.ACTION_SEND);
        sendIntent.putExtra(Intent.EXTRA_STREAM, uri);
		sendIntent.setType(mimeType);
		sendIntent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
		context.startActivity(Intent.createChooser(sendIntent, title));

		return true;
	}
}

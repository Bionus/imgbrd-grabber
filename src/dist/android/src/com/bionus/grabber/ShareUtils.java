package com.bionus.grabber;

import org.qtproject.qt5.android.QtNative;

import java.io.File;
import java.lang.String;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;

import android.support.v4.content.FileProvider;

public class ShareUtils
{
	private static String AUTHORITY = "com.bionus.grabber.fileprovider";

	protected ShareUtils()
	{}

	public static boolean share(String text)
	{
		if (QtNative.activity() == null) {
			return false;
		}

		Intent sendIntent = new Intent();
		sendIntent.setAction(Intent.ACTION_SEND);
		sendIntent.putExtra(Intent.EXTRA_TEXT, text);
		sendIntent.setType("text/plain");
		QtNative.activity().startActivity(sendIntent);

		return true;
	}

	public static boolean sendFile(String path, String mimeType, String title)
	{
		if (QtNative.activity() == null) {
			return false;
		}

		// Use a FileProvider to avoid "FileUriExposedException"
		File imageFileToShare = new File(path);
        Uri uri;
        try {
            uri = FileProvider.getUriForFile(QtNative.activity(), AUTHORITY, imageFileToShare);
        } catch (IllegalArgumentException e) {
            Log.d("com.bionus.grabber.ShareUtils.sendFile", "Cannot share file: " + e.toString());
            return false;
        }
		Log.d("com.bionus.grabber.ShareUtils.sendFile", uri.toString());

		Intent sendIntent = new Intent();
		sendIntent.setAction(Intent.ACTION_SEND);
        sendIntent.putExtra(Intent.EXTRA_STREAM, uri);
		sendIntent.setType(mimeType);
		sendIntent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
		QtNative.activity().startActivity(Intent.createChooser(sendIntent, title));

		return true;
	}
}

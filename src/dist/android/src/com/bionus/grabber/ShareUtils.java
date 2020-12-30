package com.bionus.grabber;

import org.qtproject.qt5.android.QtNative;

import java.lang.String;
import android.content.Intent;
import android.util.Log;

public class ShareUtils
{
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

		Intent sendIntent = new Intent();
		sendIntent.setAction(Intent.ACTION_SEND);
		sendIntent.putExtra(Intent.EXTRA_STREAM, Uri.parse(path));
		sendIntent.setType(mimeType);
		QtNative.activity().startActivity(Intent.createChooser(sendIntent, title));

		return true;
	}
}

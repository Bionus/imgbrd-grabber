package com.bionus.grabber;

import org.qtproject.qt5.android.QtNative;

import java.lang.String;
import android.content.Intent;
import android.util.Log;

public class ShareUtils
{
	protected ShareUtils()
	{}

	public static boolean share(String text, String url)
	{
		if (QtNative.activity() == null) {
			return false;
		}

		Intent sendIntent = new Intent();
		sendIntent.setAction(Intent.ACTION_SEND);
		sendIntent.putExtra(Intent.EXTRA_TEXT, text + " " + url);
		sendIntent.setType("text/plain");
		QtNative.activity().startActivity(sendIntent);

		return true;
	}
}

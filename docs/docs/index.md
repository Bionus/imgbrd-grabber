---
title: Getting started
permalink: /docs/
---


## Installation

* [Install steps for Windows](install/windows.md)
* [Install steps for Linux](install/linux.md)
* [Install steps for MacOS](install/macos.md)
* [Install steps for Android](install/android.md)

## Usage

### First launch

When you first start the program, you should be welcomed with this window:

![startup window](img/firstlaunch.png)

There are four fields to fill:

* Language: the interface language
* Source: the source you want to try first, you can change it later, and even select more than one at a time
* Folder: where you want to store your images
* Filename: the biggest strength of Grabber is its filename formatting! See the [Filename](filename.md) documentation page for more details. The default value of `%md5%.%ext%` should be good at first.

Once finished, you can press the "OK" button.

!!! note

    If you have ignored the window that opened at first launch, you can set these settings in "Edit > Options".
    Then, go the the "Save" part. Set the "Folder" and the "Filename" fields (it is in a subpart of the "Save" part, you may need to expand it).

You should now be seeing something like this:

![startup window](img/start.png)

### Searching

The upper part of the window is "divided" into two sections:

* The search form: to make searches using tags. To insert a date, or access advanced features, you can click the "Plus" button. You can also browse other pages by changing the page value on the right.
* Hidden form: you can show it by clicking the "+" button. Here, you have some fields to quickly change the settings (images per page and columns), the sources, and "Post-filtering". Post-filtering allows you, for example, to bypass the two-tags limitation of Danbooru by adding additional tags or meta (source, id, height, etc.) in this field. Note that none of these fields will be saved upon exiting. So if you want Grabber to remember your changes to the number of images per page or columns for example, you should change them directly in the options.

After searching for something, you should see something as:

![search results](img/search.png)

### Downloading

#### Batch downloads

To download a lot of pictures at the same time, first search for the tag you want to download (or no tag if that's what you want). You should arrive on the search page. From here, just click "Get this page" to download all the images on the page you are currently seeing (usually around 20 images), or "Get all" to download all the results of your search, including further pages (can go from 1 image to thousands).

Then, go to the "Downloads" tab to see a summary of what will be downloaded:

![downloads tab](img/downloadpage.png)

Once you are satisfied with your download list, click the "Download" button to start the download. The Batch Download window should appear:

![batch download window](img/batchdownload.png)

It lists all the images that will be or have already been downloaded. Now, you just have to wait. When all images are downloaded, the program will show you a summary:

![download finished window](img/downloadfinished.png)

Success!

#### Single images

To download single images, you have to "Ctrl + click" the images you want to download in the search window. They should then be marked in blue.

![single image selection](img/getselected.png)

Once you have selected all the images you want to download, click the "Get selected" button. It will mark these images to be downloaded in the "Downloads" tab:

![single image downloads tab](img/downloadselected.png)

Once you are satisfied with your download list, click the "Download" button.

### Sources

#### Basic usage

If you want to change the source of the pictures, just click the "Sources button" at the bottom of the window:

![sources window](img/sources.png)

Here, you just have to check the sources you want and then click "OK".

#### Multiple sources

If you select multiple sources, they will be put in one column by default. You can change this behavior by editing the "Columns" settings in the options.

Note that you can also merge all results to remove duplicates by checking the "Merge results" option at the bottom of the window. It is useful if you browse from sites that mirror each other, while still having their uploaders.

#### Logging in

Some sources (for example Danbooru or Gelbooru) require login to enable full API access. Even though it is usually not mandatory thanks to the regex fallback, it is usually strongly advised to do so.

To log into a source using an account, just enter your credentials in the "Credentials" tab of the Source Settings window:

![credentials form](img/sourcecredentials.png)

When you're done, you can click the "Test" button to check if your credentials are correct. Note that not all sources support login checking.

![credentials test success](img/sourcecredentialssuccess.png)

Success!

---
layout: index
---


Imageboard/booru downloader which can download thousands of images from multiple boorus very easily. It can either be used through its graphical user intertface or command-line.

Thanks to its powerful naming features, you just have to set your filename and save directory using all the tokens available, and the program will generate a filename using the image's information. With this, you can store and manage your pictures in advanced directory structures, and save image with custom filenames!

Grabber works on Windows, Mac, and Linux.



# Downloads

<div class="downloads" markdown="1">

<div class="download" markdown="1">
![Windows](assets/img/downloads/windows.png)

{% for asset in site.github.latest_release.assets %}
{% if asset.name contains '_x64.exe' %}
[Windows (x64)]({{ asset.browser_download_url }})
{% endif %}
{% endfor %}

{% for asset in site.github.latest_release.assets %}
{% if asset.name contains '_x86.exe' %}
[Windows (x86)]({{ asset.browser_download_url }})
{% endif %}
{% endfor %}
</div>

<div class="download" markdown="1">
![Linux](assets/img/downloads/linux.png)

{% for asset in site.github.latest_release.assets %}
{% if asset.name contains '.AppImage' %}
[Linux (AppImage)]({{ asset.browser_download_url }})
{% endif %}
{% endfor %}

{% for asset in site.github.latest_release.assets %}
{% if asset.name contains '.tar.gz' %}
[Linux (tar.gz)]({{ asset.browser_download_url }})
{% endif %}
{% endfor %}
</div>

<div class="download" markdown="1">
![MacOS](assets/img/downloads/macos.png)

{% for asset in site.github.latest_release.assets %}
{% if asset.name contains '.dmg' %}
[MacOS (dmg)]({{ asset.browser_download_url }})
{% endif %}
{% endfor %}
</div>

<div class="download" markdown="1">
![MacOS](assets/img/downloads/android.png)

{% for asset in site.github.latest_release.assets %}
{% if asset.name contains '.apk' %}
[Android]({{ asset.browser_download_url }})
{% endif %}
{% endfor %}
</div>

</div>



# Sources

<div class="sources" markdown="1">
<i>Note that those are only a few sources present by default, but you can add as many as you want very easily!</i>

<div class="source" markdown="1">
![Danbooru](assets/img/sources/danbooru.png)

Danbooru
</div>

<div class="source" markdown="1">
![Gelbooru](assets/img/sources/gelbooru.png)

Gelbooru
</div>

<div class="source" markdown="1">
![Shimmie](assets/img/sources/shimmie.png)

Shimmie
</div>

<div class="source" markdown="1">
![Yandere](assets/img/sources/yandere.png)

Yande.re
</div>

<div class="source" markdown="1">
![e621](assets/img/sources/e621.png)

e621
</div>

<div class="source" markdown="1">
![Konachan](assets/img/sources/konachan.png)

Konachan
</div>

<div class="source" markdown="1">
![Zerochan](assets/img/sources/zerochan.png)

Zerochan
</div>

<div class="source" markdown="1">
![rule34](assets/img/sources/rule34.png)

rule34
</div>

<div class="source" markdown="1">
![Safebooru](assets/img/sources/safebooru.png)

Safebooru
</div>

<div class="source" markdown="1">
![Behoimi](assets/img/sources/behoimi.png)

Behoimi
</div>
</div>



# Features

<div class="features" markdown="1">
<div class="feature" markdown="1">
[![basic search](assets/img/screenshots/search-basic-thumb.png)](assets/img/screenshots/search-basic.png "You can use Grabber's interface to search the same way as you would on the website directly.")
[![multi-source search](assets/img/screenshots/search-multiple-thumb.png)](assets/img/screenshots/search-multiple.png "You can search from as many sources as you want at the same time.")
[![merged search](assets/img/screenshots/search-merged-thumb.png)](assets/img/screenshots/search-merged.png "If you don't want to see the same images on different sources, you can easily merge results to remove all duplicates.")

## Browse

* Tabs to make multiple searchs at the same time
* Able to show results from multiple imageboards at the same time in a single tab
* Remove duplicate results from multiple-imageboard searches
* Auto-completion of tags in the search field
* Blacklisting of tags to mark or hide images you don't want to see
* Proxy support
* Post-filtering (useful for imageboards limiting the number of tags you can use in a single search)
* Auto-download images as you search according to a whitelist
</div>

<div class="feature" markdown="1">
[![download window](assets/img/screenshots/download-thumb.png)](assets/img/screenshots/download.png "If you feel like downloading all results of a given search, it just takes a few clicks to start a download! Then you can just wait for the images to download.")

## Download

* Download thousands of images at once
* Download single images using their MD5 or ID
* Command line interface to download images
</div>

<div class="feature" markdown="1">
[![sources window](assets/img/screenshots/sources-thumb.png)](assets/img/screenshots/sources.png "Adding a new source is as easy as copying and pasting its URL.")

## Customize

* Add your own imageboards very easily
* Authentication for sources behind a login wall
* Theme support using CSS. See [Themes](https://github.com/Bionus/imgbrd-grabber/wiki/Themes) for details.
* Lots of options to customize the program's behaviour
</div>

<div class="feature" markdown="1">
[![filename window](assets/img/screenshots/filename-thumb.png)](assets/img/screenshots/filename.png "Download your images where you want, in any directory structure you want, with the filename you want. All of the image's metadata is available for you to use as tokens in the filename.")

## Organize

* Save images using a custom format, for example `%artist%/%copyright%/%character%/%md5%.%ext%`, or using Javascript code. See [Filename](https://github.com/Bionus/imgbrd-grabber/wiki/Filename) for details.
* Favorite tags to keep track of new images
* "View it later" tags to save a search for later
* Support saving images directly to a local booru, such as [Shimmie](https://github.com/Bionus/imgbrd-grabber/wiki/Shimmie), [Gelbooru](https://github.com/Bionus/imgbrd-grabber/wiki/Gelbooru) or [MyImouto](https://github.com/Bionus/imgbrd-grabber/wiki/MyImouto).
* Can add entries to a database for each image or tag while downloading. See [Commands](https://github.com/Bionus/imgbrd-grabber/wiki/Commands) for details.
* Conditional filenames triggered by a tag
* Rename already downloaded images
</div>
</div>



# Languages

<div class="flags" markdown="1">
<div class="flag" markdown="1">
![English flag](assets/img/flags/en.png)

English
</div>

<div class="flag" markdown="1">
![French flag](assets/img/flags/fr.png)

French
</div>

<div class="flag" markdown="1">
![Russian flag](assets/img/flags/ru.png)

Russian
</div>

<div class="flag" markdown="1">
![Chinese flag](assets/img/flags/cn.png)

Chinese (simplified)
</div>

<div class="flag" markdown="1">
![Spanish flag](assets/img/flags/sp.png)

Spanish
</div>
</div>



# Contact

If you have any questions about the program, found a bug and don't want to use the github issue tracker, or anything, you can contact me by mail in French or in English at [bio.nus@hotmail.fr](mailto:bio.nus@hotmail.fr).

Please prefer using Github's built-in [issue tracker](https://github.com/Bionus/imgbrd-grabber/issues) though, as the answer to your question or problem might help other people!
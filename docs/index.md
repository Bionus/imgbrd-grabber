---
title: Home
template: home.html
hide: [navigation, toc]
glightbox: false
---



## Features

<div class="features" markdown="1">
<div class="feature" markdown="1">

[![basic search](assets/img/screenshots/search-basic-thumb.png)](assets/img/screenshots/search-basic.png "You can use Grabber's interface to search the same way as you would on the website directly.")
[![multi-source search](assets/img/screenshots/search-multiple-thumb.png)](assets/img/screenshots/search-multiple.png "You can search from as many sources as you want at the same time.")
[![merged search](assets/img/screenshots/search-merged-thumb.png)](assets/img/screenshots/search-merged.png "If you don't want to see the same images on different sources, you can easily merge results to remove all duplicates.")

### Browse

* Tabs to make multiple searches at the same time
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

### Download

* Download thousands of images at once
* Download single images using their MD5 or ID
* Command line interface to download images

</div>

<div class="feature" markdown="1">

[![sources window](assets/img/screenshots/sources-thumb.png)](assets/img/screenshots/sources.png "Adding a new source is as easy as copying and pasting its URL.")

### Customize

* Add your own imageboards very easily
* Authentication for sources behind a login wall
* Theme support using CSS. See [Themes](docs/plugins/theme.md) for details.
* Lots of options to customize the program's behaviour

</div>

<div class="feature" markdown="1">

[![filename window](assets/img/screenshots/filename-thumb.png)](assets/img/screenshots/filename.png "Download your images where you want, in any directory structure you want, with the filename you want. All of the image's metadata is available for you to use as tokens in the filename.")

### Organize

* Save images using a custom format, for example `%artist%/%copyright%/%character%/%md5%.%ext%`, or using Javascript code. See [Filename](docs/filename.md) for details.
* Favorite tags to keep track of new images
* "View it later" tags to save a search for later
* Support saving images directly to a local booru, such as [Szurubooru](docs/commands/szurubooru.md), [MyImouto](docs/commands/my-imouto.md), [Gelbooru](docs/commands/gelbooru.md), or [Shimmie](docs/commands/shimmie.md).
* Can add entries to a database for each image or tag while downloading. See [Commands](docs/commands/index.md) for details.
* Conditional filenames triggered by a tag
* Rename already downloaded images

</div>
</div>



## Sources

<div class="sources" markdown="1">
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

_Note that those are only a few sources present by default, but you can add as many as you want very easily!_



## Languages

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



## Downloads

<div class="downloads" markdown="1">

<div class="download" markdown="1">

![Windows](assets/img/downloads/windows.png)

{% for asset in site.github.latest_release.assets %}
{% if '_x64.exe' in asset.name %}
[Windows (x64)]({{ asset.browser_download_url }})
{% endif %}
{% endfor %}

{% for asset in site.github.latest_release.assets %}
{% if '_x86.exe' in asset.name %}
[Windows (x86)]({{ asset.browser_download_url }})
{% endif %}
{% endfor %}

{% for asset in site.github.latest_release.assets %}
{% if '_x64.zip' in asset.name %}
[Windows (x64, zip)]({{ asset.browser_download_url }})
{% endif %}
{% endfor %}

{% for asset in site.github.latest_release.assets %}
{% if '_x86.zip' in asset.name %}
[Windows (x86, zip)]({{ asset.browser_download_url }})
{% endif %}
{% endfor %}
</div>

<div class="download" markdown="1">

![Linux](assets/img/downloads/linux.png)

{% for asset in site.github.latest_release.assets %}
{% if '.AppImage' in asset.name and '.AppImage.zsync' not in asset.name %}
[Linux (AppImage)]({{ asset.browser_download_url }})
{% endif %}
{% endfor %}

{% for asset in site.github.latest_release.assets %}
{% if '.tar.gz' in asset.name %}
[Linux (tar.gz)]({{ asset.browser_download_url }})
{% endif %}
{% endfor %}

[Linux (Flathub)](https://flathub.org/apps/details/org.bionus.Grabber)

[Arch Linux (AUR)](docs/install/linux.md#via-the-aur-arch-linux)
</div>

<div class="download" markdown="1">

![MacOS](assets/img/downloads/macos.png)

[MacOS (homebrew)](docs/install/macos.md#via-homebrew)

{% for asset in site.github.latest_release.assets %}
{% if '.dmg' in asset.name %}
[MacOS (dmg)]({{ asset.browser_download_url }})
{% endif %}
{% endfor %}
</div>

<div class="download" markdown="1">

![MacOS](assets/img/downloads/android.png)

{% for asset in site.github.latest_release.assets %}
{% if '.apk' in asset.name %}
[Android]({{ asset.browser_download_url }})
{% endif %}
{% endfor %}
</div>

</div>
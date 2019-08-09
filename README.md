<p align="center"><img src="gui/resources/images/readme-icon.png" alt="" /></p>

<h1 align="center">Grabber</h1>

[![Gitter chat](https://badges.gitter.im/imgbrd-grabber/gitter.png)](https://gitter.im/imgbrd-grabber/Lobby)
[![GitHub release](https://img.shields.io/github/release/Bionus/imgbrd-grabber.svg)](https://github.com/Bionus/imgbrd-grabber/releases/latest)
[![GitHub downloads](https://img.shields.io/github/downloads/Bionus/imgbrd-grabber/latest/total.svg)](https://github.com/Bionus/imgbrd-grabber/releases/latest)
[![GitHub downloads](https://img.shields.io/github/downloads/Bionus/imgbrd-grabber/total.svg)](https://github.com/Bionus/imgbrd-grabber/releases)
[![GitHub issues](https://img.shields.io/github/issues/Bionus/imgbrd-grabber.svg)](https://github.com/Bionus/imgbrd-grabber/issues)
[![Donate with PayPal](https://img.shields.io/badge/paypal-donate-orange.svg)](https://www.paypal.me/jvasti)
[![Donate with Patreon](https://img.shields.io/badge/patreon-donate-orange.svg)](https://www.patreon.com/bionus)
[![Build Status](https://travis-ci.org/Bionus/imgbrd-grabber.svg?branch=master)](https://travis-ci.org/Bionus/imgbrd-grabber)
[![Build Status](https://ci.appveyor.com/api/projects/status/lm08r4q0kuui7a5y/branch/master?svg=true)](https://ci.appveyor.com/project/Bionus/imgbrd-grabber)
[![Code Coverage](https://img.shields.io/codecov/c/github/Bionus/imgbrd-grabber.svg)](https://codecov.io/gh/Bionus/imgbrd-grabber)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/044edd1462094c6e8d35cb0bcdd86a2b)](https://www.codacy.com/app/bionus/imgbrd-grabber)
[![Project license](https://img.shields.io/github/license/bionus/imgbrd-grabber.svg)](https://raw.githubusercontent.com/Bionus/imgbrd-grabber/develop/LICENSE)

Imageboard/booru downloader which can download thousands of images from multiple boorus very easily.

Thanks to its powerful naming features, you just have to set your filename and save directory using all the tokens available, and the program will generate a filename using the image's information. With this, you can store and manage your pictures in advanced directory structures, and save image with custom filenames!

Grabber works on Windows, Mac, and Linux. It is available in English, French, Russian, simplified Chinese, and Spanish.

## Download
You can download the latest release [here](https://github.com/Bionus/imgbrd-grabber/releases/latest), or find all previous releases [here](https://github.com/Bionus/imgbrd-grabber/releases).

For users interested, a nightly version is built automatically on every commit on the `develop` branch, and can be downloaded [here](https://github.com/Bionus/imgbrd-grabber/releases/nightly). Note that it might be less stable than official releases, so use at your own risk.

## Features

### Browse

[<img src="https://bionus.github.io/imgbrd-grabber/assets/img/screenshots/search-basic-thumb.png" align="right" />](https://bionus.github.io/imgbrd-grabber/assets/img/screenshots/search-basic.png)

* Tabs to make multiple searches at the same time
* Able to show results from multiple imageboards at the same time in a single tab
* Remove duplicate results from multiple-imageboard searches
* Auto-completion of tags in the search field
* Blacklisting of tags to mark or hide images you don’t want to see
* Proxy support
* Post-filtering (when the imageboard search is limited)
* Auto-download images as you search according to a whitelist

### Download

[<img src="https://bionus.github.io/imgbrd-grabber/assets/img/screenshots/download-thumb.png" align="right" />](https://bionus.github.io/imgbrd-grabber/assets/img/screenshots/download.png)

* Download thousands of images at once
* Download single images using their MD5 or ID
* Command line interface to download images

<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>

### Customize

[<img src="https://bionus.github.io/imgbrd-grabber/assets/img/screenshots/sources-thumb.png" align="right" />](https://bionus.github.io/imgbrd-grabber/assets/img/screenshots/sources.png)

* Add your own imageboards very easily
* Authentication for sources behind a login wall
* Theme support using CSS. See Themes for details.
* Lots of options to customize the program’s behaviour

<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>

### Organize

[<img src="https://bionus.github.io/imgbrd-grabber/assets/img/screenshots/filename-thumb.png" align="right" />](https://bionus.github.io/imgbrd-grabber/assets/img/screenshots/filename.png)

* Save images using a custom format, for example `%artist%/%copyright%/%character%/%md5%.%ext%`, or using JavaScript code. See Filename for details.
* Favorite tags to keep track of new images
* "View it later" tags to save a search for later
* Support saving images directly to a local booru, such as Shimmie, Gelbooru or MyImouto.
* Can add entries to a database for each image or tag while downloading. See Commands for details.
* Conditional filenames triggered by a tag
* Rename already downloaded images

## Default sources
You can add additional sources very easily, but here's a short list of some sources that are included and supported by default:
* Danbooru
* Gelbooru
* E-Hentai
* Pixiv
* yande.re
* Shimmie
* e621
* Konachan
* rule34
* safebooru
* Anime-Pictures
* behoimi
* Zerochan
* Twitter

## Compilation
See the [Compilation](https://github.com/Bionus/imgbrd-grabber/wiki/Compilation) wiki page to know how to build Grabber.

## Contributors
<!-- ALL-CONTRIBUTORS-LIST:START -->
<!-- prettier-ignore -->
<table><tr><td align="center"><a href="https://github.com/Bionus"><img src="https://avatars2.githubusercontent.com/u/882719?s=122" width="122px;" alt="Jack Vasti"/><br /><sub><b>Jack Vasti</b></sub></a><br /><a href="https://github.com/Bionus/imgbrd-grabber/commits?author=Bionus" title="Code">💻</a> <a href="https://github.com/Bionus/imgbrd-grabber/commits?author=Bionus" title="Documentation">📖</a> <a href="https://github.com/Bionus/imgbrd-grabber/commits?author=Bionus" title="Tests">⚠️</a></td><td align="center"><a href="https://github.com/Zzzyyzzyxx"><img src="https://avatars0.githubusercontent.com/u/16903308?s=122" width="122px;" alt="YMI"/><br /><sub><b>YMI</b></sub></a><br /><a href="https://github.com/Bionus/imgbrd-grabber/issues?q=author%3AYMI" title="Bug reports">🐛</a> <a href="#ideas-YMI" title="Ideas, Planning, & Feedback">🤔</a></td><td align="center"><a href="https://github.com/SultrySamthepennanceman"><img src="https://avatars2.githubusercontent.com/u/12085184?s=122" width="122px;" alt="SultrySamthepenna&hellip;"/><br /><sub><b>SultrySamthepenna&hellip;</b></sub></a><br /><a href="https://github.com/Bionus/imgbrd-grabber/issues?q=author%3ASultrySamthepennanceman" title="Bug reports">🐛</a></td><td align="center"><a href="https://github.com/BarryMode"><img src="https://avatars1.githubusercontent.com/u/5648875?s=122" width="122px;" alt="Barry Anders"/><br /><sub><b>Barry Anders</b></sub></a><br /><a href="https://github.com/Bionus/imgbrd-grabber/commits?author=BarryMode" title="Code">💻</a> <a href="https://github.com/Bionus/imgbrd-grabber/issues?q=author%3ABarryMode" title="Bug reports">🐛</a></td><td align="center"><a href="https://github.com/Flat"><img src="https://avatars3.githubusercontent.com/u/2048861?s=122" width="122px;" alt="Ken Swenson"/><br /><sub><b>Ken Swenson</b></sub></a><br /><a href="https://github.com/Bionus/imgbrd-grabber/commits?author=Flat" title="Code">💻</a> <a href="#platform-Flat" title="Packaging/porting to new platform">📦</a></td><td align="center"><a href="https://github.com/larry-he"><img src="https://avatars0.githubusercontent.com/u/18506295?s=122" width="122px;" alt="Larry He"/><br /><sub><b>Larry He</b></sub></a><br /><a href="https://github.com/Bionus/imgbrd-grabber/commits?author=larry-he" title="Code">💻</a></td></tr><tr><td align="center"><a href="https://github.com/brodycas3"><img src="https://avatars3.githubusercontent.com/u/19770864?s=122" width="122px;" alt="brodycas3"/><br /><sub><b>brodycas3</b></sub></a><br /><a href="https://github.com/Bionus/imgbrd-grabber/issues?q=author%3Abrodycas3" title="Bug reports">🐛</a></td><td align="center"><a href="https://github.com/sanddudu"><img src="https://avatars1.githubusercontent.com/u/1650692?s=122" width="122px;" alt="Klion Xu"/><br /><sub><b>Klion Xu</b></sub></a><br /><a href="#translation-sanddudu" title="Translation">🌍</a></td><td align="center"><a href="https://github.com/MasterPetrik"><img src="https://avatars2.githubusercontent.com/u/22294259?s=122" width="122px;" alt="MasterPetrik"/><br /><sub><b>MasterPetrik</b></sub></a><br /><a href="https://github.com/Bionus/imgbrd-grabber/issues?q=author%3AMasterPetrik" title="Bug reports">🐛</a> <a href="#translation-MasterPetrik" title="Translation">🌍</a> <a href="#ideas-MasterPetrik" title="Ideas, Planning, & Feedback">🤔</a></td><td align="center"><a href="https://github.com/dyskette"><img src="https://avatars3.githubusercontent.com/u/6687927?s=122" width="122px;" alt="Eddy Castillo"/><br /><sub><b>Eddy Castillo</b></sub></a><br /><a href="#translation-dyskette" title="Translation">🌍</a></td><td align="center"><a href="https://github.com/MrAndre96"><img src="https://avatars0.githubusercontent.com/u/6564956?s=122" width="122px;" alt="MrAndre96"/><br /><sub><b>MrAndre96</b></sub></a><br /><a href="https://github.com/Bionus/imgbrd-grabber/issues?q=author%3AMrAndre96" title="Bug reports">🐛</a></td><td align="center"><a href="https://github.com/brazenvoid"><img src="https://avatars1.githubusercontent.com/u/8722533?s=122" width="122px;" alt="Umair Ahmed"/><br /><sub><b>Umair Ahmed</b></sub></a><br /><a href="https://github.com/Bionus/imgbrd-grabber/issues?q=author%3Abrazenvoid" title="Bug reports">🐛</a></td></tr></table>

<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/kentcdodds/all-contributors) specification.
Contributions of any kind are welcome!
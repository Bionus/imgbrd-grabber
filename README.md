<p align="center"><img src="gui/resources/images/readme-icon.png" alt="" /></p>

<h1 align="center">Grabber</h1>

[![Gitter chat](https://badges.gitter.im/imgbrd-grabber/gitter.png)](https://gitter.im/imgbrd-grabber/Lobby)
[![GitHub release](https://img.shields.io/github/release/Bionus/imgbrd-grabber.svg)](https://github.com/Bionus/imgbrd-grabber/releases/latest)
[![GitHub downloads](https://img.shields.io/github/downloads/Bionus/imgbrd-grabber/latest/total.svg)](https://github.com/Bionus/imgbrd-grabber/releases/latest)
[![GitHub downloads](https://img.shields.io/github/downloads/Bionus/imgbrd-grabber/total.svg)](https://github.com/Bionus/imgbrd-grabber/releases)
[![GitHub issues](https://img.shields.io/github/issues/Bionus/imgbrd-grabber.svg)](https://github.com/Bionus/imgbrd-grabber/issues)
[![Donate with PayPal](https://img.shields.io/badge/paypal-donate-orange.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=bio%2enus%40hotmail%2efr&lc=EN&item_name=Bionus&item_number=Grabber&currency_code=EUR)
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
You can add additional sources very easily, but here's a short list of some sources that are included and supported by default: Danbooru, Gelbooru, yande.re, Shimmie, e621, Konachan, rule34, safebooru, behoimi, Zerochan...

## Compilation
See the [Compilation](https://github.com/Bionus/imgbrd-grabber/wiki/Compilation) wiki page to know how to build Grabber.

## Contributors
<!-- ALL-CONTRIBUTORS-LIST:START -->
<!-- prettier-ignore -->
| [<img src="https://avatars2.githubusercontent.com/u/882719?s=122" width="122px;"/><br /><sub><b>Jack Vasti</b></sub>](https://github.com/Bionus)<br />[💻](https://github.com/Bionus/imgbrd-grabber/commits?author=Bionus "Code") [📖](https://github.com/Bionus/imgbrd-grabber/commits?author=Bionus "Documentation") [⚠️](https://github.com/Bionus/imgbrd-grabber/commits?author=Bionus "Tests") | [<img src="https://avatars0.githubusercontent.com/u/16903308?s=122" width="122px;"/><br /><sub><b>YMI</b></sub>](https://github.com/Zzzyyzzyxx)<br />[🐛](https://github.com/Bionus/imgbrd-grabber/issues?q=author%3AYMI "Bug reports") [🤔](#ideas-YMI "Ideas, Planning, & Feedback") | [<img src="https://avatars2.githubusercontent.com/u/12085184?s=122" width="122px;"/><br /><sub><b>SultrySamthepenna&hellip;</b></sub>](https://github.com/SultrySamthepennanceman)<br />[🐛](https://github.com/Bionus/imgbrd-grabber/issues?q=author%3ASultrySamthepennanceman "Bug reports") | [<img src="https://avatars1.githubusercontent.com/u/5648875?s=122" width="122px;"/><br /><sub><b>Barry Anders</b></sub>](https://github.com/BarryMode)<br />[💻](https://github.com/Bionus/imgbrd-grabber/commits?author=BarryMode "Code") [🐛](https://github.com/Bionus/imgbrd-grabber/issues?q=author%3ABarryMode "Bug reports") | [<img src="https://avatars3.githubusercontent.com/u/2048861?s=122" width="122px;"/><br /><sub><b>Ken Swenson</b></sub>](https://github.com/Flat)<br />[💻](https://github.com/Bionus/imgbrd-grabber/commits?author=Flat "Code") [📦](#platform-Flat "Packaging/porting to new platform") | [<img src="https://avatars0.githubusercontent.com/u/18506295?s=122" width="122px;"/><br /><sub><b>Larry He</b></sub>](https://github.com/larry-he)<br />[💻](https://github.com/Bionus/imgbrd-grabber/commits?author=larry-he "Code") |
| :---: | :---: | :---: | :---: | :---: | :---: |
| [<img src="https://avatars3.githubusercontent.com/u/19770864?s=122" width="122px;"/><br /><sub><b>brodycas3</b></sub>](https://github.com/brodycas3)<br />[🐛](https://github.com/Bionus/imgbrd-grabber/issues?q=author%3Abrodycas3 "Bug reports") | [<img src="https://avatars1.githubusercontent.com/u/1650692?s=122" width="122px;"/><br /><sub><b>Klion Xu</b></sub>](https://github.com/sanddudu)<br />[🌍](#translation-sanddudu "Translation") | [<img src="https://avatars2.githubusercontent.com/u/22294259?s=122" width="122px;"/><br /><sub><b>MasterPetrik</b></sub>](https://github.com/MasterPetrik)<br />[🐛](https://github.com/Bionus/imgbrd-grabber/issues?q=author%3AMasterPetrik "Bug reports") [🌍](#translation-MasterPetrik "Translation") [🤔](#ideas-MasterPetrik "Ideas, Planning, & Feedback") | [<img src="https://avatars3.githubusercontent.com/u/6687927?s=122" width="122px;"/><br /><sub><b>Eddy Castillo</b></sub>](https://github.com/dyskette)<br />[🌍](#translation-dyskette "Translation") | [<img src="https://avatars0.githubusercontent.com/u/6564956?s=122" width="122px;"/><br /><sub><b>MrAndre96</b></sub>](https://github.com/MrAndre96)<br />[🐛](https://github.com/Bionus/imgbrd-grabber/issues?q=author%3AMrAndre96 "Bug reports") |
<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/kentcdodds/all-contributors) specification.
Contributions of any kind are welcome!
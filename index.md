<img src="resources/images/readme-icon.png" align="right" />

# Grabber

[![GitHub release](https://img.shields.io/github/release/Bionus/imgbrd-grabber.svg)](https://github.com/Bionus/imgbrd-grabber/releases/latest)
[![GitHub downloads](https://img.shields.io/github/downloads/Bionus/imgbrd-grabber/latest/total.svg)](https://github.com/Bionus/imgbrd-grabber/releases/latest)
[![GitHub downloads](https://img.shields.io/github/downloads/Bionus/imgbrd-grabber/total.svg)](https://github.com/Bionus/imgbrd-grabber/releases)
[![GitHub issues](https://img.shields.io/github/issues/Bionus/imgbrd-grabber.svg)](https://github.com/Bionus/imgbrd-grabber/issues)
[![Donate with PayPal](https://img.shields.io/badge/paypal-donate-orange.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=bio%2enus%40hotmail%2efr&lc=FR&item_name=Bionus&item_number=Grabber&currency_code=EUR)
[![Donate with Patreon](https://img.shields.io/badge/patreon-donate-orange.svg)](https://www.patreon.com/bionus)
[![Build Status](https://travis-ci.org/Bionus/imgbrd-grabber.svg?branch=master)](https://travis-ci.org/Bionus/imgbrd-grabber)
[![Code Coverage](https://img.shields.io/codecov/c/github/Bionus/imgbrd-grabber.svg)](https://codecov.io/gh/Bionus/imgbrd-grabber)
[![Project license](https://img.shields.io/github/license/bionus/imgbrd-grabber.svg)](https://raw.githubusercontent.com/Bionus/imgbrd-grabber/develop/LICENSE)

Imageboard/booru downloader which can download thousands of images from multiple boorus very easily. It can either be used through its graphical user intertface or command-line.

Thanks to its powerful naming features, you just have to set your filename and save directory using all the tokens available, and the program will generate a filename using the image's information. With this, you can store and manage your pictures in advanced directory structures, and save image with custom filenames!

Grabber works on Windows, Mac, and Linux.

## Download
You can download the latest release [here](https://github.com/Bionus/imgbrd-grabber/releases/latest), or find all previous releases [here](https://github.com/Bionus/imgbrd-grabber/releases).

## Main features
* Browse images from the internet
* Download thousands of images at once
* Save images using a custom format, for example `%artist%/%copyright%/%character%/%md5%.%ext%`, or using Javascript code. See [Filename](https://github.com/Bionus/imgbrd-grabber/wiki/Commands) for details.
* Add your own imageboards very easily
* Authentication for sources behind a login wall
* Command line interface to download images
* Download single images using their md5 or id
* Tabs to make multiple searchs at the same time
* Able to show results from multiple imageboards at the same time in a single tab
* Merge results from these imageboards (i.e. remove duplicates in results)
* Favorite and "view it later" tags
* Auto-completion of tags in the search field
* Post-filtering (useful for imageboards limiting the number of tags you can use in a single search)
* Autodownload images as you search according to a whitelist
* Can add entries to a database for each image or tag while downloading. See [Commands](https://github.com/Bionus/imgbrd-grabber/wiki/Commands) for details.
* Blacklisting of tags to mark or hide images you don't want to see
* Proxy support
* ... and a lot more!

## Languages
* English
* French
* Russian
* Chinese (simplified)
* Spanish

## Default sources
You can add additional sources very easily, but here's a shot list of some sources that are included and supported by default: Danbooru, Gelbooru, yande.re, Shimmie, Sankaku complex, e621, Konachan, rule34, safebooru, behoimi, Zerochan...

## Contact
If you have any questions about the program, found a bug and don't want to use the github issue tracker, or anything, you can contact me by mail in French or in English at [bio.nus@hotmail.fr](mailto:bio.nus@hotmail.fr). Please prefer using Github's built-in issue tracker though, as the answer to your question or problem might help other people!

## Compilation
If you want more info about compilation steps, see the [Compilation](https://github.com/Bionus/imgbrd-grabber/wiki/Compilation) wiki page.

### Linux
Just run the `build.sh` file that you can find at the root of the repository.

### macOS
See the [macOS compilation](https://github.com/Bionus/imgbrd-grabber/wiki/Compilation#macos) instructions for compiling Grabber on macOS.

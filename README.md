# Grabber

[![GitHub release](https://img.shields.io/github/release/Bionus/imgbrd-grabber.svg)](https://github.com/Bionus/imgbrd-grabber/releases/latest)
[![GitHub downloads](https://img.shields.io/github/downloads/Bionus/imgbrd-grabber/latest/total.svg)](https://github.com/Bionus/imgbrd-grabber/releases/latest)
[![GitHub issues](https://img.shields.io/github/issues/Bionus/imgbrd-grabber.svg)](https://github.com/Bionus/imgbrd-grabber/issues)
[![Donate with PayPal](https://img.shields.io/badge/paypal-donate-orange.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=bio%2enus%40hotmail%2efr&lc=FR&item_name=Bionus&item_number=Grabber&currency_code=EUR)
[![Donate with Patreon](https://img.shields.io/badge/patreon-donate-orange.svg)](https://www.patreon.com/bionus)
[![Donate Bitcoins](https://img.shields.io/badge/bitcoin-donate-orange.svg)](https://www.coinbase.com/bionus)
[![Project license](https://img.shields.io/github/license/bionus/imgbrd-grabber.svg)](https://raw.githubusercontent.com/Bionus/imgbrd-grabber/develop/LICENSE)
[![Build Status](https://travis-ci.org/Bionus/imgbrd-grabber.svg)](https://travis-ci.org/Bionus/imgbrd-grabber)

## About
Imgbrd-Grabber is an imageboard/booru downloader which can be used with a great a graphical user interface or through command line. It can download thousands of images from multiple boorus very easily.
One of its main advantage is its very powerful naming features: just set your filename and folder using all the tokens available and conditionals, and he will generate an image-dependant one.

## Download
You can download the latest release [here](https://github.com/Bionus/imgbrd-grabber/releases/latest), or find all previous releases [here](https://github.com/Bionus/imgbrd-grabber/releases).

## Contact
If you have any questions about the program, found a bug and don't want to use the github issue tracker, or anything, you can contact me by mail in French or in English at [bio.nus@hotmail.fr](mailto:bio.nus@hotmail.fr). Please prefer using Github's built-in issue tracker though, as the answer to your question or problem might help other people!

## Main features
* Browse images from the internet
* Download huge amounts of images at once
* Command line interface to download images
* Download single images using their md5 or id
* Rename downloaded images using a formatting string, for example "%artist%/%copyright%/%character%/%md5%.%ext%" (someway like how the Firefox extension [Danbooru Downloader](https://addons.mozilla.org/fr/firefox/addon/danbooru-downloader/) does), or directly using Javascript code. See {{Filename}} for details.
* Add and remove imageboards very easily
* Multiple tabs
* Display multiple imageboards at the same time in a single tab
* Merge results from these imageboards (i.e. remove duplicates in results)
* Favorite & "View it later" tags
* Auto-completion in the search field
* Post-filtering
* Autodownload
* Can import settings from Mozilla Firefox extension "danbooru downloader"
* Can add entries to a database for each image or tag while downloading
* Blacklisting
* Proxy support
* ... and more!

## Languages
* English
* French
* Russian (thanks to Николай Тихонов)

## Default sources
You can add additional sources very easily, but here's a shot list of some sources that are included and supported by default: Danbooru, Gelbooru, yande.re, Shimmie, Sankaku complex, e621, rule34, safebooru, behoimi...

## Authors
* [Bionus](https://github.com/Bionus)

### Special thanks
* YMI for all his suggestions and helping debbuging the program
* Николай Тихонов for the Russian translation
* Everyone who helped make this program better by contributing, suggesting features, or reporting bugs!

## Compilation

If you want more info about compilation steps, see the [Compilation](https://github.com/Bionus/imgbrd-grabber/wiki/Compilation) wiki page.

### Linux

```
./build.sh
```

Or if you want to run the commands yourself :

```
sudo add-apt-repository --yes ppa:ubuntu-sdk-team/ppa
sudo apt-get update -qq
sudo apt-get install -qq qt5-qmake qtbase5-dev qtdeclarative5-dev qtscript5-dev qtmultimedia5-dev libpulse-dev
sudo apt-get install qt5-default qttools5-dev-tools
qmake Grabber.pro
make
mv Grabber release/Grabber
touch release/settings.ini
```

### OS X

See the [OS X compilation](https://github.com/Bionus/imgbrd-grabber/wiki/Compilation#os-x) instructions for compiling Grabber on Mac.

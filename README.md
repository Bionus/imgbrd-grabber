# Grabber

## About
Imgbrd-Grabber is an imageboard/booru downloader with graphical user interface. It can download thousands of images from multiple boorus automatically. Its main advantage is its very powerful naming features: just set your filename and folder using all the tokens available and conditionals, and he will generate an image-dependant name, allowing your collection to be sorted in a clean way, unlike what other downloaders may do.

## Source code
This code was written totally chaotically at the beginning (no comments, bad classes, etc), so you can try to read it, but I feel a little bit ashamed for this. Feel free to send pull requests for any improvement you can see (be it minor or major).
However with recent versions I'm cleaning the code up, in order to simplify and fix obscure bugs in the program, so it should be getting better with time.

## Contact
If you have any questions about the program, found a bug and don't want to use the issue tracker, or anything else, you can contact me by mail in French or in English at [bio.nus@hotmail.fr](mailto:bio.nus@hotmail.fr).

## Main features
* Browse images from the internet
* Download huge amounts of images
* Download single images using their md5 or id
* Rename downloaded images using a formatting string, for example "%artist%/%copyright%/%character%/%md5%.%ext%" (someway like how the Firefox extension Danbooru Downloader does), or directly using Javascript code. See Filename for details.
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
* Blacklist
* Fullscreen
* Proxy support
* Comand line interface (CLI)
... and more!

## Languages
* English
* French
* Russian (thanks to Николай Тихонов)

## Authors
* Bionus

## Thanks
* YMI for all his suggestions and helping debbuging the program
* Николай Тихонов: for the Russian translation

## License
The program is licensed under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0).

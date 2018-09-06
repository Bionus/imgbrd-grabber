---
title: Translations
---


# Introduction

If your language is not available for the program or not up-to-date, you can add/edit it pretty easily. The goal of this page is to tell you how to do this.


# Download Qt Linguist and lrelease

The software used to write translations for Grabber is Qt Linguist. You can also use any other software that support Qt's `.ts` files.

Qt Linguist and lrelease are included in Grabber's [translation tools](https://github.com/Bionus/imgbrd-grabber/releases/download/v4.9.1/translation-tools.zip). Just download them and unzip them somewhere.


# How to create a new translation file

1. Download "YourLanguage.ts" from Github [here]( https://raw.githubusercontent.com/Bionus/imgbrd-grabber/develop/languages/YourLanguage.ts). The translation file for the Crash Reporter can be found [here](https://raw.githubusercontent.com/Bionus/imgbrd-grabber/develop/CrashReporter/languages/YourLanguage.ts).
2. Rename it by the name of your language in English, for example "German" (so the file will be called "German.ts").
3. Launch Qt Linguist.
4. Open ("File > Open..." or `Ctrl + O`) your translation file.
5. A new window will pop-up: for the source language's language, set English, and for the country USA (you can also leave the default value). For the target language and country, just put here your language and country (in the previous example, German from Germany).


# Editing an existing translation

1. Launch Qt Linguist.
2. Open ("File > Open..." or `Ctrl + O`) the language file you want to update.


# Translating

*Note: if you understand another language (such as French) as well or better than English, you can also open it: it will then show both French and English versions when asking you for a translation.*

Now that you're ready, all that's left is to actually get translating!

1. There is a part in the window with five fields: "Source text", "'Your language' translation" and "'Your language' translator comments".
2. Fill-up the "'Your language' translation" part, then mark item as done ("Translation > Done and Next", the green tick button, or `Ctrl + Return`).
3. Once all the translations have been marked as done, save and quit.
4. Drag and drop your translation file on "lrelease.exe".
5. Another file will be created : "'Your language'.qm". Move it to the "languages" directory in the Grabber's installation folder (by default "%ProgramFiles(x86)%/Grabber").
6. You can know choose your language in the options! Note that it may need a restart for all texts to be properly translated.

*You can find more information about translating with Qt Linguist in [the official Qt documentation](https://doc.qt.io/qt-5/linguist-translators.html).*


# Sending the result

Once finished, you can either send me a mail with your translation file (the `.ts` one) at bio.nus@hotmail.fr, so I will add your translation to the officially supported languages!

You can also open a new issue and upload the translation file in it. I will then close the issue once your language is included in the program.

Another option is to send a pull request on the project, if you know how to do so. I will then review and merge it.
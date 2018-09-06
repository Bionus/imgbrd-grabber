---
layout: docs
title: MyImouto
---


# Introduction

First of all, know that using commands can slow down huge batch downloads (a recent computer may need from 100ms to 1s more per image, which is very consequent).



# Install a web server and MyImouto

Please follow the instructions on MyImouto's wiki to install everything:

https://github.com/myimouto/myimouto/wiki/How-to-install



# Install cURL

## Windows
Download cURL from [here](https://curl.haxx.se/dlwiz/?type=bin) (take the 32 or 64 bit version depending on your system). Choose "Generic" and "Any" for the next options. You'll then get a list of packages, you can take the first. This will make you download a 7z file, that, once extracted, will contain the `curl.exe` binary in its `bin` folder. Just copy all the contents of the `bin` folder into Grabber's installation folder, so that `curl.exe` and `Grabber.exe` are in the same place.

## Other operating systems
cURL is very likely already installed on UNIX systems. If it is not, it must be in your package manager with the package name `curl`.



# Grabber settings

Launch Grabber, then open the options window and go to the "Commands" part.

There, you should empty everything. The only value you need to set is for the "Image" field:
```shell
curl -F "user[name]=YOUR_USERNAME" -F "user[password]=YOUR_PASSWORD" -F "post[tags]=%all:includenamespace,separator= ,unsafe%" -F "post[rating]=%rating%" -F "post[file]=@%path:nobackslash%" http://localhost:3000/post/create
```

Make sure to replace the placeholders by your actual username and password.

Now, every time you save an image, a copy will be created in your local booru!
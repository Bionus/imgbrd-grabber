---
layout: docs
title: Szurubooru
---



**⚠️ Using commands can slow down huge batch downloads (a recent computer may need from 100ms to 1s more per image) ⚠️**



## Szurubooru

### Install
Follow the official [INSTALL.md](https://github.com/rr-/szurubooru/blob/master/doc/INSTALL.md) documentation from the Szurubooru repository.
Note that you'll need to have [Docker](https://docs.docker.com/get-docker/) installed.

Don't forget to update the `MOUNT_` variables in the `.env` file.

If you're having issues setting it up (for example with filesystem permissions), there is a [Common errors](https://github.com/rr-/szurubooru/wiki/Common-errors) page in their wiki that might be of help for troubleshooting.


### Configuration

* Create an account on the [registration page](http://localhost:8080/register), you should automatically be an administrator
* Create tag categories in the [tag categories page](http://localhost:8080/tag-categories), it is recommended to at least add the "character", "copyright", "artist", and "meta" categories, as well as renaming "default" to "general"
* Create a token with a very late expiration date in "Account > Login tokens"




## Grabber

### Install NodeJS

You need NodeJS to be installed on your machine to use the upload script used by Grabber.
You can download it from [their website](https://nodejs.org/en/download/), or from a package manager [here](https://nodejs.org/en/download/package-manager/).


### Download the upload script

Download the [szurubooru.js](szurubooru.js) file into Grabber's installation folder.

ℹ️ If your Szurubooru instance is not on the same machine as Grabber, or simply not accessible at `http://localhost:8080/`, make sure to update the URL in the script.


### Install NodeJS global packages

This script uses the NodeJS "axios" and "form-data" plugins, so you can install them with:
```
npm install -g axios form-data
```

Make sure the `NODE_PATH` environment variable is properly set to point to your global node_modules folder. On Windows, it's usually:
```
C:\Users\%USERNAME%\AppData\Roaming\npm\node_modules
```

But you can check the exact path with:
```
npm root -g
```


### Configuration

Open Grabber, then go to "Options > Commands", and set the "Image" field to:
```
node szurubooru.js "YOUR_USERNAME" "YOUR_TOKEN" "%all:includenamespace,unsafe,underscores%" "%rating%" "%source:raw%" "%path:nobackslash%"
```

Make sure to replace `YOUR_USERNAME` by your Szurubooru username, and `YOUR_TOKEN` by the token created earlier (in the `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx` format or similar).

This command will be run every time an image is saved, causing it to also be sent to your Szurubooru instance!
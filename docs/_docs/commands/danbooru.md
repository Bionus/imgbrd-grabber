---
layout: docs
title: Danbooru
---



**⚠️ Using commands can slow down huge batch downloads (a recent computer may need from 100ms to 1s more per image) ⚠️**



# Danbooru

## Install
Follow the quickstart in the official [README.md](https://github.com/danbooru/danbooru/blob/master/README.md) documentation from the Danbooru repository.
Note that you'll need to have [Docker](https://docs.docker.com/get-docker/) installed.

It pretty much only amounts to downloading the `docker-compose.yaml` file and doing `docker-compose up`.


## Configuration

* Create an account on the [registration page](http://localhost:3000/users/new), you should automatically be an owner
* Create an API key in "Account > API key"




# Grabber

## Install NodeJS

You need NodeJS to be installed on your maching to use the upload script used by Grabber.
You can download it from [their website](https://nodejs.org/en/download/), or from a package manager [here](https://nodejs.org/en/download/package-manager/).


## Download the upload script

Download the [danbooru.js](danbooru.js) file into Grabber's installation folder.


## Install NodeJS global packages

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


## Configuration

Open Grabber, then go to "Options > Commands", and set the "Image" field to:
```
node danbooru.js "YOUR_USERNAME" "YOUR_API_KEY" "%all:includenamespace,unsafe,underscores%" "%rating%" "%source:unsafe%" "%path:nobackslash%"
```

Make sure to replace `YOUR_USERNAME` by your Danbooru username, and `YOUR_API_KEY` by the API key created earlier.

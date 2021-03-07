---
layout: docs
title: Szurubooru
---



**⚠️ Using commands can slow down huge batch downloads (a recent computer may need from 100ms to 1s more per image) ⚠️**



# Szurubooru

ℹ️ Note that this is inspired by the official [INSTALL.md](https://github.com/rr-/szurubooru/blob/master/doc/INSTALL.md) file, so feel free to refer to the official install documentation instead.


## Install Docker

You need Docker to be installed on your maching to use Szurubooru. Setup instructions can be found here for Windows: https://docs.docker.com/docker-for-windows/install/


## Download Szurubooru

* Download [the Git archive](https://github.com/rr-/szurubooru/archive/master.zip) and unzip it somewhere
* Open Docker settings, and in "Resources > File sharing", add the Szurubooru directory
* Copy `server/config.yaml.dist` to `server/config.yaml`
* Copy `doc/example.env` to `.env` (at the root directory)


## Setup the data directory

Create a `data` directory somewhere (for example, at the root directory of Szurubooru). Note that this will be where all the images are stored!

Set the `MOUNT_DATA` environment variable to point to the `data` directory (on Windows: right click "This computer > Properties > Advanced system settings > Environment Variables")


## Run

* Open a terminal in the Szurubooru directory
* Run `docker-compose pull`
* Run `docker-compose up -d`

You're done! You should be able to access Szurubooru in your browser at [http://localhost:8080/](http://localhost:8080/)


## Configuration

* Create an account on the [registration page](http://localhost:8080/register), you should automatically be an administrator
* Create tag categories in the [tag categories page](http://localhost:8080/tag-categories) (it is recommended to at least add the "character", "copyright", and "artist" categories)
* Create a token with a very late expiration date in "Account > Login tokens"




# Grabber

## Install NodeJS

You need NodeJS to be installed on your maching to use the upload script used by Grabber.
Just run the installer you can find on their website: https://nodejs.org/en/

On Linux, you can just do:
```
sudo apt-get install nodejs
```


## Download the upload script

Download the [szurubooru.js](szurubooru.js) file into Grabber's installation folder.

Note that this script uses the NodeJS "axios" and "form-data" plugins, so install them with:
```
npm install -g axios form-data
```

Make sure the NODE_PATH environment variable is properly set to point to your global node_modules folder. On Windows, it's usually:
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
node szurubooru.js "YOUR_USERNAME" "YOUR_TOKEN" "%all:includenamespace,unsafe%" "%rating%" "%source:unsafe%" "%path%"
```

Make sure to replace `YOUR_USERNAME` by your Szurubooru username, and `YOUR_TOKEN` by the token created earlier (in the `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx` format or similar).
---
layout: docs
title: Shimmie
---


!!! warning

    Using commands can slow down huge batch downloads (a recent computer may need from 100ms to 1s more per image)

_Thanks a lot to [Siegen](https://github.com/Siegen) for his integration of Shimmie and upload script._



## Shimmie

### Install
Follow the official [Docker installation](https://github.com/shish/shimmie2/wiki/Docker) documentation from the Shimmie repository.
Note that you'll need to have [Docker](https://docs.docker.com/get-docker/) installed.
If you prefer a more on-hands installation without Docker, they also have an [on-disk installation](https://github.com/shish/shimmie2/wiki/Install) documentation you can follow instead.


### Configuration

* The first time you visit [the site](http://localhost:8000/), it will ask you for database credentials, you can just switch the "Type" to SQLite if you don't know
* Create an account on the [registration page](http://localhost:8000/index.php?q=user_admin/create), you should automatically be an administrator
* Go to the [setup page](http://localhost:8000/index.php?q=setup), and edit settings as needed (it's recommended to at least increase the "Max size per file" setting to a higher value)




## Grabber

### Install PHP

You need PHP to be installed on your machine to use the upload script used by Grabber.
You can find installation instructions on [their website](https://www.php.net/manual/en/install.php).

Once installed, make sure that the cURL extension is enabled as the script requires it. It can be done by editing the `php.ini` file in your PHP installation, replacing `;extension=curl` by `extension=curl` (without the leading semicolon). Otherwise, you might encounter this error:

> Call to undefined function curl_init()


### Download the upload script

1. Download the [UploadToShimmie.php](UploadToShimmie.php) file somewhere you can edit it.
2. Edit the top part of the file, especially `$shimmieUser` and `$shimmiePassword`.
3. Move the file into Grabber's installation folder.


### Configuration

Open Grabber, then go to "Options > Commands", and set the "Image" field to:
```bash
php "UploadToShimmie.php" -h="%md5%" -f="%path%" -t="%all:includenamespace,underscores,unsafe,separator=;%" -s="%website%" -r="%rating%"
```

!!! note

    If you did not install PHP globally, you might need to change `php` into its exact path, like `C:\tools\php82\php.exe` (for a [Chocolatey](https://chocolatey.org/) install on Windows).

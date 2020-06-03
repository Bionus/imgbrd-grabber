---
layout: docs
title: Shimmie
---


_Thanks a lot to [Siegen](https://github.com/Siegen) for his integration of Shimmie and upload script._



# Introduction

First of all, know that using commands can slow down huge batch downloads (a recent computer may need from 100ms to 1s more per image, which is very consequent).



# Installation

## Local web server

First, we're going to install WAMP, which contains everithing required to have a local web server.

1. Download the latest version of WampServer here (2.2e as I am writing this): http://sourceforge.net/projects/wampserver/
2. Install it somewhere that doesn’t requires admin rights to write into (for example, the default location c:\wamp)
3. At the end of the installation, the installer may ask for two things.
  1. First, he will ask you what your favorite browser is. You can leave it to the default value which will be Internet Explorer, or change it to another browser (Firefox, Chrome, etc.).
  2. Finally, he will ask some email settings. Leave the default values, you don’t care about that.
4. Now that WampServer is installed, you should see a little green W in your system tray, near the clock (you may need to expand the tray to see it).
5. Congratulations! You can visit your local server by going to [here](http://localhost/)!


## Shimmie

1. Download the latest version of Shimmie [here](https://github.com/shish/shimmie2).
2. Extract it to the www directory in your WampServer installation folder. You can either extract it to a subfolder or not. If you extract it to a subfolder, the site’s url will be http://localhost/subfolder/, and if you don’t it will only be [http://localhost/](http://localhost/). It’s your choice, but if you don’t create a subfolder, you must remember to remove all the files in your www folder first. For the end of this tutorial, I’ll consider that you are using the subfolder "shimmie".
3. Go to [http://localhost/phpmyadmin/](http://localhost/phpmyadmin/). If you have to login, use "root" as username and "" as password (no password).
4. Then, go to the databases tab and create a database named "shimmie" with an "utf8_unicode_ci" encoding.
5. Go to [http://localhost/shimmie/](http://localhost/shimmie/)
6. Enter your database information then click "install" (type: `MySQL`, host: `localhost`, username: `root`, no password, db name: `shimmie`).
7. Hopefully you'll end up at the welcome screen!
8. From there, register a new user with the `admin` username to have your admin account.


# Configuration

## Max upload size

The default max upload size is around 1MB. With these steps, it will increase to around 12MB.

1. Edit the `php.ini` file located in "C:\wamp\bin\php\php5.6.19" (the version number can change depending on which version of WAMP you installed). Note that if there is also a `phpForApache.ini` file, you must edit both.
2. Change `upload_max_filesize = 2M` into `upload_max_filesize = 100M`.
3. Restart WAMP services (left click > restart services).
4. Go to http://localhost/phpmyadmin/
5. Go to the "SQL" tab, and execute
```sql
INSERT INTO `shimmie`.`config` (`name`, `value`) VALUES ('upload_size', '104857600');
```

This will allow you to upload bigger files to Shimmie.

## Grabber

### Basic settings

Enable the "Replace spaces by underscores" setting.

Set the `Commands > Image` setting to
```powershell
"C:\wamp\bin\php\php5.6.19\php.exe" "UploadToShimmie.php" -h="%md5%" -f="%path%" -t="%all:includenamespace,unsafe,separator=;%" -s="%website%" -r="%rating%"
```
*(change the path to PHP according to your installed version in WAMP: just go to "C:\wamp\bin\php" and there should be only one version)*

If you are using MAMP on a Mac, your PHP path will look something like this. Again, remember to change the version number.
`/Applications/MAMP/bin/php/php5.6.19/bin/php`

### Upload script

1. Download the [UploadToShimmie.php](UploadToShimmie.php) file.
2. Edit the contents accordingly if you did not follow the default settings of this tutorial (especially `shimmieUser` and `shimmiePassword`).
3. Move the `UploadToShimmie.php` file to Grabber's installation folder.
---
layout: docs
title: Gelbooru
---


_Thanks a lot to NobodX for his help with the commands system and Gelbooru._



# Introduction #
First of all, know that using commands can slow down huge batch downloads (a recent computer may need from 100ms to 1s more per image, which is very consequent).

_Note: in this wiki, I'll consider that you're using Windows. But on Linux & other OSes, it should be pretty the same (use the cp command instead of copy, LAMP (linux) / MAMP (mac) instead of WAMP, etc.)._



# Installing a local web server #

First, we're going to install WAMP, which contains everithing required to have a local web server.

  1. Download the latest version of WampServer here (2.2e as I am writing this): http://sourceforge.net/projects/wampserver/
  1. Install it somewhere that doesn’t requires admin rights to write into (for example, the default location c:\wamp)
  1. At the end of the installation, the installer may ask for two things.
    1. First, he will ask you what your favorite browser is. You can leave it to the default value which will be Internet Explorer, or change it to another browser (Firefox, Chrome, etc.).
    1. Finally, he will ask some email settings. Leave the default values, you don’t care about that.
  1. Now that WampServer is installed, you should see a little green W in your system tray, near the clock (you may need to expand the tray to see it).
  1. Congratulations! You can visit your local server by going to [here](http://localhost/)!



# Installing Gelbooru #

  1. Download the latest version of Gelbooru here (0.1.11 as I am writing this): [http://gelbooru.com/index.php?page=forum&s=view&id=99](http://gelbooru.com/index.php?page=forum&s=view&id=99)
  1. Extract it to the www directory in your WampServer installation folder. You can either extract it to a subfolder or not. If you extract it to a subfolder, the site’s url will be http://localhost/subfolder/, and if you don’t it will only be [http://localhost/](http://localhost/). It’s your choice, but if you don’t create a subfolder, you must remember to remove all the files in your www folder first. For the end of this tutorial, I’ll consider that you are using the subfolder "gelbooru".
  1. Create a folder named "1" in the "images" folder.
  1. If you are using linux, give the "images", "images/1" and "thumbnails" folders writable permissions.
  1. Then, go to [http://localhost/phpmyadmin/](http://localhost/phpmyadmin/). If you have to login, use "root" as username and "" as password (no password).
  1. Then, go to the databases tab and create a database named "gelbooru" with an "utf8\_unicode\_ci" encoding.
Once done, Go to the "SQL" tab, and put this query in the text area:
```sql
SET GLOBAL log_bin_trust_function_creators = 1;
```
Click "Go".
  1. Then, open the file "config.php" with a plain text editor (I don’t recommend using a WYSIWYG editor such as word).
<table cellpadding='3'><tr><td border='2'><b>Replace the line</b></td><td><b>By</b></td></tr><tr><td><code>$mysql_user = "gelbooru";</code></td><td><code>$mysql_user = "root";</code></td></tr><tr><td><code>$mysql_pass = "test";</code></td><td><code>$mysql_pass = "";</code></td></tr><tr><td><code>$mysql_db = "asdf";</code></td><td><code>$mysql_db = "gelbooru";</code></td></tr><tr><td><code>$site_url = "http://127.0.0.1/branches/0.1.10/0.1.10/";</code></td><td><code>$site_url = "http://localhost/gelbooru";</code></td></tr><tr><td><code>$site_url3 = "Default Booru";</code></td><td><code>$site_url2 = $site_url3 = "Localbooru";</code></td></tr></table>
(you can change "Localbooru" to anything: it will be the name of the booru)
  1. And finally add, just before the "?>" at the end of the file:
```php
$lowerlimit = 20;
$no_cache = false;
$tag_count = 20;
$new_tag_list = 'iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii';
$new_user_list = 'iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii';
$new_cthreshold = 0;
$new_pthreshold = 0;
$new_my_tags = '';
error_reporting(0);
```
You can of course change these values to whatever you want.
  1. Go to [http://localhost/gelbooru/install/](http://localhost/gelbooru/install/) and register your admin account.
  1. If you don’t see any errors the installation went well! As the script suggests, you can now delete the c:\wamp\www\gelbooru\install folder.
  1. Finally, go to phpMyAdmin, click on the "gelbooru" database on the left menu, then click on the "SQL" tab, and put this query in the text area:
```sql
ALTER TABLE `tag_index` ADD PRIMARY KEY (`tag`)
```
Click "Go".
  1. You can now access your booru by going here: http://localhost/gelbooru/
  1. If you still run into errors after adding images, click on the WampServer icon on the system tray, go to "PHP > PHP settings" and uncheck "display errors".




# Grabber #

## 3.2.0 and higher ##
Launch Grabber, then open the options window and go to the "Commands" part. Set it to those values:

* Tag: "" (nothing)
* Image:
```powershell
C:\Windows\System32\cmd.exe /C copy "%path%" "C:\wamp\www\gelbooru\images\1\%md5%.%ext%"
```
(Be careful to put the **blank character** (ALT+255) between "cmd.exe" and "/C". Otherwise, the two string were stuck, and the command is not recognized (at least in version 5.4.2 of Grabber).)

And, in the Database part:

* Driver: `QMYSQL`
* Host: `localhost`
* User: `root`
* Password: leave empty
* Database: `gelbooru`
* Start: leave empty
* Image:
```sql
INSERT INTO posts(creation_date, hash, image, height, width, ext, rating, tags, directory, active_date) VALUES(NOW(), "%md5%", "%md5%.%ext%", %height%, %width%, ".%ext%", "%rating%", " %allo% ", "1", "20110619");
```
* Tag:
```sql
INSERT INTO tag_index(tag, index_count) VALUES("%tag%", 1) ON DUPLICATE KEY UPDATE index_count = index_count+1;
```
* End:
```sql
UPDATE post_count SET pcount = (SELECT COUNT(*) FROM posts), last_update = (CURDATE() + 0) WHERE access_key = "posts";
```

Now, every time you save an image, a copy will be created in your local booru!

## Older versions ##
Launch Grabber, then open the options window and go to the "Commands" part. Set it to those values:

Initialization (note that the mysql version may vary)
```powershell
"C:\wamp\bin\mysql\mysql5.5.8\bin\mysql.exe" --user=root --password= gelbooru
```
Image (with initialization)
```sql
INSERT INTO posts(creation_date, hash, image, height, width, ext, rating, tags, directory, active_date) VALUES(NOW(), "%md5%", "%md5%.%ext%", %height%, %width%, ".%ext%", "%rating%", " %all% ", "1", "20110619"); UPDATE post_count SET pcount=pcount+1 WHERE access_key="posts";
```
Tag (with initialization)
```sql
INSERT INTO tag_index(tag, index_count) VALUES("%tag%", 1) ON DUPLICATE KEY UPDATE index_count = index_count+1;
```
Image
```powershell
C:\Windows\System32\cmd.exe /C copy "%path%" "C:\wamp\www\gelbooru\images\1\%md5%.%ext%"
```

Now, every time you save an image, a copy will be created in your local booru!



# Troobleshooting #

## Thumbnails ##

If you test, you'll see that it doesn't works totally. In fact, no thumbnail is generated when doing this. Just visit [http://localhost/gelbooru/thumbs\_fix.php](http://localhost/gelbooru/thumbs_fix.php) to fix this. Note that you must be logged in as administrator in gelbooru to do this.

## Searches with short tags ##

By default, the minimal size for tags is 4. You may want to change this behavior to allow the search of shorter tags (some popular tags are only one character long, such as "v").
To do this, open your "my.ini" file by clicking on the Wamp icon in the system tray, then "MySQL > my.ini".
Search for "[mysqld](mysqld.md)" in the file, and just under, add "ft\_min\_word\_len=1" on a new line (1 is to allow one character long tags, you can set it to whatever you want, by default it is 4). Same thing for "[myisamchk](myisamchk.md)".
The end of your file should now look like this (there can be minor differences):
```ini
[myisamchk]
ft_min_word_len=1
key_buffer = 20M
sort_buffer_size = 20M
read_buffer = 2M
write_buffer = 2M

[mysqlhotcopy]
interactive-timeout

[mysqld]
ft_min_word_len=1
port=3306
```
Then go to phpMyAdmin, in the "gelbooru" database, check all the tables and choose "Repair tables" in the list.
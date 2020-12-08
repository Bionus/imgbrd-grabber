---
title: Configuration files
---


# Configuration files

## Windows

```
C:/Users/%USERNAME%/AppData/Local/Bionus/Grabber
```


# Files

_The list below is **not** sorted by "priority" but alphabetically. All might not exist depending on your specific configuration._

* `cache/`: temporary files for caching things such as HTTP requests
* `sites/`: list of sources and sites (see [Sources](sources.md))
* `themes/`: the installed themes (see [Themes](plugins/theme.md))
* `thumbs/`: the thumbnails for favorite tags
* `webservices/`: the icons for webservices
* `blacklist.txt`: the ignored and blacklisted tags
* `commands.log`: the log of commands (not SQL)
* `commands.sql`: the log of SQL commands
* `favorites.json`: the user's favorites
* `filenamehistory.txt`: history of the "filename" quick edit field in the main window
* `ignore.txt`: the "ignored" tags as set in the user settings
* `main.log`: log of everything that happened in the current (or last) run of the program
* `md5s.txt`: the database of already downloaded files
* `md5s.sqlite`: the database of already downloaded files (beta SQLite format, see [issue #2116](https://github.com/Bionus/imgbrd-grabber/issues/2116))
* `monitors.json`: the user's monitors
* `restore.igl`: batch and single images downloads
* `settings.ini`: contains the settings that are edited in the "options" window (so most of them)
* `tabs.json`: the user's opened tabs
* `viewitlater.txt`: the "kept for later" tags
* `wordsc.txt`: tags automatically added to the auto-complete list, in addition to the `words.txt` file that can be found in the installation folder

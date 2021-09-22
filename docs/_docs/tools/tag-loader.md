---
layout: docs
title: Tag loader
---



# Introduction

Most sources do not provide detailed tags information when listing results. So all the program knows from the results is that some image has tags such as "tag1 tag2 tag3". However, if your filename contains a token such as `%character%`, Grabber will not be able to generate the filename with this information, because it doesn't know what types are all those tags: is "tag1" a character, an artist, some general tag or something else? Since the API doesn't provide this information, Grabber doesn't know.

By default, if there is missing tag type information, the program will have to load the image's information page before starting to download the image. From there, it will parse the tags again, hopefully this time with their type, in order to be able to generate the correct filename. That means that for each image, 2 network calls will be done to the server: first to load the image's details, then to download the actual file.

The goal of the tag loader is to generate a local database to store all tags from a given source, along with their type. This way, even with incomplete tag type information, as long as the program knows the name of the tag, it can match it to its type in the tag database. This way, Grabber will be able to make less requests to the server, saving both download times and reducing the load on the servers.


## Logs

The following log can be a hint that the source you're using would work more efficiently were you to load their tag database first:

```
Not enough information to directly load the image (from blacklist: 0 / from file url: 0 / from filename tags: 1/1)
```

You'll notice three parts in this line:
* `from blacklist: 1`: you have a blacklist set, but the tags could not be loaded from the listing page
* `from file url: 1`: this source only provides the download link for the image on the details page
* `from filename tags: 1/1`: specific tags are used in the filename (like `%character%`) but the image contain some non-typed tags
* `from filename tags: 1/2`: this source only provides some required token (or all) on the details page

Note that only in the `from filename tags: 1/1` case would a  tag database help. In other cases, even with a tag database, the details would still need to be loaded.


## Pre-generated databases

You can find some already generated databases directly on Github:  
<https://github.com/Bionus/imgbrd-grabber/releases/tag-databases>

Note that they are only generated for a handful of sources and might not be totally up to date. However, they can be useful to get started quickly without having to generate one yourselves.


## Command-line interface

Tag databases can also be loaded by CLI, using the `--load-tag-database` switch.  
See [Command-line interface]({{ site.baseurl }}{% link _docs/cli.md %}) for more details.
---
title: FAQ
---


## Frequently Asked Questions

### How to login?

See the [Website login](website-login.md) documentation page.


### How to make the program portable?

See the [Portable version](portable-version.md) documentation page.


### Where are the settings, favorites, etc. stored?

See the [Configuration files](configuration-files.md#location) documentation page.


### How can I filter images by extension?

You can use post-filters to achieve this result, if your source does not naturally support this feature.

The `filetype` or `ext` token could be used (they're aliases of each other, and both contain the extension), for example `ext:png`, or `-ext:webm`.

More info on the [Search](search.md) documentation page.


### Why are multiple tags of the same category (copyright, artist, etc.) merged in the filename?

Tag category have a special setting which decides what the behavior for multiple tags is.

It can be found in "Save > Tags > Category name". Change it to something else than "Replace all tags" if you want a different behavior.


### How to avoid duplicates?

See the [Duplicate files](duplicate-files.md) documentation page.


### Why do I get a message saying my filename is not unique?

This means that different images can end up having the same filename if you are not careful.

Note that the message is a **warning**, and not an error. You are free to ignore it, you might just sometimes have duplicate filenames for your images, which will cause some images' download to be skipped.

For example, the filename `%id%.%ext%` will generate this warning, because an ID in a given source will represent a different image on another one.

See [Duplicate files](duplicate-files.md) for more details.


### Why do my tags have spaces between words instead of underscores?

There's a setting to control which separator is used for words in tags, whether it is a space (` `) or an underscore (`_`).

You can find this setting in "Save > Filename", called "Replace spaces by underscores".

Another option is to use the [underscores](filename.md#underscores-bool) modifier in your filename (such as `%all:underscores%`).
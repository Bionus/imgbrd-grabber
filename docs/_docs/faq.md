---
title: FAQ
---


# Frequently Asked Questions

## How to login?

See the [Website login]({{ site.baseurl }}{% link _docs/website-login.md %}) documentation page.


## How to make the program portable?

See the [Portable version]({{ site.baseurl }}{% link _docs/portable-version.md %}) documentation page.


## Where are the settings, favorites, etc. stored?

```
C:\Users\%USERNAME%\AppData\Local\Bionus\Grabber
```


## How can I filter images by extension?

You can use post-filters to achieve this result, if your source does not naturally support this feature.

The `filetype` or `ext` token could be used (they're aliases of each other, and both contain the extension), for example `ext:png`, or `-ext:webm`.

More info on the [Search]({{ site.baseurl }}{% link _docs/search.md %}) documentation page.


## Why are multiple tags of the same category (copyright, artist, etc.) merged in the filename?

Tag category have a special setting which decides what the behavior for multiple tags is.

It can be found in "Save > Tags > Category name". Change it to something else than "Replace all tags" if you want a different behavior.


## How to avoid duplicates?

In addition to being able to not overwrite existing files, Grabber stores a global MD5 list to ensure you don't download twice the same image in different locations.

You can change this behavior to one of those below, in the "Save" category of the settings:
* **Save**: download and save the image twice (as if it's not a duplicate)
* **Copy**: save the image twice, but copy it from the original image to prevent an useless download
* **Move**: move the original image to the duplicate location (useful for example if you're re-downloading an image that has changed a few tags)
* **Link**: create a shortcut with the duplicate image's name, pointing to the original image (useful if you often have duplicates in different folders, with each folder having an useful meaning)
* **Don't save**: skip duplicate images


## Why do I get an message saying my filename is not unique?

This means that different images can end up having the same filename if you are not careful.

Note that the message is a **warning**, and not an error. You are free to ignore it, you might just sometimes have duplicate filenames for your images.

For example, the filename `%id%.%ext%` will generate this warning, because an ID in a given source will represent a different image on another one.


## Why do my tags have spaces between words instead of underscores?

There's a setting to control which separator is used for words in tags, whether it is a space (` `) or an underscore (`_`).

You can find this setting in "Save > Filename", called "Replace spaces by underscores".

Another option is to use the [underscores]({{ site.baseurl }}{% link _docs/filename.md %}#underscores-bool) modifier in your filename (such as `%all:underscores%`).
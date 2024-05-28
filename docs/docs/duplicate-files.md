---
title: Duplicate files
---


## Already existing files

When a file is downloaded, Grabber will first check if the same file already exists at the destination, using your configured [Filename](filename.md). If a file already exists with the same filename, Grabber will not download the file.

That's why it's important to have a configured filename that generates a different paths for each image. Otherwise, you might have multiple images sharing the same path, and only one will be downloaded.

For example, `%artist%.%ext%` will always generate the same name for different images from the same artist, causing collisions. As a result, only the first image will be downloaded and other ones will be skipped and counted as duplicates.


## The MD5 list

### Introduction

To understand how the MD5 list works, you first have to know what is an MD5. For each image, we can generate a special code, called "MD5". A MD5 looks like `098f6bcd4621d373cade4e832627b4f6` and is unique to each image, as it corresponds to the contents of the image. So the same image, even on different websites, would always have the same MD5. You can learn more about [hashing](https://en.wikipedia.org/wiki/Hash_function) and [MD5](https://en.wikipedia.org/wiki/MD5) on Wikipedia.

Many sources that Grabber supports provide the MD5 of the image, which allows Grabber to know if you already downloaded the same image somewhere else, even if it was from a different website.

### Configuration

So in addition to being able to not overwrite already existing files, Grabber stores a list of the MD5 codes of all images you've downloaded, to ensure you don't download twice the same image from different sources.

There are two settings that control this behavior in Grabber, both located in the "Save" category of the settings:

* **If a file already exists globally**: if a file with the same MD5 has been downloaded by Grabber anywhere on your computer
* **If it's in the same directory**: if a file with the same MD5 has already been downloaded by Grabber in the same target directory as the image you're trying to save

The available options for both of those settings are:

* **Save**: download and save the image twice (as if it were not a duplicate)
* **Copy**: save the image twice, but copy it from the original image to prevent a useless download
* **Move**: move the original image to the duplicate location (useful for example if you're re-downloading an image that has changed a few tags)
* **Shortcut** / **Symbolic link**: create a [shortcut](https://en.wikipedia.org/wiki/Shortcut_(computing)) or [symbolic link](https://en.wikipedia.org/wiki/Symbolic_link) (depending on your platform) with the duplicate image's name, pointing to the original image (useful if you often have duplicates in different folders, with each folder having a useful meaning)
* **Hard link**: creates a [hard link](https://en.wikipedia.org/wiki/Hard_link) with the duplicate image's name, pointing to the original image (similar to a shortcut / symbolic link)
* **Don't save**: skip duplicate images


### Deleted files

By default, if you delete a file from your computer after it was downloaded, it will also be removed from the MD5 list. Which means that if you try to download the same file again, from the same source or from another one, it will be downloaded again.

If you want Grabber to "remember" that you already downloaded this file even if you deleted it, you can enable the "Keep deleted files in the MD5 list" setting in the "Save" category.
---
layout: docs
title: Blacklist fixer
---



# Introduction

The blacklist fixer is a tool used to retro-actively apply a blacklist to already downloaded images.

For example, if you already downloaded a few thousands of images, but forgot to blacklist a certain tag, the blacklist fixer can be used to detect which downloaded images contain tags from the new blacklist, and delete them.



# Limitations

There are few limitations for the blacklist fixer:

* It is unable to extract the tags from the filename or anywhere else, so it will make a request to the server for each image to get its tags
* You can only use one source for fetching an image's tags
* If your filename does not contain the image's MD5, Grabber will re-calculate it, which might be different from the server's image's MD5

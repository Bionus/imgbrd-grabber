---
layout: docs
title: Image renamer
---



# Introduction

The image renamer is a tool used to rename existing images when you change your filename.

For example, if you already downloaded a few thousands of images, but decided to change your filename format afterwards, the image renamer can be used to rename your already downloaded images to the new format.



# Limitations

There are few limitations for the image renamer:

* It is unable to extract information from the filename or anywhere else, so it will make a request to the server for each image to get its tags and other information
* You can only use one source for fetching an image's information
* If your filename does not contain the image's MD5, Grabber will re-calculate it, which might be different from the server's image's MD5

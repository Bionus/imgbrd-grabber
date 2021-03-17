---
title: Metadata
---


# Introduction

You might want to store a post's information not in the downloaded file's filename, but instead inside its metadata. This usually allows better searching, and avoid filesystem filename length limits.



# Windows Property System

Only available on Windows due to the nature of those properties.

Only available on a few extensions by default (JPG / MP4, not PNG / GIF / WEBM). If you want this to work on every extension, you should use something like [FileMeta](https://github.com/Dijji/FileMeta).

Example of commonly used property names (non-exhaustive):
* System.Author
* System.Keywords
* System.Comment

The full list can be found here:  
https://docs.microsoft.com/en-us/windows/win32/properties/props

Here's a basic example of what is possible to do:

![metadata](img/metadata.png)
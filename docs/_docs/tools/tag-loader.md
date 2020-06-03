---
layout: docs
title: Tag loader
---



# Introduction

Most sources do not provide detailed tags information when listing results. So all the program knows from the results is that some image has tags such as "tag1 tag2 tag3". However, if your filename contains a token such as `%character%`, Grabber will not be able to generate the filename with this information, because it doesn't know what types are all those tags: is "tag1" a character, an artist, some general tag or something else? Since the API doesn't provide this information, Grabber doesn't know.

By default, if there is missing tag type information, the program will have to load the image's information page before starting to download the image. From there, it will parse the tags again, hopefully this time with their type, in order to be able to generate the correct filename. That means that for each image, 2 network calls will be done to the server: first to load the image's details, then to download the actual file.

The goal of the tag loader is to generate a local database to store all tags from a given source, along with their type. This way, even with incomplete tag type information, as long as the program knows the name of the tag, it can match it to its type in the tag database. This way, Grabber will be able to make less requests to the server, saving both download times and reducing the load on the servers.

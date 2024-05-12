---
title: Monitors
---


## Introduction

When you never want to miss any image for one of your given searches, you might want to consider using a monitor. Monitors run from time to time in the background, and can either automatically download new images, or simply notify you when new images have appeared.

It works by checking for new images since the last time the monitor ran. It will **not** check images before that. Monitors are used for _incremental_ checks, not full checks. If you need to download all images for a given search, you should use a batch download instead. You could then set up a monitor if you want to download images that might be uploaded after your batch download is done running.


## Adding a monitor

Just perform the search that you want to monitor, then click the "Monitor" button at the bottom of the tab. Note that monitoring big searches might not be very efficient, so it is recommended to monitor more specific searches, like a single artist.


## Handling non-tagged images

Sometimes, images will be posted on your sources without proper tags or information. For example, some sources us a special `tagme` tag to indicate that. To avoid this, you can set a delay on a monitor. If you do so, it will ignore the most recent images and only download images that have been uploaded after the delay you've set.

So instead of checking images between `now` and `last check`, it will check images between `now - delay` and `last check - delay`.

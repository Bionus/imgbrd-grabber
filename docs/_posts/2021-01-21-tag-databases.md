---
title: "Tag databases"
date: 2021-01-21 19:07:52 +0100
---


Hey everyone!

&nbsp;

Tag databases, while documented [on this very website](https://www.bionus.org/imgbrd-grabber/docs/tools/tag-loader.html), is a little-known feature of Grabber.
However, they are very useful, and as such there is ongoing work to make them more easy to use.

<!--more-->

Part of this work is a way to provide already generated tag databases so that users don't need to spend a lot of time and overload servers' tag APIs. The more people use tag databases, the less requests are done on servers, saving both users and servers bandwidth, since the additional request to get tag information is not necessary anymore.

The already generated tag databases can now be found here:  
<https://github.com/Bionus/imgbrd-grabber/releases/tag-databases>

Currently, only one source has been added, selected because to a recent issue where a tag database would have been very useful. But I am open to suggestions for sources where providing such a database would be useful! Feel free to suggest those [on Github](https://github.com/Bionus/imgbrd-grabber/issues) as usual, with a rationale.

Note that those are not bundled with the installer because they can quickly take a lot of size, and not everyone would use those sources. So for those users, they would be a waste of space on their machine.

The next steps include:
* Automatically generating those databases from time to time using [Github actions](https://github.com/Bionus/imgbrd-grabber/actions)
* Automatically downloading them from Grabber

&nbsp;

Best,  
Bionus
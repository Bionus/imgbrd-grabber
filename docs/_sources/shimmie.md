---
title: Shimmie
---


# Shimmie

## Search syntax

<https://shimmie.shishnet.org/ext_doc/index>

In addition:
* `width` + `height` will be automatically converted to `size` if possible
* Range searches (e.g. `meta:A..B`) are automatically converted to two comparisons (e.g. `meta:>=A meta:<=B`)
* `date:` is transformed to `posted:`
* `downvote:` is transformed to `downvoted_by:`
* `upvote:` is transformed to `upvoted_by:`
* `tagcount:` is transformed to `tags:`
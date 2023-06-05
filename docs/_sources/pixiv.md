---
title: Pixiv
---


## Pixiv

Whatever URL is entered when adding a site with this source, it will be overriden by:
* <https://app-api.pixiv.net/> (for search / listing)
* <https://public-api.secure.pixiv.net/> (for galleries / details)

### Search syntax

* `tag1 tag2`: basic tag search
* `mode:{partial,full,tc}`: tag search type (default: `partial`)
* `bookmarks:USER_ID`: fetch a given user's bookmarks
* `user:USER_ID`: fetch art from a given user
* `is:followed`: show illustrations from artists you follow
* `type:{illust,manga}`: only load works of a given type (only applies to `user:` and empty searches)

#### Date
The date format is `YYYY-MM-DD`.

* `date:EXACT`: get results from a given date
* `date:>=START`: get results after the given date (inclusive)
* `date:<=END`: get results before the given date (inclusive)
* `date:START..END`: get results between the start and end date

### Login

Pixiv removed username/password authentication.
As such, you need to use OAuth access and refresh tokens directly.

Here's an useful Python script that allows you to do exactly that:  
<https://gist.github.com/upbit/6edda27cb1644e94183291109b8a5fde>
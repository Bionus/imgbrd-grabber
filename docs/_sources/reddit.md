---
title: Reddit
---


# Reddit

The only known working URL for this source is:
* <https://www.reddit.com>

## Search syntax

* `some search`: basic search
* `subreddit:SUBREDDIT`: load results from a specific subreddit
* `user:USERNAME`: load results from a specific user
* `sort:{hot,new,top,rising,relevance,comments}`: sort order for results. `rising` is only valid for listing. `relevance` and `comments` is only valid for search (default: `hot` for listing, `relevance` for search)
* `since:{hour,day,week,month,year,all}`: retrive posts for a given duration, only valid for search or "top" listing (default: `all`)


Search:
/search.json?q=${tag}&sort=${sort}&t=${since}
/r/${subreddit}/search.json?q=${tag}&sort=${sort}&t=${since}&restrict_sr=1

Basic:
/r/${subreddit}/${sort}.json?t=${since}
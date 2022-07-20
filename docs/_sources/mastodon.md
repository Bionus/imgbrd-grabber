---
title: Mastodon
---


# Mastodon

## Search syntax

* Empty search: public timeline
* `search`: search for statuses matching the search (requires login)
* `user_id:ID`: get all media for a given user ID

_Note: getting an user ID on Mastodon is not trivial. You can do it by opening a status from a given user, then opening `https://domain/api/v1/statuses/STATUS_ID`. There, you'll see the user ID in the "account.id" JSON field._
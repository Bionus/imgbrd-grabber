---
title: Mastodon
---


## Mastodon

### Search syntax

* Empty search: public timeline
* `search`: search for statuses matching the search (requires login)
* `user_id:ID`: get all media for a given user ID

!!! note

    Getting an user ID on Mastodon is not trivial. You can do it by opening a status from a given user, then opening `https://domain/api/v1/statuses/STATUS_ID`. There, you'll see the user ID in the "account.id" JSON field.

### Login

You need an OAuth app to login. To do so, you need to have cURL (or equivalent) installed, and run this command:

```bash
curl -X POST -F "client_name=Grabber" -F "redirect_uris=http://127.0.0.1/" https://pawoo.net/api/v1/apps
```

The output will output a `client_id` and `client_secret` that you can use for the OAuth login.
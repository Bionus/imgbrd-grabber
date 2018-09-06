---
title: Sources
---


# Introduction

If your site is not based on Danbooru, Gelbooru, Shimmie, or another already-included source file, it gets more complicated to add it.

**⚠️ You must have knowledge of JavaScript to make your own JavaScript source ⚠️**

# Utils

* [JavaScript model helper](javascript-helper.html)

# Basics

```javascript
__source = {
    name: "Danbooru (2.0)",
    apis: {
        api_id: api,
    },
    auth: {
        auth_id: auth,
    },
};
```

# API

```javascript
{
    name: "JSON", // The name of the API
    auth: [], // The auth required to use this API, multiple values means OR
    maxLimit: 200, // The maximum "images per page" the user can set with this API

    search: {
        url: search_url_function,
        parse: parse_url_function,
    },

    tags: {
        url: tags_url_function,
        parse: tags_parse_function,
    },
}
```

## Search "url" function

**Parameters:**
* query (`{ search: string, page: number }`)
* opts (`{ limit: number, auth: {} }`)
* previous (`{ page: number, minId: number, maxId: number }`)

**Returns:** `string | { url: string, headers?: {}, error?: string }`

### Description
Builds a search API url from a search query.

### Example
```javascript
function(query, opts, previous) {
    return "/posts.json?limit=" + opts.limit + "&page=" + query.page + "&tags=" + query.search;
}
```

## Search "parse" function

**Parameters:**
* src (`string`)

**Returns:** `{ images: {}[], tags?: { id?: number, name: string, count?: number, type?: string, typeId?: number }[], error?: string }`

## Tags "url" function

**Parameters:**
* query (`{ search: string, page: number }`)
* opts (`{ limit: number, auth: {} }`)

**Returns:** `string | { url: string, headers?: {}, error?: string }`

### Description
Builds a tag listing API url from a search query.

### Example
```javascript
function(query, opts) {
    return "/tags.json?limit=" + opts.limit + "&page=" + query.page;
}
```

## Tags "parse" function

**Parameters:**
* src (`string`)

**Returns:** `{ tags?: { id?: number, name: string, count?: number, type?: string, typeId?: number }[], error?: string }`

# Auth

## URL auth

### Example
```javascript
{
    type: "url",
    fields: [
        {
            key: "login",
            type: "username",
        },
        {
            key: "password_hash",
            type: "hash",
            hash: "sha1",
            salt: "choujin-steiner--%value%--",
        },
    ],
}
```

## POST auth

### Example
```javascript
{
    type: "post",
    url: "/session",
    fields: [
        {
            key: "name",
            type: "username",
        },
        {
            key: "password",
            type: "password",
        },
    ],
    check: {
        type: "cookie",
        key: "password_hash",
    },
}
```

# Location

Then, save it to `C:\Users\%USERNAME%\AppData\Local\Bionus\Grabber/sites/Source name/model.js`. You should also put in this directory an `icon.png` file, which is the 16x16 image that will be displayed in the sources window.
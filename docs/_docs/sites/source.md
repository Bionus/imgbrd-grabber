---
title: Sources
---


# Introduction

If your site is not based on Danbooru, Gelbooru, Shimmie, or another already-included source file, it gets more complicated to add it.

**⚠️ You must have knowledge of JavaScript to make your own JavaScript source ⚠️**

**⚠️ Note that all examples below are using TypeScript⚠️**


# Structure

Sources can be found in your settings folder. On Windows, by default, it is `C:\Users\%USERNAME%\AppData\Local\Bionus\Grabber\sites`.

A source is defined by:
* `icon.png`: a PNG icon, usually the favicon of the source (recommended size: 16x16, it will be resized if larger)
* `model.js`: the JS script that decides which URLs to load and how to parse the results into images, usually compiled from a `model.ts` TypeScript file
* `sites.txt`: the list of the sites based on this source, separated by a new line. This file can be ignored and will be created when adding a new site using this source in Grabber

# Utils

The JS environment is not fully fledged. Therefore, there is an helper to do a few things that are not easily possible otherwise: [JavaScript model helper](javascript-helper.html).

Also, you can use the `console` commands as in JavaScript to generate logs in Grabber's console. Supported methods:
* `console.debug` (debug level, not shown by default)
* `console.info` (info level)
* `console.log` (info level)
* `console.warn` (warning level)
* `console.error` (error level)

# Basics

A basic source file simply exports a `source` object, defined by the `ISource` type. It must have a name and at least one API. All other fields are optional.

**Full example**
```typescript
export const source: ISource = {
    name: "New source", // The name of the source
    modifiers: [], // Allowed "modifiers" on this source
    forcedTokens: [], // Filename tokens that should trigger a full details load
    tagFormat,
    searchFormat,
    auth: {
        auth_id: auth, // The auth ID can be used to reference this auth method
    }
    apis: {
        api_id: api, // The API ID can be used to reference this API
    },
};
```

**Minimal example**
```typescript
export const source: ISource = {
    name: "New source",
    apis: {
        api_id: api,
    },
};
```

# API

A source can have one or more APIs. It must have a name, an `auth` array (which can be empty), and the `search` API.

```typescript
{
    name: "JSON", // The name of the API
    auth: [], // The ID of the auth required to use this API, multiple values means OR
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

**Returns:** (either)
* `string`: an URL
* `{ url: string, headers?: { [key: string]: string }`: an URL and headers
* `{ error?: string }`: an error

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

**Returns:** (either)
* `string`: an URL
* `{ url: string, headers?: { [key: string]: string }`: an URL and headers
* `{ error?: string }`: an error

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

The credentials are passed directly in the URL of each request.

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

## GET and POST auth

The credentials are sent to a given URL on the website to login before doing any request.

### Example
```javascript
{
    type: "post", // Either "get" or "post"
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

## OAuth 2 auth

### Example
```javascript
{
    type: "oauth2",
    authType: "password", // Either "password", "client_credentials", or "header_basic"
    tokenUrl: "/auth/token",
    requestUrl: "/auth/request", // Optional
    refreshTokenUrl: "/auth/refresh", // Optional
    scope: ["user"], // Optional
}
```

# Tag format
A tag format is defined by two fields:
* `case`: the word casing:
  * `lower`: some tag
  * `upper_first`: Some tag
  * `upper`: Some Tag
  * `caps`: SOME TAG
* `wordSeparator`: the separator to add between each word

### Example
```typescript
{
    case: "lower",
    wordSeparator: "_",
}
```
Will have tags looking like "some_tag".

# Search format
A search format must define at least a `and` operator. An operator is either:
* A string such as ` && `
* An object of type `{ separator: string, prefix?: string }`, where the separator is what should be added between both operands, and the prefix what should be added before each operand

If the search also support the OR operator, you must then define `or`, `parenthesis`, and `precedence`:
* `or`: same as `and`, either a string or an object
* `parenthesis`: a boolean to say whether this source supports parenthesis
* `precedence`: which operator has precedence over the other, either `or` or `and`

### Example
```typescript
{
    and: " ",
    or: {
        separator: " ",
        prefix: "~",
    },
    parenthesis: false,
    precedence: "or",
}
```
Will convert the logical search `(A | B) & C` into the text "~A ~B C".
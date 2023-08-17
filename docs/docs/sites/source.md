---
title: Sources
---


## Introduction

If your site is not based on Danbooru, Gelbooru, Shimmie, or another already-included source file, it gets more complicated to add it.

!!! warning

    You must have knowledge of JavaScript to make your own JavaScript source.

!!! note

    All examples below are using TypeScript.

!!! note

    All types described in here are documented in depth in [types.d.ts](https://github.com/Bionus/imgbrd-grabber/blob/develop/src/sites/types.d.ts). Make sure to check it out if you're confused about the usage of a given type.

This page is more about providing actual examples and explaining the flow of creating a source in a more "natural" way, rather than being an exhaustive documentation on types.

To have a tutorial following the step-by-step creation of a real source as an example, see [Source example](source-example.md).


## Structure

Sources can be found in your settings folder. On Windows, by default, it is `C:\Users\%USERNAME%\AppData\Local\Bionus\Grabber\sites`.

A source is defined by a folder containing three files:

* `icon.png`: a PNG icon, usually the favicon of the source (recommended size: 16x16, it will be resized if larger)
* `model.js`: the JS script that decides which URLs to load and how to parse the results into images (usually compiled from a `model.ts` TypeScript file)
* `supported.txt`: the list of supported  sites based on this source, separated by a new line
* `sites.txt`: the list of default sites to add when installing this source, separated by a new line. This file can be ignored and will be created when adding a new site using this source in Grabber


## Utils

The JS environment used by Grabber is not fully fledged. Therefore, there is an helper to help do a few things that are not easily possible otherwise: [JavaScript model helper](javascript-helper.md).

Also, note that you can use the `console` commands as in JavaScript to generate logs in Grabber's console. Supported methods:

* `console.debug` (debug level, not shown by default)
* `console.info` (info level)
* `console.log` (info level)
* `console.warn` (warning level)
* `console.error` (error level)


## Basics

A basic source file simply exports a `source` object, defined by the `ISource` type. It must have a name and at least one API. All other fields are optional.

**Example**
```typescript
export const source: ISource = {
    name: "New source",
    apis: {
        api_id: api,
    },
};
```

## API

A source can have one or more APIs, defined by the `IApi` type. It must have a name, an `auth` array (which can be empty), and the `search` endpoint description.

Most sources have only one API. Examples are API types are "JSON", "XML", "RSS", or even "HTML". HTML is harder to parse and prone to breaking, but it's sometimes the only option for sources that don't provide a public API.

**Example**
```typescript
const api = {
    name: "JSON", // The name of the API
    auth: [], // The ID of the auth required to use this API, multiple values means OR

    search: {
        url: search_url_function,
        parse: parse_url_function,
    },
}
```

### Search "url" function


The goal of this function is to return the URL that lists the images for a given search on that source.

The main parameter, `query`, contains a `search` string containing the user's input, as well as a `page` number containing the page number the user wants to see.

From this, the function must return a string representing the URL to load. Note that you shouldn't include the base URL / domain in that link. It's recommended to use a absolute path instead.

**Example**
```javascript
function(query) {
    return "/posts.json?page=" + query.page + "&tags=" + encodeURIComponent(query.search);
}
```

### Search "parse" function

**Parameters:**

* src (`string`)

**Returns:** `{ images: {}[], tags?: { id?: number, name: string, count?: number, type?: string, typeId?: number }[], error?: string }`

### Tags "url" function

**Parameters:**

* query (`{ search: string, page: number }`)
* opts (`{ limit: number, auth: {} }`)

**Returns:** (either)

* `string`: an URL
* `{ url: string, headers?: { [key: string]: string }`: an URL and headers
* `{ error?: string }`: an error

#### Description
Builds a tag listing API url from a search query.

#### Example
```javascript
function(query, opts) {
    return "/tags.json?limit=" + opts.limit + "&page=" + query.page;
}
```

### Tags "parse" function

**Parameters:**

* src (`string`)

**Returns:** `{ tags?: { id?: number, name: string, count?: number, type?: string, typeId?: number }[], error?: string }`

## Auth

### URL auth

The credentials are passed directly in the URL of each request.

#### Example
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

### GET and POST auth

The credentials are sent to a given URL on the website to login before doing any request.

#### Example
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

### OAuth 2 auth

#### Example
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

## Tag format
A tag format is defined by two fields:

* `case`: the word casing:
    * `lower`: some tag
    * `upper_first`: Some tag
    * `upper`: Some Tag
    * `caps`: SOME TAG
* `wordSeparator`: the separator to add between each word

#### Example
```typescript
{
    case: "lower",
    wordSeparator: "_",
}
```
Will have tags looking like "some_tag".

## Search format
A search format must define at least a `and` operator. An operator is either:

* A string such as ` && `
* An object of type `{ separator: string, prefix?: string }`, where the separator is what should be added between both operands, and the prefix what should be added before each operand

If the search also support the OR operator, you must then define `or`, `parenthesis`, and `precedence`:

* `or`: same as `and`, either a string or an object
* `parenthesis`: a boolean to say whether this source supports parenthesis
* `precedence`: which operator has precedence over the other, either `or` or `and`

#### Example
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
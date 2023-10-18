---
title: Source example
---


## Introduction

The goal of this page is to provide a step-by-step example on how to create a new source, in this case Danbooru.

!!! warning

    It is strongly recommended to have read the [Source](source.md) docs first and to have [types.d.ts](https://github.com/Bionus/imgbrd-grabber/blob/develop/src/sites/types.d.ts) open.


### TypeScript development environment

All sources in Grabber are coded in TypeScript. So before going further, it is recommended to set it up first. For that, you'll first need to install [NodeJS](https://nodejs.org/en/).

Then, download Grabber source code somewhere, either via git or via Github "download code as ZIP" feature.

Once done, run `npm install` in the "src/sites/" directory to install the dependencies. Then, you can build all sources with `npm run build`. You can also run the linter to check for invalid coding style issues with `npm run check`.

!!! note

    VS Code is the recommended light IDE for editing sources, but any other can do the trick.


## Structure

First, let's create a new directory in `C:\Users\%USERNAME%\AppData\Local\Bionus\Grabber\sites`, named "Danbooru".

In there, let's create a `sites.txt` file, containing a single line, "danbooru.donmai.us".

In addition, we download Danbooru's favicon. Most of the times, it can be found in "/favicon.ico" on the server. In other cases, you can find out the correct link by looking for "shortcut icon" in any page's source code.

This favicon being a .ico file, we need to convert it to PNG. If you have an image editor that can do that, go ahead. Otherwise, there's lots of tools online providing this service, that can be found by searching "ico to png online" on a search engine. Rename the final file to "icon.png" and you're done.

Then, the last step is to create a "model.ts" file that will hold our source code. You can leave it empty for now. Running `npm run build` should generate a corresponding "model.js" file. Open the TS file in an editor, and let's start the actual coding.


## Basics

First, we create a basic source, with a name and no API for now.

```typescript
export const source: ISource = {
    name: "Danbooru",
    apis: {},
};
```

By checking in [types.d.ts](https://github.com/Bionus/imgbrd-grabber/blob/develop/src/sites/types.d.ts), we see that the `ISource` type actually has a lot more fields. We ignore them for now as they're optional. We'll go back to them later on.


## API

A source must have at least one API. For Danbooru, the most straightforward API is the JSON API, accessible by simply adding ".json" to most endpoints (for example, "/posts.json" instead of "/posts", or "/tags.json" instead of "/tags").

We can introduce it as a separate variable:
```typescript
const api: IApi = {
    name: "JSON",
    auth: [],

    search: {
        url() {},
        parse() {},
    },
}

export const source: ISource = {
    name: "Danbooru",
    apis: {
        json: api,
    },
};
```

Or directly put it inside the source object:
```typescript
export const source: ISource = {
    name: "Danbooru",
    apis: {
        json: {
            name: "JSON",
            auth: [],

            search: {
                url() {},
                parse() {},
            },
        },
    },
};
```

We can see a few things here:
* The `name` field is quite obvious, we just use "JSON" because that's the format used by this API
* The `auth` field is left as an empty array, because even anonymous users can use this API, no authentication is required
* The `search` object contains two functions, `url()` and `parse()` that we will now discuss in more detail


### Search "url" function

This function returns the URL that lists the image on that API. As we saw at the beginning, that would be "/posts.json" for Danbooru.

The [types.d.ts](https://github.com/Bionus/imgbrd-grabber/blob/develop/src/sites/types.d.ts) file also explains which parameters are passed to this function. In our case, we only need the `query` parameter. Each of its member is described more in depth in the `types.d.ts` file, but what's of interest are those two:
```typescript
/**
 * What the user entered in the search field, as-is.
 */
search: string;

/**
 * The page number of this query.
 */
page: number;
```

Let's use those two members to generate a proper URL:
```typescript
url(query: ISearchQuery): string {
    return "/posts.json?page=" + query.page + "&tags=" + encodeURIComponent(query.search);
}
```

Note that we use `encodeURIComponent` for the free input part, as we don't know if the user will enter some special characters that could break the search, such as "&" or "=".


### Search "parse" function

With the URL generated above, Grabber will make an HTTP call. Then, it will pas the output of this call to the "parse" function, whose job is to turn this raw output into a list of images.

In our case, the raw output is made of JSON. JavaScript (and by extension TypeScript) provides a convenient way to parse JSON, namely [JSON.parse()](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/JSON/parse).

Let's use this information to do some basic parsing and map it to the correct image format, defined in [types.d.ts](https://github.com/Bionus/imgbrd-grabber/blob/develop/src/sites/types.d.ts) as the `IImage` type:
```typescript
parse(src: string): IParsedSearch {
    const parsed = JSON.parse(src);

    const images = parsed.map((raw: any): IImage => ({
        file_url: raw.file_url,
        preview_file_url: raw.preview_url,
    }))

    return { images };
}
```

And here you have a basic parsing method! However, we might want to return more than just the thumbnail and file URLs. Let's add a few more fields, taking inspiration from both the JSON output and the `IImage` type:
```typescript
parse(src: string): IParsedSearch {
    const parsed = JSON.parse(src);

    const images = parsed.map((raw: any): IImage => {
        return {
            file_url: raw.file_url,
            preview_file_url: raw.preview_url,
            id: raw.id,
            md5: raw.md5,
            rating: raw.rating,
            tags: raw.tag_string,
            author: raw.uploader_name,
            score: raw.score,
        };
    })

    return { images };
}
```

Of course, there's still more fields we could add, but let's stop here for the scope of this example.


## Authentication

While this particular API does not require authentication, it's pretty common for sources to require being authenticated before being able to access their API. In Danbooru's case, it's also useful to provide authentication because it allows premium users to use their premium features within Grabber (for example to search more than 2 tags at a time).

So how do we do that? First, we have to determine which kind of authentication is required. Usually, for a proper API, the user would need to provide its API key somehow in the request. Or for an HTML-based "API", that would be a more classic login form somewhere.

Grabber supports many types of login, such as OAuth, POST, HTTP Basic, in-url, etc. The exhaustive list can be found in the `IAuth` type.

For this example, there's two ways to authenticate requests to Danbooru. The first is pretty obvious and is the login form. The second can be found in [their documentation](https://danbooru.donmai.us/wiki_pages/help:api), in the "Authentication" part. It says that we can use either HTTP Basic authentication using an API key, or just pass it in the URL.


### Login form

#### Basics
The "login form" authentication type is described in the `IHttpAuth` login type, let's start by writing a basic version:

```typescript
const auth: IAuth = {
    type: "post",
    url: "/session",
    fields: [],
    check: null,
};
```

How do we get those values? Simply go to the login page and inspect the main `<form>` element there. The "method" attribute corresponds to the `type` you want to use (GET or POST, by default and most usually POST), and the "action" attribute corresponds to the `url`.

Here's the HTML of the `<form>` field for reference:
```html
<form
    novalidate="novalidate"
    class="simple_form session"
    action="/session"
    accept-charset="UTF-8"
    method="post"
>
```

Now you'll notice we're still missing the `fields` and `check` members.

#### Auth fields
The fields correspond to the different inputs on the login page. In most cases, that would be an username and password. Sometimes, there's also a few hidden fields on the page (`<input type="hidden">`) so be careful with those as well. Those fields are built using the `IAuthField` type.

In this example, there's two inputs on the page: username and password, which gives us those two fields:

```typescript
[
    {
        id: "pseudo",
        key: "name",
    },
    {
        id: "password",
        key: "password",
        type: "password",
    },
]
```

* `id` corresponds to a certain mapping in Grabber. For usernames, you need to use "pseudo". For passwords, it's the straightforward "password"
* `key` corresponds to the "name" property of the `<input>` field, i.e. the name of the data as sent by the form
* `type` is optional, and allows to customize the type of input on Grabber's side. "password" for example will hide the user input

Here's the HTML of those two fields for reference (the most important being the `<input />` fields):
```html
<div class="input string required session_name">
    <label class="string required" for="session_name"><abbr title="required">*</abbr> Name</label>
    <input class="string required" type="text" name="session[name]" id="session_name" />
</div>
<div class="input password required session_password field_with_hint">
    <label class="password required" for="session_password"><abbr title="required">*</abbr> Password</label>
    <input autocomplete="password" class="password required" type="password" name="session[password]" id="session_password" />
    <span class="hint"><a href="/password_reset">Forgot password?</a></span>
</div>
```

#### Auth check
Once the HTTP call is done, it can be very useful to check whether the login was successful or not. To do so, there's multiple ways to check, described in the `IAuthCheck` type.

For our example, we can use the simplest one, "cookie". Indeed, when logged in, many websites will generate what is called a user session, whose ID will be stored in a cookie. That means that anonymous users don't have said cookie, but logged-in users have it. As such, if we check that this cookie exists, we can know whether the login was successful or not.

We can also check other various cookies, such as a "remember me" cookie or similar. All cookies can be checked using the browser's "Storage" devtool, so it's quite easy to see which ones are specific to a logged in user or not.

Doing this kind of check is quite simple, for example for the cookie "password_hash" on Danbooru:

```typescript
{
    type: "cookie",
    key: "password_hash",
}
```

#### Final result

So here's how our login form auth looks like, now that it's complete (ignored the `api` part for clarity):

```typescript
const auth: IAuth = {
    type: "post",
    url: "/session",
    fields: [
        {
            id: "pseudo",
            key: "name",
        },
        {
            id: "password",
            key: "password",
            type: "password",
        },
    ],
    check: {
        type: "cookie",
        key: "password_hash",
    },
};

export const source: ISource = {
    name: "Danbooru",
    auths: {
        session: auth,
    },
};
```


## Going further

Now the source is properly working, but there's a few additional things that are possible to do in Grabber but not supported. Let's go over them one by one to get a fully detailed source.


### Tag format

This describes how the tags are formatted on the source, described by the `ITagFormat` type. For Danbooru, tags look like "some_tag". It's fully lowercase, with an underscore between individual tags.

As such, the tag format is quite straightforward:
```typescript
export const source: ISource = {
    // ...
    tagFormat: {
        case: "lower",
        wordSeparator: "_",
    },
    // ...
};
```


### Search format

This describes how the searches are built on the source, described by the `SearchFormat` type. For Danbooru, basic searches look quite simple: just separate tags by spaces.

But there's actually a lesser known feature to do "OR" operations, using tilde. "~tag1 ~tag2" will return all images having either tag1 OR tag2.

Both the "or" and "and" members of a search format can either be a simple separator string, or a separator plus a prefix. There also is a `parenthesis` member to say whether parenthesis are supported on this source. Finally, there is a `precedence` member that says which search operator ("or" or "and") have priority over the other. Here, it can be determined by searching "tag1 ~tag2 ~tag3". If it returns results looking like "tag1 (~tag2 ~tag3)" that means the "or" operator has precedence. If it's "(tag1 ~tag2) ~tag3", then it's the "and" operator.

In this case, it gives this result:

```typescript
export const source: ISource = {
    // ...
    searchFormat: {
        and: " ",
        or: {
            separator: " ",
            prefix: "~",
        },
        parenthesis: false,
        precedence: "or",
    },
    // ...
};
```

---
title: Source registries
---


## Introduction

Source registries are JSON files pointing to source files. They are used to add sources easily to Grabber.

They can be hosted anywhere an can point to sources anywhere. The official source registry is hosted in Github, in the [sources-metadata](https://github.com/Bionus/imgbrd-grabber/releases/tag/sources-metadata) release.


## Format

```json
{
    "name": "...",
    "home": "https://...",
    "url": "https://...",
    "sources": [{
        "slug": "...",
        "name": "...",
        "hash": "...",
        "lastCommit": {
            "hash": "...",
            "author": "...",
            "date": "...",
            "subject": "..."
        },
        "defaultSites": ["..."],
        "supportedSites": ["..."]
    }]
}
```

* `name`: the name of the source registry
* `home`: the URL for the homepage
* `url`: the base URL used when loading sources, such as `$url/$slug.zip` returns the source ZIP
* `sources`: the list of sources managed by this registry
    * `slug`: the machine name of this source (will be the name used to load the ZIP file), usually without spaces or special characters
    * `name`: the name of this source (same as `name` in the source model file)
    * `hash`: the SHA256 hash of the `model.js` file
    * `lastCommit`: the last change to this model file
        * `hash`: the commit hash for this change
        * `author`: the author of this change
        * `date`: the date of this change in ISO format
        * `subject`: the subject of this change
    * `defaultSites`: sites that are automatically added when adding this source (`sites.txt` file)
    * `supportedSites`: sites that are supported by this source (`supported.txt` file)

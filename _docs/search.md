---
title: Search
---


# Introduction

The main feature of Grabber is to search images, and as such, many things can impact the results you see.


# Search field

What is written in the main search field will usually be passed as-is to the source, so depending on your source, the search syntax might be different.

You should check on the source which syntax they accept, but in most cases, all you have to do is separate each tag with a space.


# Post-filter

## Introduction
Sometimes, a source does not allow you to use a given syntax, or limits the number of terms you can use.

In this case, the best solution is to use a post-filter, which loads the results from the source using your search, then filter them with another search.

**⚠️ If your post-filter is too specific, all images might get filtered and Grabber will show zero results! ⚠️**

## Syntax
Post-filtering uses the most popular search syntax in boorus, which is:
* Separate tags by spaces to make a "AND" (ex: `tag1 tag2`)
* Prefix a tag or a filter by a caret (`-`) to negate it (ex: `-tag`)
* `field:value` to filter by a token (ex: `rating:safe`), the list of tokens can be found on the [Filename]({{ site.baseurl }}{% link _docs/filename.md %}) page
* `field:>value` to filter by a token value using a comparison (ex: `id:>1000`), you can use any of these operators: `>`, `>=`, `<`, `<=`
* `field:from..to` to filter by a range (ex: `date:2019-09-02..2019-09-07`)


# Blacklist

## Introduction
You can blacklist some tags or combinaison of tags to not see them in the results.

It can be edited in the settings in the "Save > Blacklist" category, or tags can be added to the blacklist by right-clicking on them and selecting "Blacklist".

## Syntax
Each line of the blacklist corresponds to a conbinaison of tags, and use the same syntax as post-filters.

## Example
Given this blacklist:
```
tag1
tag2 tag3
tag4 rating:safe
```

An image will be considered as "blacklisted" if either of those three conditions is met:
* The image contains the "tag1" tag
* The image contains both the "tag2" and "tag3" tags
* The image contains the "tag4" tag and its rating is "safe"
---
title: Filename
---


# Introduction

If you want to download lots of images at the same time and have each of them be downloaded in a different location or having a different filename, you have to first tell the program which filename format to use.

The program has many tokens to use to ensure different and unique filenames, you can find an exhaustive list [below](#available-tokens).

The most basic tokens are `md5` (an image unique identifier across all websites), `id` (an image unique identifier on a given website) and `ext` (the image's extension). Each token should be written between percent signs (`%`) to differentiate it from plain text. So if you want to use the `md5` token in your filename, you have to write `%md5%`.

## Examples

The most basic unique filename is:
```
%md5%.%ext%
```
Which would generate files such as:
* `f7deda9c6934179779f63910d5e8d2dc.png`
* `a75b2e6c0b9247d04ff88c0aa02bb17a.gif`

A more complex one could be:
```
%artist%/%copyright%/%character%/%md5%.%ext%
```
Which would generate files such as:
* `namu (ratias7777)/sword art online/leafa/362333f4fb4c458577ee72919cfbe10d.png`
* `moneti (daifuku)/touhou/flandre scarlet/0498c78f67d5545bea731d4b1051096c.jpg`
* `moneti (daifuku)/touhou/izayoi sakuya/fe2e6eb294f33f455d43b3effcf4ec6c.jpg`

Note that here, each slash (`/`) corresponds to a different directory:
```
├── namu (ratias7777)
│   └── sword art online
│       └── leafa
│           └── 362333f4fb4c458577ee72919cfbe10d.png
└── moneti (daifuku)
    └── touhou
        ├── flandre scarlet
        │   └── 0498c78f67d5545bea731d4b1051096c.jpg
        └── izayoi sakuya
            └── fe2e6eb294f33f455d43b3effcf4ec6c.jpg
```

# Available tokens

* `ext`: the file extension.
* `artist`
* `copyright`
* `character`
* `model`: boorus such as [Behoimi](http://behoimi.org/) use, adding to classical type, the "model" type.
* `general`
* `search`: the search that returned this image (as is).
* `search_1`, `search_2`, `search_3`...: first/second/third... tag of the search.
* `filename`: image filename, as stored on the booru's server.
* `rating`: `safe`, `questionable`, `explicit`, or `unknown`.
* `score`
* `md5`: the file's MD5 hash. If not returned by the server, it is generated automatically.
* `id`
* `website`: the website's URL (without HTTP and trailing slash).
* `websitename`: the website name as set in the sources' settings.
* `height`: the image's height.
* `width`: the image's width.
* `mpixels`: the image's height multiplied by the image's width.
* `author`: the name of the author of the image.
* `authorid`: the ID of the author of the image.
* `date`: the date the image was posted on the server, using the format "MM-dd-yyyy HH.mm".
* `all`: all tags.
* `allo`: do not use this unless you know what you're doing. It is the same as `%all%`, but without the removal of forbidden characters and any other kind treatment (using it as a filename may cause some images to not save).
* `source`: the source of this file, usually an URL.
* `sources`: all sources for this file, usually a list of URLs.
* `url_file`: the URL of that file.
* `url_original`: the URL of that file's original.
* `url_sample`: the URL of that file's sample.
* `url_thumbnail`: the URL of that file's thumbnail.
* `url_page`: the URL of the details page of that file.
* `count`: counts the current image downloaded during a batch download. Useful to have unique counters per batchs.
* `num`: counter that goes up for every file with the same name. Useful to prevent overwriting already existing files.


# Options

## Introduction
Most tokens can have options. To add options to a token, just add `:` after its name, followed by the option name and its value (separated by a `=`). Multiple options can be set, separating them by `,`.

When a same option is set multiple times, only the latest one is taken into account.

The value can be omitted for boolean options. In this case, `option_name` will be equivalent to `option_name=true`. Useful to set multiple flags.

Example: `%token:option1=value1,option2%`

## Common options
### Max length (int)
If the contents of the token is longer than this value, it will be cut.

Example:
```
%md5%
37d49104a49a3c27d3fabfd7e8c24f97

%md5:maxlength=8%
37d49104
```

### Unsafe (bool)
Disable removal of unsafe characters. Don't use it in normal filenames, as they won't save on Windows if they contain special characters.

Example:
```
%copyright%
fate_stay_night

%copyright:unsafe%
fate/stay_night
```

### Underscores (bool)
If the "replace spaces by underscores" setting is not enabled globally, you can enable for a given token using this option.

Example:
```
%copyright%
fate stay night

%copyright:underscores%
fate_stay_night
```

### Spaces (bool)
If the "replace spaces by underscores" setting is enabled globally, you can disable for a given token using this option.

Example:
```
%copyright%
fate_stay_night

%copyright:spaces%
fate stay night
```

### SQL escape (bool)
Escapes unsafe SQL characters. Useful for SQL commands. Note that it encloses strings in single quotes.

Example:
```
%all%
let's_try \o/

%all:escape%
'let''s_try \\o/'
```

### HTML escape (bool)
Escapes unsafe HTML characters.

Example:
```
%all%
>_<

%all:htmlescape%
&gt;_&lt;
```

## Numbers
### Length (int)
Length of the left-pad of the number.

Example:
```
%count%.%ext%
7.jpg

%count:length=3%.%ext%
007.jpg
```

## Lists
### Separator (string)
The separator to use in a tag list.

Example:
```
%character%
character1 character2

%character:separator=---%
character1---character2
```

### Count (boolean)
If enabled, the token will be replace by the number of values in the list.

Example:
```
%character%
character1 character2

%character:count%
2
```

### Sort (boolean)
If enabled, the list will be sorted alphabetically.

Example:
```
%character%
artist1 tag1 character1 tag2

%character:sort%
artist1 character1 tag1 tag2
```

## Tag lists
### Include namespace (boolean)
If enabled, the namespace of the tags will be included in the result. Better used with the `usafe` option to keep the `:`.

Example:
```
%all%
artist1 tag1 character1 tag2

%all:includenamespace%
artist_artist1 general_tag1 character_character1 general_tag2

%all:includenamespace,unsafe%
artist:artist1 general:tag1 character:character1 general:tag2
```

### Exclude namespace (string list)
The list of namespaces you don't want to see when using the "include namespace" option.

Example:
```
%all%
artist1 tag1 character1 tag2

%all:includenamespace,unsafe%
artist:artist1 general:tag1 character:character1 general:tag2

%all:includenamespace,excludenamespace=general,unsafe%
artist:artist1 tag1 character:character1 tag2

%all:includenamespace,excludenamespace=general character,unsafe%
artist:artist1 tag1 character1 tag2
```

### Ignore namespace (string list)
The list of namespaces you don't want to see in the results.

Example:
```
%all%
artist1 tag1 character1 tag2

%all:ignorenamespace=character%
artist1 tag1 tag2
```

## Dates
### Format
Beware of not using any character forbidden by your operating system in the format, or images won't save (so you cannot use `:` on Windows for example).

| Expression      | Output      |
| --------------- | ----------- |
| d | the day as number without a leading zero (1 to 31) |
| dd | the day as number with a leading zero (01 to 31) |
| ddd | the abbreviated localized day name |
| dddd | the long localized day name |
| M | the month as number without a leading zero (1-12) |
| MM | the month as number with a leading zero (01-12) |
| MMM | the abbreviated localized month name |
| MMMM | the long localized month name |
| yy | the year as two digit number (00-99) |
| yyyy | the year as four digit number |
| h | the hour without a leading zero (0 to 23 or 1 to 12 if AM/PM display) |
| hh | the hour with a leading zero (00 to 23 or 01 to 12 if AM/PM display) |
| m | the minute without a leading zero (0 to 59) |
| mm | the minute with a leading zero (00 to 59) |
| s | the second without a leading zero (0 to 59) |
| ss | the second with a leading zero (00 to 59) |
| z | the milliseconds without leading zeroes (0 to 999) |
| zzz | the milliseconds with leading zeroes (000 to 999) |
| AP | use AM/PM display. AP will be replaced by either "AM" or "PM". |
| ap | use am/pm display. ap will be replaced by either "am" or "pm". |

All other input characters will be ignored. Any sequence of characters that are enclosed in singlequotes will be treated as text and not be used as an expression. Two consecutive singlequotes `''` are replaced by a singlequote in the output.

Some examples, assumed that the date is 21 May 2001 at 14:13:09:

| Format          | Result        |
| --------------- | ------------- |
| `dd.MM.yyyy`    | 21.05.2001    |
| `ddd MMMM d yy` | Tue May 21 01 |
| `hh:mm:ss.zzz`  | 14:13:09.042  |
| `h:m:s ap`      | 2:13:9 pm     |

Example:
```
%date:format=yyyy-MM-dd%
2001-05-21
```

# Custom tokens
You can also add custom tokens. To do so, go to the options, "save" part and "custom tokens" subpart. Then just click the button "Add a new custom token" and choose a name and tags that will be searched (separate them with spaces). Any tags in this list found in the image will be placed into your token. For example if an image got tags "a", "b", "c", "d" and your tags list for token "test" is "a c e", "%test%" will be replaced by "a c".

_Note: custom tokens with an already used name (such as %character%) will be used first._


# Conditionals
You can use conditionals in your filename, using strict inequality signs (< and >).

*Note: there is currently issues with conditionals inside other conditionals, so it is advised to not imbricate conditionals in your filename.*

## Simple conditionals ##
### Token conditionals ###
If any token found in the conditional does not exist or is empty, nothing into it will be displayed.
Since an example is worth a thousand words:
```
image - <%artist% some text <%nothing% another text> test><<%character% some text> text %nothing%> %md5%.%ext%
```
Assuming the image contains both an artist and a character, it will return:
```
image - %artist% some text  test %md5%.%ext%
```
If the image doesn't have any artist, it will be:
```
image -  %md5%.%ext%
```

### Tag conditionals ###
You can also test directly the presence of a tag surrounding him with quotes. For example:
```
image - <image contains the tag "solo"><"group" is one of the image tags> %md5%.%ext%
```
If the image contains the "solo" tag, it will return:
```
image - image contains the tag solo  %md5%.%ext%
```
If the image contains the "group" tag:
```
image -  group is one of the image tags %md5%.%ext%
```
If, for some obscure reason, it have both:
```
image - image contains the tag solo group is one of the image tags %md5%.%ext%
```
If, it does not have solo nor group:
```
image -   %md5%.%ext%
```

## Inverting conditions
You can invert any condition by prefixing it with an exclamation mark `!` symbol.

It works for tokens:
```
%md5% -<!%artist% no artist>.%ext%
```
As well as tags:
```
<image does not contain the tag !"tag1"> %md5%.%ext%
```

# JavaScript
You can also use JavaScript in your filenames. To do so, you can either use the field in the filename window, that you can open clicking the "..." button near the filename field, or by adding "javascript:" before your script.

For example, if your script is "md5 + '.' + ext", your filename will be "javascript:md5 + '.' + ext".

# Similar tags
Sometimes, you have similar tags in your image, which can be problematic for your filename. For example, if you have an image tagged as both "pokemon" and "pokemon_bw", you might get "crossover" when using `%copyright%`, or "pokemon pokemon_bw", which can become annoying for organization.

In order to fix this, there are a few options.

## The "use shortest if possible" setting
This setting is specific for copyright tags, and can be found in the option window, in the "Save > Tags > Copyright" category, as a checkbox at the bottom of the window.

This feature takes all tags marked as "copyright", and remove all those which are another copyright with a suffix appended. So in our example, "pokemon pokemon_bw" would be simplified as "pokemon", since "pokemon_bw" is "pokemon" + "\_bw"..

## Ignored tags
You can "ignore" tags by right-clicking on them then choosing "ignore". This causes the tag to not be treated as having any particular type, and therefore not appearing, in our example, in `%copyright%`. It will however still be in `%all%`.

---
title: JavaScript helper
---


## Introduction

In a JavaScript model file, you have access to a global `Grabber` helper object, which contains a handful of functions useful in models.

Some of them are simply JavaScript functions, and others are bindings to C++ code, when the JavaScript API is not sufficient, or when Qt can do the job easily.

## C++

### regexMatches

**Parameters:**

* regex (string)
* str (string)

**Returns:** object[]

#### Description
Global match a PCRE regex in a string.

#### Example
```
\d+ (\d+) (?<third>\d+)
```
```javascript
"01 23 45 67 89 01 23 45 67 89"
```
```javascript
[
    {
        "0": "01 23 45",
        "1": "23",
        "2": "45"
        "third": "45"
    },
    {
        "0": "67 89 01",
        "1": "89",
        "2": "01",
        "third": "01"
    },
    {
        "0": "23 45 67",
        "1": "45",
        "2": "67",
        "third": "67"
    }
]
```

### parseXML

**Parameters:**

* str (string)

**Returns:** object

#### Description
Parses the string passed and return the parsed XML tree.

#### Example
```xml
<?xml version="1.0" encoding="UTF-8"?>
<posts type="array">
    <post>
        <id>123</id>
        <name>toto</name>
    </post>
    <post>
        <id attr="456">789</id>
        <name>test</name>
    </post>
</posts>
```
```javascript
{
    "posts": {
        "@attributes": {
            "type": "array"
        },
        "post": [
            {
                "id": {
                    "#text": "123"
                },
                "name": {
                    "#text": "toto"
                }
            },
            {
                "id": {
                    "@attributes": {
                        "attr": "456"
                    },
                    "#text": "789"
                },
                "name": {
                    "#text": "test"
                }
            }
        ]
    }
}
```

### parseHTML

**Parameters:**

* html (string)
* fragment (bool) = false

**Returns:** object (JavascriptHtmlDocument)

#### Description
Parses the string passed and return a parsed HTML node object.

Set `fragment` to true if the HTML to parse is not a whole HTML document but only a part of it.

#### Example
```typescript
const parsed = Grabber.parseHTML(src);
const images = parsed.find("#images-list img");
const inner = images.map((img) => img.innerHTML());
```

### JavascriptHtmlDocument::find

**Parameters:**

* css (string)

**Returns:** object[] (JavascriptHtmlDocument[])

#### Description
Returns a list of nodes matching the passed CSS selectors.

### JavascriptHtmlDocument::tag

**Returns:** string

#### Description
Returns the tag name of the node.

### JavascriptHtmlDocument::outerHTML

**Returns:** string

#### Description
Returns the serialized HTML of the element and its descendants.

### JavascriptHtmlDocument::innerHTML

**Returns:** string

#### Description
Returns the serialized HTML of the element's descendants.

### JavascriptHtmlDocument::innerText

**Returns:** string

#### Description
Returns the rendered text content of an element.

### JavascriptHtmlDocument::path

**Returns:** string[]

#### Description
Returns the list of the tags of the parent nodes.

### JavascriptHtmlDocument::pathIds

**Returns:** string[]

#### Description
Returns the list of the IDs of the parent nodes.

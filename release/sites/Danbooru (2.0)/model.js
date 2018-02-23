function mapFields(data, map) {
    var result = {};
    if (typeof data !== "object") {
        return result;
    }
    for (var to in map) {
        var from = map[to];
        var val = from in data ? data[from] : undefined;
        if (val && typeof val === "object" && ("#text" in val || "@attributes" in val)) {
            val = val["#text"];
        }
        result[to] = val;
    }
    return result;
}

function countToInt(str) {
    var count = str.toLowerCase().trim().replace(",", "");
    if (count.slice(-1) === "k") {
        var withoutK = count.substring(0, count.length - 1).trim();
        count = parseFloat(withoutK, 10) * 1000;
    } else {
        count = parseFloat(count, 10);
    }
    return Math.floor(count);
}

function loginUrl(fields, values) {
    var res = "";
    for (var i in fields) {
        var field = fields[i];
        res += field.key + "=" + values[field.key] + "&";
    }
    return res;
}

function fixPageUrl(url, page, previous) {
    url = url.replace("{page}", page);
    if (previous) {
        url = url.replace("{min}", previous.minId);
        url = url.replace("{max}", previous.maxId);
        url = url.replace("{min-1}", previous.minId - 1);
        url = url.replace("{max-1}", previous.maxId - 1);
        url = url.replace("{min+1}", previous.minId + 1);
        url = url.replace("{max+1}", previous.maxId + 1);
    }
    return url;
}

function pageUrl(page, previous, limit, ifBelow, ifPrev, ifNext) {
    if (page < limit || !previous) {
        return fixPageUrl(ifBelow, page, previous);
    }
    if (previous.page > page) {
        return fixPageUrl(ifPrev, page, previous);
    }
    return fixPageUrl(ifNext, page, previous);
}

function buildImage(data) {
    data["page_url"] = "/posts/" + data["id"];
    return data;
}

var auth = {
    url: {
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
    },
    session: {
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
        }
    },
};

return {
    name: "Danbooru (2.0)",
    apis: {
        json: {
            name: "JSON",
            auth: [],
            maxLimit: 200,
            search: {
                url: function(query, opts, previous) {
                    var loginPart = loginUrl(auth.url.fields, opts["auth"]);
                    var pagePart = pageUrl(query.page, previous, 1000, "{page}", "a{max}", "b{min}");
                    return "/posts.json?" + loginPart + "limit=" + opts.limit + "&page=" + pagePart + "&tags=" + query.search;
                },
                parse: function(src) {
                    var map = {
                        "created_at": "created_at",
                        "status": "status",
                        "source": "source",
                        "has_comments": "has_comments",
                        "file_url": "file_url",
                        "sample_url": "large_file_url",
                        "change": "change",
                        "sample_width": "sample_width",
                        "has_children": "has_children",
                        "preview_url": "preview_file_url",
                        "width": "image_width",
                        "md5": "md5",
                        "preview_width": "preview_width",
                        "sample_height": "sample_height",
                        "parent_id": "parent_id",
                        "height": "image_height",
                        "has_notes": "has_notes",
                        "creator_id": "uploader_id",
                        "file_size": "file_size",
                        "id": "id",
                        "preview_height": "preview_height",
                        "rating": "rating",
                        "tags": "tag_string",
                        "author": "uploader_name",
                        "score": "score",
                        "tags_artist": "tag_string_artist",
                        "tags_character": "tag_string_character",
                        "tags_copyright": "tag_string_copyright",
                        "tags_general": "tag_string_general",
                    };

                    var data = JSON.parse(src);

                    var images = [];
                    for (var i = 0; i < data.length; ++i) {
                        images.push(buildImage(mapFields(data[i], map)));
                    }

                    return { images: images };
                },
            },
        },
        xml: {
            name: "XML",
            auth: [],
            maxLimit: 200,
            search: {
                url: function(query, opts, previous) {
                    var loginPart = loginUrl(auth.url.fields, opts["auth"]);
                    var pagePart = pageUrl(query.page, previous, 1000, "{page}", "a{max}", "b{min}");
                    return "/posts.xml?" + loginPart + "limit=" + opts.limit + "&page=" + pagePart + "&tags=" + query.search;
                },
                parse: function(src) {
                    var map = {
                        "created_at": "created-at",
                        "status": "status",
                        "source": "source",
                        "has_comments": "has-comments",
                        "file_url": "file-url",
                        "sample_url": "large-file-url",
                        "change": "change",
                        "sample_width": "sample-width",
                        "has_children": "has-children",
                        "preview_url": "preview-file-url",
                        "width": "image-width",
                        "md5": "md5",
                        "preview_width": "preview-width",
                        "sample_height": "sample-height",
                        "parent_id": "parent-id",
                        "height": "image-height",
                        "has_notes": "has-notes",
                        "creator_id": "uploader-id",
                        "file_size": "file-size",
                        "id": "id",
                        "preview_height": "preview-height",
                        "rating": "rating",
                        "tags": "tag-string",
                        "author": "uploader-name",
                        "score": "score",
                        "tags_artist": "tag-string-artist",
                        "tags_character": "tag-string-character",
                        "tags_copyright": "tag-string-copyright",
                        "tags_general": "tag-string-general",
                    };

                    var data = Grabber.parseXML(src).posts.post;

                    var images = [];
                    for (var i = 0; i < data.length; ++i) {
                        images.push(buildImage(mapFields(data[i], map)));
                    }

                    return { images: images };
                },
            },
        },
        html: {
            name: "Regex",
            auth: [],
            maxLimit: 200,
            search: {
                url: function(query, opts, previous) {
                    var loginPart = loginUrl(auth.url.fields, opts["auth"]);
                    var pagePart = pageUrl(query.page, previous, 1000, "{page}", "a{max}", "b{min}");
                    return "/posts?" + loginPart + "limit=" + opts.limit + "&page=" + pagePart + "&tags=" + query.search;
                },
                parse: function(src) {
                    var matches = Grabber.regexMatches('<li class="category-(?<type>[^"]+)">(?:\\s*<a class="wiki-link" href="[^"]+">\\?</a>)?\\s*<a class="search-tag"\\s+[^>]*href="[^"]+"[^>]*>(?<tag>[^<]+)</a>\\s*<span class="post-count">(?<count>[^<]+)</span>\\s*</li>', src);
                    var tags = {};
                    for (var i in matches) {
                        var match = matches[i];
                        if (!(match["tag"] in tags)) {
                            tags[match["tag"]] = {
                                tag: match["tag"],
                                count: countToInt(match["count"]),
                                type: match["type"],
                            };
                        }
                    }

                    var matches = Grabber.regexMatches('<article[^>]* id="[^"]*" class="[^"]*"\\s+data-id="(?<id>[^"]*)"\\s+data-has-sound="[^"]*"\\s+data-tags="(?<tags>[^"]*)"\\s+data-pools="(?<pools>[^"]*)"\\s+data-uploader="(?<author>[^"]*)"\\s+data-approver-id="(?<approver>[^"]*)"\\s+data-rating="(?<rating>[^"]*)"\\s+data-width="(?<width>[^"]*)"\\s+data-height="(?<height>[^"]*)"\\s+data-flags="(?<flags>[^"]*)"\\s+data-parent-id="(?<parent_id>[^"]*)"\\s+data-has-children="(?<has_children>[^"]*)"\\s+data-score="(?<score>[^"]*)"\\s+data-views="[^"]*"\\s+data-fav-count="(?<fav_count>[^"]*)"\\s+data-pixiv-id="[^"]*"\\s+data-file-ext="(?<ext>[^"]*)"\\s+data-source="[^"]*"\\s+data-normalized-source="[^"]*"\\s+data-is-favorited="[^"]*"\\s+data-md5="(?<md5>[^"]*)"\\s+data-file-url="(?<file_url>[^"]*)"\\s+data-large-file-url="(?<sample_url>[^"]*)"\\s+data-preview-file-url="(?<preview_url>[^"]*)"', src);
                    var images = [];
                    for (var i in matches) {
                        var match = matches[i];
                        if ("json" in match) {
                            var json = JSON.parse(match["json"]);
                            for (var key in json) {
                                match[key] = json[key];
                            }
                        }
                        images.push(buildImage(match));
                    }

                    return { images: images, tags: tags };
                },
            },
        },
    },
    auth: auth,
}

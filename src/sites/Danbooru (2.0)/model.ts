function completeImage(img: IImage): IImage {
    img.identity = {
        "id": img.id!,
        "md5": img.md5!,
    };

    if (img.ext && img.ext[0] === ".") {
        img.ext = img.ext.substr(1);
    }

    if (!img.file_url || img.file_url.length < 5) {
        img.file_url = `/data/${img.md5}.${img.ext || "jpg"}`;
    } else {
        img.file_url = img.file_url
            .replace("/preview/", "/")
            .replace("/ssd/", "/")
            .replace("/sample/[^.]*sample-", "/");
    }

    if (!img.sample_url || img.sample_url.length < 5) {
        img.sample_url = `/data/sample/sample-${img.md5}.jpg`;
    }

    if (!img.preview_url || img.preview_url.length < 5) {
        img.preview_url = `/data/preview/${img.md5}.jpg`;
    }

    return img;
}

export const source: ISource = {
    name: "Danbooru (2.0)",
    modifiers: ["rating:general", "rating:safe", "rating:questionable", "rating:explicit", "rating:g", "rating:s", "rating:q", "rating:e", "user:", "fav:", "fastfav:", "md5:", "source:", "id:", "width:", "height:", "score:", "mpixels:", "filesize:", "date:", "gentags:", "arttags:", "chartags:", "copytags:", "approver:", "parent:", "sub:", "status:any", "status:deleted", "status:active", "status:flagged", "status:pending", "order:id", "order:id_desc", "order:score", "order:score_asc", "order:mpixels", "order:mpixels_asc", "order:filesize", "order:landscape", "order:portrait", "order:favcount", "order:rank", "order:change", "order:change_desc", "parent:none", "unlocked:rating"],
    forcedTokens: ["filename"],
    tagFormat: {
        case: "lower",
        wordSeparator: "_",
    },
    searchFormat: {
        and: " ",
        or: {
            separator: " ",
            prefix: "~",
        },
        parenthesis: false,
        precedence: "or",
    },
    auth: {
        httpBasic: {
            type: "http_basic",
            passwordType: "apiKey",
        },
        url: {
            type: "url",
            fields: [
                {
                    id: "pseudo",
                    key: "login",
                },
                {
                    id: "apiKey",
                    key: "api_key",
                    type: "password",
                },
            ],
        },
        session: {
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
        },
    },
    apis: {
        json: {
            name: "JSON",
            auth: [],
            maxLimit: 200,
            search: {
                parseErrors: true,
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string | IError => {
                    try {
                        const pagePart = Grabber.pageUrl(query.page, previous, 1000, "{page}", "a{max}", "b{min}");
                        return "/posts.json?limit=" + opts.limit + "&page=" + pagePart + "&tags=" + encodeURIComponent(query.search);
                    } catch (e: any) {
                        return { error: e.message };
                    }
                },
                parse: (src: string): IParsedSearch | IError => {
                    const map = {
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
                        "tags_meta": "tag_string_meta",
                    };

                    const data = JSON.parse(src);

                    if ("success" in data && data["success"] === false && "message" in data) {
                        return { error: data["message"] };
                    }

                    const images: IImage[] = [];
                    for (const image of data) {
                        const img = Grabber.mapFields(image, map);
                        if (!img.md5 || img.md5.length === 0) {
                            continue;
                        }
                        if ("media_asset" in image && "variants" in image["media_asset"]) {
                            img.medias = image["media_asset"]["variants"].map((variant: any) => ({
                                type: variant["type"] === "sample" ? "sample" : (variant["type"] === "original" ? "full" : undefined),
                                url: variant.url,
                                width: variant.width,
                                height: variant.height,
                            }));
                        }
                        images.push(completeImage(img));
                    }

                    return { images };
                },
            },
            endpoints: {
                ugoira_details: {
                    input: {
                        id: {
                            type: "input",
                        },
                    },
                    url: (query: Record<"id", number>): string => {
                        return "/posts/" + String(query.id) + ".json?only=media_metadata";
                    },
                    parse: (src: string): IParsedUgoiraDetails => {
                        const delays = JSON.parse(src)["media_metadata"]["metadata"]["Ugoira:FrameDelays"];
                        return {
                            frames: delays.map((delay: number) => ({ delay })),
                        };
                    },
                },
                pool_list: {
                    name: "Pools",
                    input: {
                        name: {
                            type: "input",
                        },
                    },
                    url: (query: Record<"name", string>, opts: IUrlOptions): string => {
                        return "/pools.json?page=" + String(opts.page) + (query.name ? "&search[name_contains]=" + encodeURIComponent(query.name) : "");
                    },
                    parse: (src: string): IParsedSearch => {
                        const data = JSON.parse(src);
                        const images = data.map((raw: any): IImage => ({
                            id: raw["id"],
                            name: raw["name"],
                            created_at: raw["created_at"],
                            type: "gallery",
                            gallery_count: raw["post_count"],
                            details_endpoint: {
                                endpoint: "pool_details",
                                input: { id: raw["id"] },
                            },
                        }))
                        return { images };
                    },
                },
                pool_details: {
                    input: {
                        id: {
                            type: "input",
                        },
                    },
                    url: (query: Record<"id", number>): string => {
                        return "/pools/" + String(query.id) + ".json";
                    },
                    parse: (src: string): IParsedGallery => {
                        const data = JSON.parse(src);
                        const images = data["post_ids"].map((id: number): IImage => ({ id }));
                        return { images };
                    },
                },
            },
            tags: {
                url: (query: ITagsQuery, opts: IUrlOptions): string => {
                    return "/tags.json?limit=" + opts.limit + "&search[order]=" + query.order + "&page=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    const map = {
                        "id": "id",
                        "name": "name",
                        "count": "post_count",
                        "typeId": "category",
                        "related": "related_tags",
                    };

                    const data = JSON.parse(src);

                    const tags: ITag[] = [];
                    for (const tag of data) {
                        const ret = Grabber.mapFields(tag, map);
                        if (ret.related) {
                            ret.related = ret.related.split(" ").filter((_: string, i: number) => i % 2 === 0);
                        }
                        tags.push(ret);
                    }

                    return { tags };
                },
            },
        },
        xml: {
            name: "XML",
            auth: [],
            maxLimit: 200,
            search: {
                parseErrors: true,
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string | IError => {
                    try {
                        const pagePart = Grabber.pageUrl(query.page, previous, 1000, "{page}", "a{max}", "b{min}");
                        return "/posts.xml?limit=" + opts.limit + "&page=" + pagePart + "&tags=" + encodeURIComponent(query.search);
                    } catch (e: any) {
                        return { error: e.message };
                    }
                },
                parse: (src: string): IParsedSearch | IError => {
                    const map = {
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
                        "tags_meta": "tag-string-meta",
                    };

                    const xml = Grabber.parseXML(src);

                    if ("result" in xml && "@attributes" in xml["result"] && "success" in xml["result"]["@attributes"] && xml["result"]["@attributes"]["success"] === "false") {
                        return { error: xml["result"]["#text"] };
                    }

                    const data = Grabber.makeArray(Grabber.typedXML(xml).posts.post);
                    const images: IImage[] = [];
                    for (const image of data) {
                        const img = Grabber.mapFields(image, map);
                        if (!img.md5 || img.md5.length === 0) {
                            continue;
                        }
                        images.push(completeImage(img));
                    }

                    return { images };
                },
            },
            tags: {
                url: (query: ITagsQuery, opts: IUrlOptions): string => {
                    return "/tags.xml?limit=" + opts.limit + "&search[order]=" + query.order + "&page=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    const map = {
                        "id": "id",
                        "name": "name",
                        "count": "post-count",
                        "typeId": "category",
                        "related": "related-tags",
                    };

                    const data = Grabber.makeArray(Grabber.typedXML(Grabber.parseXML(src)).tags.tag);

                    const tags: ITag[] = [];
                    for (const tag of data) {
                        const ret = Grabber.mapFields(tag, map);
                        if (ret.related) {
                            ret.related = ret.related.split(" ").filter((_: string, i: number) => i % 2 === 0);
                        }
                        tags.push(ret);
                    }

                    return { tags };
                },
            },
        },
        html: {
            name: "Regex",
            auth: [],
            maxLimit: 200,
            search: {
                parseErrors: true,
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string | IError => {
                    try {
                        const pagePart = Grabber.pageUrl(query.page, previous, 1000, "{page}", "a{max}", "b{min}");
                        return "/posts?limit=" + opts.limit + "&page=" + pagePart + "&tags=" + encodeURIComponent(query.search);
                    } catch (e: any) {
                        return { error: e.message };
                    }
                },
                parse: (src: string, statusCode: number): IParsedSearch | IError => {
                    const match = src.match(/<div id="page">\s*<p>([^<]+)<\/p>\s*<\/div>/m);
                    if (match) {
                        return { error: match[1] };
                    }

                    let wiki = Grabber.regexToConst("wiki", '<div id="excerpt"(?:[^>]+)>(?<wiki>.+?)</div>', src);
                    wiki = wiki ? wiki.replace(/href="\/wiki_pages\/show_or_new\?title=([^"]+)"/g, 'href="$1"') : wiki;
                    const pageCounts = Grabber.regexMatches('>(?<page>\\d+)</(?:a|span)></li><li[^<]*><(?:a|span)[^>]*>(?:&gt;&gt;|<i class="[^"]+"></i>)<|</i>\\s*<(?:a|span)[^>]*>(?<page_2>\\d+)<', src);
                    return {
                        tags: Grabber.regexToTags('<li class="(?:category|tag-type)-(?<typeId>[^"]+)"[^>]*>(?:\\s*<a class="wiki-link" href="[^"]+">\\?</a>)?(?:\\s*<a[^>]* class="search-inc-tag">[^<]+</a>\\s*<a[^>]* class="search-exl-tag">[^<]+</a>)?\\s*<a class="search-tag"\\s+[^>]*href="[^"]+"[^>]*>(?<name>[^<]+)</a>\\s*<span class="post-count"[^>]*>(?<count>[^<]+)</span>\\s*</li>', src),
                        images: Grabber.regexToImages('<article[^>]* id="[^"]*" class="[^"]*"\\s+data-id="(?<id>[^"]*)"\\s+data-has-sound="[^"]*"\\s+data-tags="(?<tags>[^"]*)"\\s+data-pools="(?<pools>[^"]*)"(?:\\s+data-uploader="(?<author>[^"]*)")?\\s+data-approver-id="(?<approver>[^"]*)"\\s+data-rating="(?<rating>[^"]*)"\\s+data-width="(?<width>[^"]*)"\\s+data-height="(?<height>[^"]*)"\\s+data-flags="(?<flags>[^"]*)"\\s+data-parent-id="(?<parent_id>[^"]*)"\\s+data-has-children="(?<has_children>[^"]*)"\\s+data-score="(?<score>[^"]*)"\\s+data-views="[^"]*"\\s+data-fav-count="(?<fav_count>[^"]*)"\\s+data-pixiv-id="[^"]*"\\s+data-file-ext="(?<ext>[^"]*)"\\s+data-source="(?<source>[^"]*)"\\s+data-top-tagger="[^"]*"\\s+data-uploader-id="[^"]*"\\s+data-normalized-source="[^"]*"\\s+data-is-favorited="[^"]*"\\s+data-md5="(?<md5>[^"]*)"\\s+data-file-url="(?<file_url>[^"]*)"\\s+data-large-file-url="(?<sample_url>[^"]*)"\\s+data-preview-file-url="(?<preview_url>[^"]*)"', src).map(completeImage),
                        wiki,
                        pageCount: pageCounts?.length > 0 ? pageCounts[pageCounts.length - 1]["page"] : undefined,
                    };
                },
            },
            details: {
                url: (id: string, md5: string): string => {
                    return "/posts/" + id;
                },
                parse: (src: string): IParsedDetails => {
                    return {
                        pools: Grabber.regexToPools('<div class="status-notice" id="pool\\d+">[^<]*Pool:[^<]*(?:<a href="/post/show/(?<previous>\\d+)" >&lt;&lt;</a>)?[^<]*<a href="/pool/show/(?<id>\\d+)" >(?<name>[^<]+)</a>[^<]*(?:<a href="/post/show/(?<next>\\d+)" >&gt;&gt;</a>)?[^<]*</div>', src),
                        tags: Grabber.regexToTags('<li class="category-(?<typeId>[^"]+)">(?:\\s*<a class="wiki-link" href="[^"]+">\\?</a>)?\\s*<a class="search-tag"\\s+[^>]*href="[^"]+"[^>]*>(?<name>[^<]+)</a>\\s*<span class="post-count">(?<count>[^<]+)</span>\\s*</li>', src),
                        imageUrl: Grabber.regexToConst("url", 'Size: <a href="(?<url>[^"]+?)(?:\\?download=1[^"]*)?"', src),
                    };
                },
            },
            tagTypes: {
                url: (): string => {
                    return "/tags";
                },
                parse: (src: string): IParsedTagTypes | IError => {
                    const contents = src.match(/<select[^>]* name="search\[category\]"[^>]*>([\s\S]+)<\/select>/);
                    if (!contents) {
                        return { error: "Parse error: could not find the tag type <select> tag" };
                    }
                    const results = Grabber.regexMatches('<option value="(?<id>\\d+)">(?<name>[^<]+)</option>', contents[1]);
                    const types = results.map((r: any) => ({
                        id: r.id,
                        name: r.name.toLowerCase(),
                    }));
                    return { types };
                },
            },
            tags: {
                url: (query: ITagsQuery, opts: IUrlOptions): string => {
                    return "/tags?limit=" + opts.limit + "&search[order]=" + query.order + "&page=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    return {
                        tags: Grabber.regexToTags('<tr[^>]*>\\s*<td[^>]*>(?<count>\\d+)</td>\\s*<td class="category-(?<typeId>\\d+)">\\s*<a[^>]+>\\?</a>\\s*<a[^>]+>(?<name>.+?)</a>\\s*</td>\\s*<td[^>]*>\\s*(?:<a href="/tags/(?<id>\\d+)/[^"]+">)?', src),
                    };
                },
            },
            check: {
                url: (): string => {
                    return "/";
                },
                parse: (src: string): boolean => {
                    return src.indexOf("Running Danbooru v2") !== -1
                        || src.search(/Running Danbooru <a[^>]*>v2/) !== -1
                        || src.indexOf("https://github.com/danbooru/danbooru") !== -1
                        || src.indexOf("ATFBooru") !== -1
                        || src.indexOf("All The Fallen") !== -1
                        || src.indexOf("https://github.com/Iratu/atfbooru") !== -1;
                },
            },
        },
    },
};

const mapFields = (data: any, map: any): any => {
    const result: any = {};
    if (typeof data !== "object") {
        return result;
    }
    for (const to in map) {
        const from = map[to];
        let val = from in data ? data[from] : undefined;
        if (val && typeof val === "object" && ("#text" in val || "@attributes" in val)) {
            val = val["#text"];
        }
        result[to] = val;
    }
    return result;
};

const countToInt = (str: string): number => {
    let count: number;
    const normalized = str.toLowerCase().trim().replace(",", "");
    if (normalized.slice(-1) === "k") {
        const withoutK = normalized.substring(0, normalized.length - 1).trim();
        count = parseFloat(withoutK) * 1000;
    } else {
        count = parseFloat(normalized);
    }
    return Math.floor(count);
};

const makeTag = (match: any): ITag => {
    match["count"] = countToInt(match["count"]);
    return match;
};

const loginUrl = (fields: any, values: any): string => {
    let res = "";
    for (const field of fields) {
        res += field.key + "=" + values[field.key] + "&";
    }
    return res;
};

const fixPageUrl = (url: string, page: number, previous: any): string => {
    url = url.replace("{page}", String(page));
    if (previous) {
        url = url.replace("{min}", previous.minId);
        url = url.replace("{max}", previous.maxId);
        url = url.replace("{min-1}", String(previous.minId - 1));
        url = url.replace("{max-1}", String(previous.maxId - 1));
        url = url.replace("{min+1}", previous.minId + 1);
        url = url.replace("{max+1}", previous.maxId + 1);
    }
    return url;
};

const pageUrl = (page: number, previous: any, limit: number, ifBelow: string, ifPrev: string, ifNext: string): string => {
    if (page < limit || !previous) {
        return fixPageUrl(ifBelow, page, previous);
    }
    if (previous.page > page) {
        return fixPageUrl(ifPrev, page, previous);
    }
    return fixPageUrl(ifNext, page, previous);
};

const buildImage = (data: any): IImage => {
    data["page_url"] = "/posts/" + data["id"];
    return data;
};

const auth: { [id: string]: IAuth } = {
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
        },
    },
};

__source = {
    name: "Danbooru (2.0)",
    modifiers: ["rating:safe", "rating:questionable", "rating:explicit", "rating:s", "rating:q", "rating:e", "user:", "fav:", "fastfav:", "md5:", "source:", "id:", "width:", "height:", "score:", "mpixels:", "filesize:", "date:", "gentags:", "arttags:", "chartags:", "copytags:", "approver:", "parent:", "sub:", "status:any", "status:deleted", "status:active", "status:flagged", "status:pending", "order:id", "order:id_desc", "order:score", "order:score_asc", "order:mpixels", "order:mpixels_asc", "order:filesize", "order:landscape", "order:portrait", "order:favcount", "order:rank", "order:change", "order:change_desc", "parent:none", "unlocked:rating"],
    auth,
    apis: {
        json: {
            name: "JSON",
            auth: [],
            maxLimit: 200,
            search: {
                url: (query: any, opts: any, previous: any): IUrl | IError | string => {
                    const loginPart = loginUrl(auth.url.fields, opts["auth"]);
                    const pagePart = pageUrl(query.page, previous, 1000, "{page}", "a{max}", "b{min}");
                    return "/posts.json?" + loginPart + "limit=" + opts.limit + "&page=" + pagePart + "&tags=" + query.search;
                },
                parse: (src: string): IParsedSearch => {
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
                    };

                    const data = JSON.parse(src);

                    const images: IImage[] = [];
                    for (const image of data) {
                        images.push(buildImage(mapFields(image, map)));
                    }

                    return { images };
                },
            },
            tags: {
                url: (query: any, opts: any): IUrl | IError | string => {
                    const loginPart = loginUrl(auth.url.fields, opts["auth"]);
                    return "/tags.json?" + loginPart + "limit=" + opts.limit + "&page=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    const map = {
                        "id": "id",
                        "name": "name",
                        "count": "post_count",
                        "typeId": "category",
                    };

                    const data = JSON.parse(src);

                    const tags: ITag[] = [];
                    for (const tag of data) {
                        tags.push(mapFields(tag, map));
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
                url: (query: any, opts: any, previous: any): IUrl | IError | string => {
                    const loginPart = loginUrl(auth.url.fields, opts["auth"]);
                    const pagePart = pageUrl(query.page, previous, 1000, "{page}", "a{max}", "b{min}");
                    return "/posts.xml?" + loginPart + "limit=" + opts.limit + "&page=" + pagePart + "&tags=" + query.search;
                },
                parse: (src: string): IParsedSearch => {
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
                    };

                    const data = Grabber.parseXML(src).posts.post;

                    const images: IImage[] = [];
                    for (const image of data) {
                        images.push(buildImage(mapFields(image, map)));
                    }

                    return { images };
                },
            },
            tags: {
                url: (query: any, opts: any): IUrl | IError | string => {
                    const loginPart = loginUrl(auth.url.fields, opts["auth"]);
                    return "/tags.xml?" + loginPart + "limit=" + opts.limit + "&page=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    const map = {
                        "id": "id",
                        "name": "name",
                        "count": "post-count",
                        "typeId": "category",
                    };

                    const data = Grabber.parseXML(src).tags.tag;

                    const tags: ITag[] = [];
                    for (const tag of data) {
                        tags.push(mapFields(tag, map));
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
                url: (query: any, opts: any, previous: any): IUrl | IError | string => {
                    const loginPart = loginUrl(auth.url.fields, opts["auth"]);
                    const pagePart = pageUrl(query.page, previous, 1000, "{page}", "a{max}", "b{min}");
                    return "/posts?" + loginPart + "limit=" + opts.limit + "&page=" + pagePart + "&tags=" + query.search;
                },
                parse: (src: string): IParsedSearch => {
                    // Tags
                    const tags: { [name: string]: ITag } = {};
                    const tagMatches = Grabber.regexMatches('<li class="category-(?<typeId>[^"]+)">(?:\\s*<a class="wiki-link" href="[^"]+">\\?</a>)?\\s*<a class="search-tag"\\s+[^>]*href="[^"]+"[^>]*>(?<name>[^<]+)</a>\\s*<span class="post-count">(?<count>[^<]+)</span>\\s*</li>', src);
                    for (const tagMatch of tagMatches) {
                        if (!(tagMatch["name"] in tags)) {
                            tags[tagMatch["name"]] = makeTag(tagMatch);
                        }
                    }

                    // Images
                    const images: IImage[] = [];
                    const imgMatches = Grabber.regexMatches('<article[^>]* id="[^"]*" class="[^"]*"\\s+data-id="(?<id>[^"]*)"\\s+data-has-sound="[^"]*"\\s+data-tags="(?<tags>[^"]*)"\\s+data-pools="(?<pools>[^"]*)"\\s+data-uploader="(?<author>[^"]*)"\\s+data-approver-id="(?<approver>[^"]*)"\\s+data-rating="(?<rating>[^"]*)"\\s+data-width="(?<width>[^"]*)"\\s+data-height="(?<height>[^"]*)"\\s+data-flags="(?<flags>[^"]*)"\\s+data-parent-id="(?<parent_id>[^"]*)"\\s+data-has-children="(?<has_children>[^"]*)"\\s+data-score="(?<score>[^"]*)"\\s+data-views="[^"]*"\\s+data-fav-count="(?<fav_count>[^"]*)"\\s+data-pixiv-id="[^"]*"\\s+data-file-ext="(?<ext>[^"]*)"\\s+data-source="[^"]*"\\s+data-normalized-source="[^"]*"\\s+data-is-favorited="[^"]*"\\s+data-md5="(?<md5>[^"]*)"\\s+data-file-url="(?<file_url>[^"]*)"\\s+data-large-file-url="(?<sample_url>[^"]*)"\\s+data-preview-file-url="(?<preview_url>[^"]*)"', src);
                    for (const imgMatch of imgMatches) {
                        if ("json" in imgMatch) {
                            const json = JSON.parse(imgMatch["json"]);
                            for (const key in json) {
                                imgMatch[key] = json[key];
                            }
                        }
                        images.push(buildImage(imgMatch));
                    }

                    return { images, tags };
                },
            },
            details: {
                url: (id: number, md5: string): IUrl | IError | string => {
                    return "/posts/" + id;
                },
                parse: (src: string): IParsedDetails => {
                    // Pools
                    const pools: IPool[] = [];
                    const poolMatches = Grabber.regexMatches('<div class="status-notice" id="pool\\d+">[^<]*Pool:[^<]*(?:<a href="/post/show/(?<previous>\\d+)" >&lt;&lt;</a>)?[^<]*<a href="/pool/show/(?<id>\\d+)" >(?<name>[^<]+)</a>[^<]*(?:<a href="/post/show/(?<next>\\d+)" >&gt;&gt;</a>)?[^<]*</div>', src);
                    for (const poolMatch of poolMatches) {
                        pools.push(poolMatch);
                    }

                    // Tags
                    const tags: { [name: string]: ITag } = {};
                    const tagMatches = Grabber.regexMatches('<li class="category-(?<typeId>[^"]+)">(?:\\s*<a class="wiki-link" href="[^"]+">\\?</a>)?\\s*<a class="search-tag"\\s+[^>]*href="[^"]+"[^>]*>(?<name>[^<]+)</a>\\s*<span class="post-count">(?<count>[^<]+)</span>\\s*</li>', src);
                    for (const tagMatch of tagMatches) {
                        if (!(tagMatch["name"] in tags)) {
                            tags[tagMatch["name"]] = makeTag(tagMatch);
                        }
                    }

                    // Image url
                    let imageUrl: string;
                    const imageUrlMatches = Grabber.regexMatches('<section[^>]* data-file-url="(?<url>[^"]*)"', src);
                    for (const imageUrlMatch of imageUrlMatches) {
                        imageUrl = imageUrlMatch["url"];
                    }

                    return { pools, tags, imageUrl };
                },
            },
            tags: {
                url: (query: any, opts: any): IUrl | IError | string => {
                    const loginPart = loginUrl(auth.url.fields, opts["auth"]);
                    return "/tags?" + loginPart + "limit=" + opts.limit + "&page=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    // Tags
                    const tags: ITag[] = [];
                    const tagMatches = Grabber.regexMatches('<tr[^>]*>\\s*<td[^>]*>(?<count>\\d+)</td>\\s*<td class="category-(?<typeId>\\d+)">\\s*<a[^>]+>\\?</a>\\s*<a[^>]+>(?<name>.+?)</a>\\s*</td>\\s*<td[^>]*>\\s*(?:<a href="/tags/(?<id>\\d+)/[^"]+">)?', src);
                    for (const tagMatch of tagMatches) {
                        tags.push(makeTag(tagMatch));
                    }

                    return { tags };
                },
            },
        },
    },
};

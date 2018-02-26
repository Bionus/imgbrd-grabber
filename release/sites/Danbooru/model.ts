const makeTag = (match: any): ITag => {
    match["count"] = Grabber.countToInt(match["count"]);
    return match;
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
        url: "/user/authenticate",
        fields: [
            {
                key: "user[name]",
                type: "username",
            },
            {
                key: "user[password]",
                type: "password",
            },
        ],
        check: {
            type: "cookie",
            key: "pass_hash",
        },
    },
};

export const source: ISource = {
    name: "Danbooru",
    modifiers: ["rating:safe", "rating:questionable", "rating:explicit", "user:", "fav:", "fastfav:", "md5:", "source:", "id:", "width:", "height:", "score:", "mpixels:", "filesize:", "date:", "gentags:", "arttags:", "chartags:", "copytags:", "approver:", "parent:", "sub:", "status:any", "status:deleted", "status:active", "status:flagged", "status:pending", "order:id", "order:id_desc", "order:score", "order:score_asc", "order:mpixels", "order:mpixels_asc", "order:filesize", "order:landscape", "order:portrait", "order:favcount", "order:rank", "order:change", "order:change_desc", "parent:none", "unlocked:rating"],
    auth,
    apis: {
        json: {
            name: "JSON",
            auth: [],
            maxLimit: 200,
            search: {
                url: (query: any, opts: any, previous: any): IUrl | IError | string => {
                    const loginPart = Grabber.loginUrl(auth.url.fields, opts["auth"]);
                    const pagePart = Grabber.pageUrl(query.page, previous, 750, "page={page}", "after_id={max}", "before_id={min}");
                    return "/post/index.json?" + loginPart + "limit=" + opts.limit + "&" + pagePart + "&typed_tags=true&tags=" + query.search;
                },
                parse: (src: string): IParsedSearch => {
                    const data = JSON.parse(src);

                    const images: IImage[] = [];
                    for (const image of data) {
                        images.push(image);
                    }

                    return { images };
                },
            },
            tags: {
                url: (query: any, opts: any): IUrl | IError | string => {
                    const loginPart = Grabber.loginUrl(auth.url.fields, opts["auth"]);
                    return "/tag/index.json?" + loginPart + "limit=" + opts.limit + "&page=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    const map = {
                        "id": "id",
                        "name": "name",
                        "count": "count",
                        "typeId": "type",
                    };

                    const data = JSON.parse(src);

                    const tags: ITag[] = [];
                    for (const tag of data) {
                        tags.push(Grabber.mapFields(tag, map));
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
                    const loginPart = Grabber.loginUrl(auth.url.fields, opts["auth"]);
                    const pagePart = Grabber.pageUrl(query.page, previous, 750, "page={page}", "after_id={max}", "before_id={min}");
                    return "/post/index.xml?" + loginPart + "limit=" + opts.limit + "&" + pagePart + "&typed_tags=true&tags=" + query.search;
                },
                parse: (src: string): IParsedSearch => {
                    const data = Grabber.parseXML(src).posts.post;

                    const images: IImage[] = [];
                    for (const image of data) {
                        images.push(image["@attributes"]);
                    }

                    return { images };
                },
            },
            tags: {
                url: (query: any, opts: any): IUrl | IError | string => {
                    const loginPart = Grabber.loginUrl(auth.url.fields, opts["auth"]);
                    return "/tag/index.xml?" + loginPart + "limit=" + opts.limit + "&page=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    const map = {
                        "id": "id",
                        "name": "name",
                        "count": "count",
                        "typeId": "type",
                    };

                    const data = Grabber.parseXML(src).tags.tag;

                    const tags: ITag[] = [];
                    for (const tag of data) {
                        tags.push(Grabber.mapFields(tag["@attributes"], map));
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
                    const loginPart = Grabber.loginUrl(auth.url.fields, opts["auth"]);
                    const pagePart = Grabber.pageUrl(query.page, previous, 750, "page={page}", "after_id={max}", "before_id={min}");
                    return "/post/index?" + loginPart + "limit=" + opts.limit + "&" + pagePart + "&typed_tags=true&tags=" + query.search;
                },
                parse: (src: string): IParsedSearch => {
                    // Tags
                    const tags: { [name: string]: ITag } = {};
                    const tagMatches = Grabber.regexMatches('<li class="?[^">]*tag-type-(?<type>[^">]+)(?:|"[^>]*)>.*?<a href="[^"]+"[^>]*>(?<name>[^<\\?]+)</a>.*?<span class="?post-count"?>(?<count>\\d+)</span>.*?</li>', src);
                    for (const tagMatch of tagMatches) {
                        if (!(tagMatch["name"] in tags)) {
                            tags[tagMatch["name"]] = makeTag(tagMatch);
                        }
                    }

                    // Images
                    const images: IImage[] = [];
                    const imgMatches = Grabber.regexMatches("Post\\.register\\((?<json>\\{.+?\\})\\);?", src);
                    for (const imgMatch of imgMatches) {
                        if ("json" in imgMatch) {
                            const json = JSON.parse(imgMatch["json"]);
                            for (const key in json) {
                                imgMatch[key] = json[key];
                            }
                        }
                        images.push(imgMatch);
                    }

                    // Wiki
                    let wiki: string;
                    const wikiMatches = Grabber.regexMatches('<div id="sidebar-wiki"(?:[^>]+)>(?<wiki>.+?)</div>');
                    for (const wikiMatch of wikiMatches) {
                        wiki = wikiMatch["wiki"];
                    }

                    // Last page
                    let pageCount: number;
                    const lastPageMatches = Grabber.regexMatches('<link href="[^"]*\\?.*?page=(?<page>\\d+)[^"]*" rel="last" title="Last Page">');
                    for (const lastPageMatch of lastPageMatches) {
                        pageCount = lastPageMatch["page"];
                    }

                    return { images, tags, wiki, pageCount };
                },
            },
            details: {
                url: (id: number, md5: string): IUrl | IError | string => {
                    return "/post/show/" + id;
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
                    const tagMatches = Grabber.regexMatches('<li class="?[^">]*tag-type-(?<type>[^">]+)(?:|"[^>]*)>.*?<a href="[^"]+"[^>]*>(?<name>[^<\\?]+)</a>.*?<span class="?post-count"?>(?<count>\\d+)</span>.*?</li>', src);
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
                    const loginPart = Grabber.loginUrl(auth.url.fields, opts["auth"]);
                    return "/tag/index?" + loginPart + "limit=" + opts.limit + "&page=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    // Tags
                    const tags: ITag[] = [];
                    const tagMatches = Grabber.regexMatches("<tr[^>]*>\\s*<td[^>]*>(?<count>\\d+)</td>\\s*<td[^>]*>\\s*(?:<a[^>]+>\\?</a>\\s*)?<a[^>]+>(?<tag>.+?)</a>\\s*</td>\\s*<td[^>]*>\\s*(?<type>.+?)\\s*\\([^()]+\\)\\s*</td>", src);
                    for (const tagMatch of tagMatches) {
                        tags.push(makeTag(tagMatch));
                    }

                    return { tags };
                },
            },
        },
    },
};

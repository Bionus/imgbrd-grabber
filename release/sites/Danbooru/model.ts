const makeImage = (data: any): IImage => {
    if (data && "tags" in data && typeof data["tags"] === "object") {
        for (const type in data["tags"]) {
            const children = data["tags"][type];
            const elts: any = "tag" in children ? children["tag"] : children;
            let tags: string = "";
            for (const i in elts) {
                const tag = elts[i];
                tags += (tags.length !== 0 ? " " : "") + (typeof tag === "object" ? tag["#text"] : tag);
            }
            data["tags_" + type] = tags;
        }
    }
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
                url: (query: any, opts: any, previous: any): string => {
                    const loginPart = Grabber.loginUrl(auth.url.fields, opts["auth"]);
                    const pagePart = Grabber.pageUrl(query.page, previous, 750, "page={page}", "after_id={max}", "before_id={min}");
                    return "/post/index.json?" + loginPart + "limit=" + opts.limit + "&" + pagePart + "&typed_tags=true&tags=" + query.search;
                },
                parse: (src: string): IParsedSearch => {
                    const data = JSON.parse(src);

                    const images: IImage[] = [];
                    for (const image of data) {
                        images.push(makeImage(image));
                    }

                    return { images };
                },
            },
            tags: {
                url: (query: any, opts: any): string => {
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
                url: (query: any, opts: any, previous: any): string => {
                    const loginPart = Grabber.loginUrl(auth.url.fields, opts["auth"]);
                    const pagePart = Grabber.pageUrl(query.page, previous, 750, "page={page}", "after_id={max}", "before_id={min}");
                    return "/post/index.xml?" + loginPart + "limit=" + opts.limit + "&" + pagePart + "&typed_tags=true&tags=" + query.search;
                },
                parse: (src: string): IParsedSearch => {
                    const data = Grabber.parseXML(src).posts.post;

                    const images: IImage[] = [];
                    for (const dta of data) {
                        const image: any = "@attributes" in dta && "id" in dta["@attributes"] ? dta["@attributes"] : dta;
                        images.push(makeImage(image));
                    }

                    return { images };
                },
            },
            tags: {
                url: (query: any, opts: any): string => {
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
                    for (const dta of data) {
                        const tag: any = "@attributes" in dta && "id" in dta["@attributes"] ? dta["@attributes"] : dta;
                        tags.push(Grabber.mapFields(tag, map));
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
                url: (query: any, opts: any, previous: any): string => {
                    const loginPart = Grabber.loginUrl(auth.url.fields, opts["auth"]);
                    const pagePart = Grabber.pageUrl(query.page, previous, 750, "page={page}", "after_id={max}", "before_id={min}");
                    return "/post/index?" + loginPart + "limit=" + opts.limit + "&" + pagePart + "&typed_tags=true&tags=" + query.search;
                },
                parse: (src: string): IParsedSearch => {
                    return {
                        images: Grabber.regexToImages("Post\\.register\\((?<json>\\{.+?\\})\\);?", src),
                        tags: Grabber.regexToTags('<li class="?[^">]*tag-type-(?<type>[^">]+)(?:|"[^>]*)>.*?<a href="[^"]+"[^>]*>(?<name>[^<\\?]+)</a>.*?<span class="?post-count"?>(?<count>\\d+)</span>.*?</li>', src),
                        wiki: Grabber.regexToConst("wiki", '<div id="sidebar-wiki"(?:[^>]+)>(?<wiki>.+?)</div>'),
                        pageCount: Grabber.regexToConst("page", '<link href="[^"]*\\?.*?page=(?<page>\\d+)[^"]*" rel="last" title="Last Page">'),
                    };
                },
            },
            details: {
                url: (id: number, md5: string): string => {
                    return "/post/show/" + id;
                },
                parse: (src: string): IParsedDetails => {
                    return {
                        pools: Grabber.regexToPools('<div class="status-notice" id="pool\\d+">[^<]*Pool:[^<]*(?:<a href="/post/show/(?<previous>\\d+)" >&lt;&lt;</a>)?[^<]*<a href="/pool/show/(?<id>\\d+)" >(?<name>[^<]+)</a>[^<]*(?:<a href="/post/show/(?<next>\\d+)" >&gt;&gt;</a>)?[^<]*</div>', src),
                        tags: Grabber.regexToTags('<li class="?[^">]*tag-type-(?<type>[^">]+)(?:|"[^>]*)>.*?<a href="[^"]+"[^>]*>(?<name>[^<\\?]+)</a>.*?<span class="?post-count"?>(?<count>\\d+)</span>.*?</li>', src),
                        imageUrl: Grabber.regexToConst("url", '<section[^>]* data-file-url="(?<url>[^"]*)"', src),
                    };
                },
            },
            tags: {
                url: (query: any, opts: any): string => {
                    const loginPart = Grabber.loginUrl(auth.url.fields, opts["auth"]);
                    return "/tag/index?" + loginPart + "limit=" + opts.limit + "&page=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    return {
                        tags: Grabber.regexToTags("<tr[^>]*>\\s*<td[^>]*>(?<count>\\d+)</td>\\s*<td[^>]*>\\s*(?:<a[^>]+>\\?</a>\\s*)?<a[^>]+>(?<name>.+?)</a>\\s*</td>\\s*<td[^>]*>\\s*(?<type>.+?)\\s*\\([^()]+\\)\\s*</td>", src),
                    };
                },
            },
        },
    },
};

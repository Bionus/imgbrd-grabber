function completeImage(img: IImage): IImage {
    if (!img["file_url"] && img["file_url"].length < 5) {
        img["file_url"] = img["preview_url"].replace("/preview/", "/");
    }

    return img;
}

export const source: any = {
    name: "Moebooru",
    modifiers: ["rating:safe", "rating:questionable", "rating:explicit", "user:", "fav:", "fastfav:", "md5:", "source:", "id:", "width:", "height:", "score:", "mpixels:", "filesize:", "date:", "gentags:", "arttags:", "chartags:", "copytags:", "approver:", "parent:", "sub:", "status:any", "status:deleted", "status:active", "status:flagged", "status:pending", "order:id", "order:id_desc", "order:score", "order:score_asc", "order:mpixels", "order:mpixels_asc", "order:filesize", "order:landscape", "order:portrait", "order:favcount", "order:rank", "order:change", "order:change_desc", "parent:none", "unlocked:rating"],
    forcedTokens: [],
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
        url: {
            type: "url",
            fields: [
                {
                    id: "pseudo",
                    key: "login",
                },
                {
                    id: "password",
                    type: "password",
                },
                {
                    key: "password_hash",
                    type: "hash",
                    hash: "sha1",
                    salt: "choujin-steiner--%password%--",
                },
            ],
        },
    },
    apis: {
        json: {
            name: "JSON",
            auth: [],
            maxLimit: 1000,
            search: {
                url: (query: any, opts: any, previous: any): string => {
                    const pagePart = Grabber.pageUrl(query.page, previous, -1, "{page}");
                    return "/post/index.json?limit=" + opts.limit + "&page=" + pagePart + "&tags=" + encodeURIComponent(query.search);
                },
                parse: (src: string): IParsedSearch => {
                    const data = JSON.parse(src);

                    const images: IImage[] = [];
                    for (const image of data) {
                        images.push(completeImage(image));
                    }

                    return { images };
                },
            },
            tags: {
                url: (query: any, opts: any): string => {
                    return "/tag.json?page=" + query.page;
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
            maxLimit: 1000,
            search: {
                url: (query: any, opts: any, previous: any): string => {
                    const pagePart = Grabber.pageUrl(query.page, previous, -1, "{page}");
                    return "/post/index.xml?limit=" + opts.limit + "&page=" + pagePart + "&tags=" + encodeURIComponent(query.search);
                },
                parse: (src: string): IParsedSearch => {
                    const parsed = Grabber.parseXML(src);
                    const data = Grabber.makeArray(parsed.posts.post);

                    const images: IImage[] = [];
                    for (const dta of data) {
                        const image: any = "@attributes" in dta && "id" in dta["@attributes"] ? dta["@attributes"] : dta;
                        images.push(completeImage(image));
                    }

                    return {
                        images,
                        imageCount: parsed.posts["@attributes"]["count"],
                    };
                },
            },
            tags: {
                url: (query: any, opts: any): string => {
                    return "/tag.xml?page=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    const map = {
                        "id": "id",
                        "name": "name",
                        "count": "count",
                        "typeId": "type",
                    };

                    const data = Grabber.makeArray(Grabber.parseXML(src).tags.tag);

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
            maxLimit: 1000,
            search: {
                url: (query: any, opts: any, previous: any): string => {
                    const pagePart = Grabber.pageUrl(query.page, previous, -1, "{page}");
                    return "/post/index?limit=" + opts.limit + "&page=" + pagePart + "&tags=" + encodeURIComponent(query.search);
                },
                parse: (src: string): IParsedSearch => {
                    const images = Grabber.regexToImages("Post\\.register\\((?<json>\\{.+?\\})\\);?", src).map(completeImage);
                    let pageCount = Grabber.regexToConst("page", '>(?<page>\\d+)</a>\\s*<a class="next_page" rel="next" href="', src);
                    if (pageCount === undefined && /<div id="paginator">\s*<\/div>/.test(src)) {
                        pageCount = 1;
                    }
                    return {
                        tags: Grabber.regexToTags('<li class="(?:[^"]* )?tag-type-(?<type>[^" ]+)"[^>]*>(?:[^<]*<a[^>]*>[^<]*</a>)*[^<]*<a[^>]*>(?<name>[^<]*)</a>[^<]*<span[^>]*>(?<count>\\d+)k?</span>[^<]*</li>', src),
                        images,
                        pageCount,
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
                        tags: Grabber.regexToTags('<li class="(?:[^"]* )?tag-type-(?<type>[^" ]+)"[^>]*>(?:[^<]*<a[^>]*>[^<]*</a>)*[^<]*<a[^>]*>(?<name>[^<]*)</a>[^<]*<span[^>]*>(?<count>\\d+)k?</span>[^<]*</li>', src),
                    };
                },
            },
            tags: {
                url: (query: any, opts: any): string => {
                    return "/tag?page=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    return {
                        tags: Grabber.regexToTags('<tr[^>]*>\\s*<td[^>]*>(?<count>\\d+)</td>\\s*<td[^>]*>\\s*<a.+?>\\?</a>\\s*<a[^>]+>(?<name>.*?)</a>\\s*</td>\\s*<td>(?<type>.+?)</td>\\s*<td[^>]*><a href="/tag/edit/(?<id>\\d+)">', src),
                    };
                },
            },
            check: {
                url: (): string => {
                    return "/";
                },
                parse: (src: string): boolean => {
                    return src.indexOf("Running Moebooru") !== -1;
                },
            },
        },
    },
};

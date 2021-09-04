function buildSearch(search: string): string {
    // Replace "ratio:4:3" meta by "4:3_aspect_ratio" tag
    search = search.replace(/(^| )ratio:(\d+:\d+)($| )/g, "$1$2_aspect_ratio$3");

    return search;
}

function buildImageFromJson(img: any): IImage {
    img.created_at = img.created_at["s"];
    img.score = img.total_score;
    img.author = img.author.name;

    return completeImage(img, true);
}

function completeImage(img: IImage, fromJson: boolean = false): IImage {
    if ((!img.file_url || img.file_url.length < 5) && img.preview_url) {
        img.file_url = img.preview_url.replace("/preview/", "/");
    }

    if (img.file_url && !fromJson) {
        img.file_url = img.file_url.replace(/([^s])\.sankakucomplex/, "$1s.sankakucomplex");
    }

    return img;
}

export const source: ISource = {
    name: "Sankaku",
    modifiers: ["rating:safe", "rating:questionable", "rating:explicit", "user:", "fav:", "fastfav:", "md5:", "source:", "id:", "width:", "height:", "score:", "mpixels:", "filesize:", "date:", "gentags:", "arttags:", "chartags:", "copytags:", "approver:", "parent:", "sub:", "order:id", "order:id_desc", "order:score", "order:score_asc", "order:mpixels", "order:mpixels_asc", "order:filesize", "order:landscape", "order:portrait", "order:favcount", "order:rank", "order:change", "order:change_desc", "parent:none", "unlocked:rating"],
    forcedTokens: ["*"],
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
                {
                    key: "appkey",
                    type: "hash",
                    hash: "sha1",
                    salt: "sankakuapp_%pseudo:lower%_Z5NE9YASej",
                },
            ],
            check: {
                type: "max_page",
                value: 50,
            },
        },
        oauth2: {
            type: "oauth2",
            authType: "password_json",
            tokenUrl: "https://capi-v2.sankakucomplex.com/auth/token",
        },
    },
    apis: {
        json: {
            name: "JSON",
            auth: [],
            maxLimit: 200,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): IUrl => {
                    const baseUrl = opts.baseUrl
                        .replace("//chan.", "//capi-v2.")
                        .replace("//idol.", "//iapi.");
                    const pagePart = Grabber.pageUrl(query.page, previous, opts.loggedIn ? 1000 : 50, "page={page}", "prev={max}", "next={min-1}");
                    const search = buildSearch(query.search);
                    const url = baseUrl + "/posts?lang=english&" + pagePart + "&limit=" + opts.limit + "&tags=" + encodeURIComponent(search);
                    return {
                        url,
                        headers: {
                            Accept: "application/vnd.sankaku.api+json;v=2",
                        },
                    };
                },
                parse: (src: string): IParsedSearch => {
                    const data = JSON.parse(src);
                    const images: IImage[] = data.map(buildImageFromJson);
                    return { images };
                },
            },
            details: {
                url: (id: string, md5: string, opts: IUrlDetailsOptions): string => {
                    const baseUrl = opts.baseUrl
                        .replace("//chan.", "//capi-v2.")
                        .replace("//idol.", "//iapi.");
                    return baseUrl + "/posts/" + id;
                },
                parse: (src: string): IParsedDetails => {
                    const data = JSON.parse(src);
                    return buildImageFromJson(data);
                },
            }
        },
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 20,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string | IError => {
                    try {
                        const pagePart = Grabber.pageUrl(query.page, previous, opts.loggedIn ? 50 : 25, "page={page}", "prev={max}", "next={min-1}");
                        const search = buildSearch(query.search);
                        return "/post/index?" + pagePart + "&tags=" + encodeURIComponent(search);
                    } catch (e) {
                        return { error: e.message };
                    }
                },
                parse: (src: string): IParsedSearch => {
                    src = src.replace(/<div class="?popular-preview-post"?>[\s\S]+?<\/div>/g, "");
                    const searchImageCounts = Grabber.regexMatches('class="?tag-(?:count|type-none)"? title="Post Count: (?<count>[0-9,]+)"', src);
                    const lastPage = Grabber.regexToConst("page", '<span class="?current"?>\\s*(?<page>[0-9,]+)\\s*</span>\\s*>>\\s*</div>', src);
                    let wiki = Grabber.regexToConst("wiki", '<div id="?wiki-excerpt"?[^>]*>(?<wiki>.+?)</div>', src);
                    wiki = wiki ? wiki.replace(/href="\/wiki\/show\?title=([^"]+)"/g, 'href="$1"') : undefined;
                    return {
                        tags: Grabber.regexToTags('<li class="?[^">]*tag-type-(?<type>[^">]+)(?:|"[^>]*)>.*?<a href="[^"]+"[^>]*>(?<name>[^<]+)</a>.*?<span class="?post-count"?>(?<count>\\d+)</span>.*?</li>', src),
                        images: Grabber.regexToImages('<span[^>]* id="?p(?<id>\\d+)"?><a[^>]*><img[^>]* src="(?<preview_url>[^"]+/preview/\\w{2}/\\w{2}/(?<md5>[^.]+)\\.[^"]+|[^"]+/download-preview.png)" title="(?<tags>[^"]+)"[^>]+></a></span>', src).map(completeImage),
                        wiki,
                        pageCount: lastPage ? Grabber.countToInt(lastPage) : undefined,
                        imageCount: searchImageCounts.length === 1 ? Grabber.countToInt(searchImageCounts[0].count) : undefined,
                    };
                },
            },
            details: {
                url: (id: string, md5: string): string => {
                    return "/post/show/" + id;
                },
                parse: (src: string): IParsedDetails => {
                    return {
                        pools: Grabber.regexToPools('<div class="status-notice" id="pool\\d+">[^<]*Pool:[^<]*(?:<a href="/post/show/(?<previous>\\d+)" >&lt;&lt;</a>)?[^<]*<a href="/pool/show/(?<id>\\d+)" >(?<name>[^<]+)</a>[^<]*(?:<a href="/post/show/(?<next>\\d+)" >&gt;&gt;</a>)?[^<]*</div>', src),
                        tags: Grabber.regexToTags('<li class="?[^">]*tag-type-(?<type>[^">]+)(?:|"[^>]*)>.*?<a href="[^"]+"[^>]*>(?<name>[^<]+)</a>.*?<span class="?post-count"?>(?<count>\\d+)</span>.*?</li>', src),
                        imageUrl: Grabber.regexToConst("url", '<li>Original: <a href="(?<url>[^"]+)"|<a href="(?<url_2>[^"]+)">Save this file', src).replace(/&amp;/g, "&"),
                        createdAt: Grabber.regexToConst("date", '<a href="/\\?tags=date[^"]+" title="(?<date>[^"]+)">', src),
                    };
                },
            },
            tagTypes: {
                url: (): string => {
                    return "/tag/index";
                },
                parse: (src: string): IParsedTagTypes | IError => {
                    const contents = src.match(/<select[^>]* id=['"]?type['"]?[^>]*>([\s\S]+)<\/select>/);
                    if (!contents) {
                        return { error: "Parse error: could not find the tag type <select> tag" };
                    }
                    const results = Grabber.regexMatches('<option value="?(?<id>\\d+)"?>(?<name>[^<]+)</option>', contents[1]);
                    const types = results.map((r: any) => ({
                        id: r.id,
                        name: r.name.toLowerCase(),
                    }));
                    return { types };
                },
            },
            tags: {
                url: (query: ITagsQuery, opts: IUrlOptions): string => {
                    return "/tag/index?language=en&order=" + query.order + "&page=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    return {
                        tags: Grabber.regexToTags('<tr[^>]*>\\s*<td[^>]*>(?<count>\\d+)</td>\\s*<td class="?tag-type-(?<type>[^">]+)"?>\\s*\\[<a[^>]+>\\?</a>\\]\\s*<a[^>]+>(?<name>.+?)</a>\\s*</td>', src),
                    };
                },
            },
            check: {
                url: (): string => {
                    return "/";
                },
                parse: (src: string): boolean => {
                    return src.indexOf("Sankaku") !== -1;
                },
            },
        },
    },
};

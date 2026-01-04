function buildSearch(search: string): string {
    // Replace "ratio:4:3" meta by "4:3_aspect_ratio" tag
    return search.replace(/(^| )ratio:(\d+:\d+)($| )/g, "$1$2_aspect_ratio$3");
}

function buildImageFromJson(img: any): IImage {
    // Date conversion
    if (img.created_at && typeof img.created_at === "object" && typeof img.created_at.s === "string") {
        img.created_at = img.created_at.s;
    }

    // Score and author
    if (img.total_score !== undefined) {
        img.score = img.total_score;
    }
    if (img.author && img.author.name) {
        img.author = img.author.name;
    }

    // Video metadata (keep as extra filename token)
    if (img.video_duration !== undefined) {
        img.tokens = { ...(img.tokens ?? {}), duration: img.video_duration };
    }

    // File metadata
    if (img.file_ext) {
        img.ext = img.file_ext;
    }

    return completeImage(img as IImage, true);
}

function completeImage(img: IImage, fromJson: boolean): IImage {
    if ((!img.file_url || img.file_url.length < 5) && img.preview_url) {
        img.file_url = img.preview_url.replace("/preview/", "/");
    }
    if (img.file_url && !fromJson) {
        img.file_url = img.file_url.replace(/([^s])\.sankakucomplex/, "$1s.sankakucomplex");
    }
    return img;
}

const tagTypeById: Record<number, string> = {
    0: "general",
    1: "artist",
    2: "studio",
    3: "copyright",
    4: "character",
    5: "species",
    8: "medium",
    9: "meta",
};

function applyTagTypes(tags: ITag[]): ITag[] {
    for (const tag of tags) {
        const rawTypeId = (tag as any).typeId;
        const typeId = typeof rawTypeId === "string" ? parseInt(rawTypeId, 10) : rawTypeId;
        if (typeof typeId === "number") {
            tag.type = tagTypeById[typeId] ?? "unknown";
        } else {
            tag.type = "unknown";
        }
    }
    return tags;
}

export const source: ISource = {
    name: "Sankaku",
    modifiers: [
        "rating:safe", "rating:questionable", "rating:explicit",
        "user:", "fav:", "fastfav:", "md5:", "source:", "id:",
        "width:", "height:", "score:", "mpixels:", "filesize:", "date:",
        "gentags:", "arttags:", "chartags:", "copytags:", "approver:",
        "parent:", "sub:",
        "order:id", "order:id_desc",
        "order:score", "order:score_asc",
        "order:popularity", "order:quality",
        "order:mpixels", "order:mpixels_asc",
        "order:filesize",
        "order:landscape", "order:portrait",
        "order:favcount", "order:rank",
        "order:change", "order:change_desc",
        "parent:none", "unlocked:rating", "threshold:",
    ],
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
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): IRequest => {
                    const isIdol = opts.baseUrl.indexOf("idol") !== -1;
                    const baseUrl = isIdol ? "https://i.sankakuapi.com" : "https://sankakuapi.com";
                    const threshold = isIdol ? 2 : 5;
                    const pagePart = Grabber.pageUrl(query.page, previous, opts.loggedIn ? 100 : 50, "page={page}", "prev={max}", "next={min-1}");
                    const search = buildSearch(query.search);
                    const url = baseUrl + "/v2/posts/keyset?lang=en&default_threshold=" + threshold + "&hide_posts_in_books=in-larger-tags&limit=" + opts.limit + "&" + pagePart + "&tags=" + encodeURIComponent(search);
                    return {
                        url,
                        headers: {
                            Accept: "application/vnd.sankaku.api+json;v=2",
                        },
                    };
                },
                parse: (src: string, _statusCode: number): IParsedSearch => {
                    const data = JSON.parse(src);
                    const posts: any[] = Array.isArray(data) ? data : (data.data || data.posts || []);

                    const images: IImage[] = [];
                    for (const post of posts) {
                        try {
                            images.push(buildImageFromJson(post));
                        } catch {
                            // Skip invalid posts
                        }
                    }

                    const tags = Grabber.regexToTags('"type":(?<typeId>\\d+).*?"(?:name_en|tagName|name)":"(?<name>[^"]+)".*?"(?:post_)?count":(?<count>\\d+)', src) as ITag[];
                    applyTagTypes(tags);

                    return {
                        images,
                        tags,
                    };
                },
            },
            details: {
                url: (id: string, _md5: string, opts: IUrlDetailsOptions): string => {
                    const isIdol = opts.baseUrl.indexOf("idol") !== -1;
                    const baseUrl = isIdol ? "https://i.sankakuapi.com" : "https://sankakuapi.com";
                    return baseUrl + "/v2/posts/" + id;
                },
                parse: (src: string, _statusCode: number): IParsedDetails => {
                    const tags = Grabber.regexToTags('"type":(?<typeId>.*?),.*?"count":(?<count>.*?),.*?"name":"(?<name>.*?)"}', src) as ITag[];
                    applyTagTypes(tags);
                    return { tags };
                },
            },
        },
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 20,
            forcedTokens: ["*"],
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string | IError => {
                    try {
                        const pagePart = Grabber.pageUrl(query.page, previous, opts.loggedIn ? 50 : 25, "page={page}", "prev={max}", "next={min-1}");
                        const search = buildSearch(query.search);
                        return "/post/index?" + pagePart + "&tags=" + encodeURIComponent(search);
                    } catch (e: any) {
                        return { error: e && e.message ? e.message : String(e) };
                    }
                },
                parse: (src: string, _statusCode: number): IParsedSearch => {
                    const cleanedSrc = src.replace(/<div class="?popular-preview-post"?>[\s\S]+?<\/div>/g, "");
                    const searchImageCounts = Grabber.regexMatches('class="?tag-(?:count|type-none)"? title="Post Count: (?<count>[0-9,]+)"', cleanedSrc);
                    const lastPage = Grabber.regexToConst("page", '<span class="?current"?>\\s*(?<page>[0-9,]+)\\s*<\/span>\\s*>>\\s*<\/div>', cleanedSrc);
                    let wiki = Grabber.regexToConst("wiki", '<div id="?wiki-excerpt"?[^>]*>(?<wiki>.+?)<\/div>', cleanedSrc);
                    wiki = wiki ? wiki.replace(/href="\/wiki\/show\?title=([^"]+)"/g, 'href="$1"') : undefined;

                    return {
                        tags: Grabber.regexToTags('<li class="?[^">]*tag-type-(?<type>[^">]+)(?:|"[^>]*)>.*?<a href="[^"]+"[^>]*>(?<name>[^<]+)<\/a>.*?<span class="?post-count"?>(?<count>\\d+)<\/span>.*?<\/li>', cleanedSrc),
                        images: Grabber.regexToImages('<span[^>]* id="?p(?<id>\\d+)"?><a[^>]*><img[^>]* src="(?<preview_url>[^"]+\/preview\/\\w{2}\/\\w{2}\/((?<md5>[^.]+))\\.[^"]+|[^"]+\/download-preview.png)" title="(?<tags>[^"]+)"[^>]+><\/a><\/span>', cleanedSrc).map((img: IImage) => completeImage(img, false)),
                        wiki,
                        pageCount: lastPage ? Grabber.countToInt(lastPage) : undefined,
                        imageCount: searchImageCounts.length === 1 ? Grabber.countToInt(searchImageCounts[0].count) : undefined,
                    };
                },
            },
            details: {
                url: (id: string, _md5: string): string => "/post/show/" + id,
                parse: (src: string, _statusCode: number): IParsedDetails => {
                    return {
                        pools: Grabber.regexToPools('<div class="status-notice" id="pool\\d+">[^<]*Pool:[^<]*(?:<a href="/post/show/(?<previous>\\d+)" >&lt;&lt;<\/a>)?[^<]*<a href="/pool/show/(?<id>\\d+)" >(?<name>[^<]+)<\/a>[^<]*(?:<a href="/post/show/(?<next>\\d+)" >&gt;&gt;<\/a>)?[^<]*<\/div>', src),
                        tags: Grabber.regexToTags('<li class="?[^">]*tag-type-(?<type>[^">]+)(?:|"[^>]*)>.*?<a href="[^"]+"[^>]*>(?<name>[^<]+)<\/a>.*?<span class="?post-count"?>(?<count>\\d+)<\/span>.*?<\/li>', src),
                        imageUrl: Grabber.regexToConst("url", '<li>Original: <a href="(?<url>[^"]+)"|<a href="(?<url_2>[^"]+)">Save this file', src).replace(/&amp;/g, "&"),
                        createdAt: Grabber.regexToConst("date", '<a href="/\\?tags=date[^"]+" title="(?<date>[^"]+)">', src),
                    };
                },
            },
            tagTypes: {
                url: (): string => "/tag/index",
                parse: (src: string, _statusCode: number): IParsedTagTypes | IError => {
                    const contents = src.match(/<select[^>]* id=['"]?type['"]?[^>]*>([\s\S]+)<\/select>/);
                    if (!contents) {
                        return { error: "Parse error: could not find the tag type <select> tag" };
                    }
                    const results = Grabber.regexMatches('<option value="?(?<id>\\d+)"?>(?<name>[^<]+)<\/option>', contents[1]);
                    const types = results.map((r: any) => ({
                        id: parseInt(r.id, 10),
                        name: String(r.name).toLowerCase(),
                    }));
                    return { types };
                },
            },
            tags: {
                url: (query: ITagsQuery, _opts: IUrlOptions): string => "/tag/index?language=en&order=" + query.order + "&page=" + query.page,
                parse: (src: string, _statusCode: number): IParsedTags => {
                    return {
                        tags: Grabber.regexToTags('<tr[^>]*>\\s*<td[^>]*>(?<count>\\d+)<\/td>\\s*<td class="?tag-type-(?<type>[^">]+)"?>\\s*\\[<a[^>]+>\\?<\/a>\\]\\s*<a[^>]+>(?<name>.+?)<\/a>\\s*<\/td>', src),
                    };
                },
            },
            check: {
                url: (): string => "/",
                parse: (src: string, _statusCode: number): boolean => src.indexOf("Sankaku") !== -1,
            },
        },
    },
};

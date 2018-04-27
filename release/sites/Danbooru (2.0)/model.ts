function completeImage(img: IImage): IImage {
    if (img["ext"] && img["ext"][0] === ".") {
        img["ext"] = img["ext"].mid(1);
    }

    if (!img["file_url"] || img["file_url"].length < 5) {
        img["file_url"] = `/data/${img["md5"]}.${img["ext"]}`;
    } else {
        img["file_url"] = img["file_url"]
            .replace("/preview/", "/")
            .replace("/ssd/", "/")
            .replace("/sample/[^.]*sample-", "/");
    }

    if (!img["sample_url"] || img["sample_url"].length < 5) {
        img["sample_url"] = `/data/sample/sample-${img["md5"]}.jpg`;
    }

    if (!img["preview_url"] || img["preview_url"].length < 5) {
        img["preview_url"] = `/data/preview/${img["md5"]}.jpg`;
    }

    return img;
}

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

export const source: ISource = {
    name: "Danbooru (2.0)",
    modifiers: ["rating:safe", "rating:questionable", "rating:explicit", "rating:s", "rating:q", "rating:e", "user:", "fav:", "fastfav:", "md5:", "source:", "id:", "width:", "height:", "score:", "mpixels:", "filesize:", "date:", "gentags:", "arttags:", "chartags:", "copytags:", "approver:", "parent:", "sub:", "status:any", "status:deleted", "status:active", "status:flagged", "status:pending", "order:id", "order:id_desc", "order:score", "order:score_asc", "order:mpixels", "order:mpixels_asc", "order:filesize", "order:landscape", "order:portrait", "order:favcount", "order:rank", "order:change", "order:change_desc", "parent:none", "unlocked:rating"],
    forcedTokens: ["filename"],
    tagFormat: {
        case: "lower",
        wordSeparator: "_",
    },
    auth,
    apis: {
        json: {
            name: "JSON",
            auth: [],
            maxLimit: 200,
            search: {
                url: (query: any, opts: any, previous: any): string => {
                    const loginPart = Grabber.loginUrl(auth.url.fields, opts["auth"]);
                    const pagePart = Grabber.pageUrl(query.page, previous, 1000, "{page}", "a{max}", "b{min}");
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
                        images.push(completeImage(Grabber.mapFields(image, map)));
                    }

                    return { images };
                },
            },
            tags: {
                url: (query: any, opts: any): string => {
                    const loginPart = Grabber.loginUrl(auth.url.fields, opts["auth"]);
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
                    const pagePart = Grabber.pageUrl(query.page, previous, 1000, "{page}", "a{max}", "b{min}");
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
                        images.push(completeImage(Grabber.mapFields(image, map)));
                    }

                    return { images };
                },
            },
            tags: {
                url: (query: any, opts: any): string => {
                    const loginPart = Grabber.loginUrl(auth.url.fields, opts["auth"]);
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
                    const pagePart = Grabber.pageUrl(query.page, previous, 1000, "{page}", "a{max}", "b{min}");
                    return "/posts?" + loginPart + "limit=" + opts.limit + "&page=" + pagePart + "&tags=" + query.search;
                },
                parse: (src: string): IParsedSearch => {
                    return {
                        tags: Grabber.regexToTags('<li class="category-(?<typeId>[^"]+)">(?:\\s*<a class="wiki-link" href="[^"]+">\\?</a>)?\\s*<a class="search-tag"\\s+[^>]*href="[^"]+"[^>]*>(?<name>[^<]+)</a>\\s*<span class="post-count">(?<count>[^<]+)</span>\\s*</li>', src),
                        images: Grabber.regexToImages('<article[^>]* id="[^"]*" class="[^"]*"\\s+data-id="(?<id>[^"]*)"\\s+data-has-sound="[^"]*"\\s+data-tags="(?<tags>[^"]*)"\\s+data-pools="(?<pools>[^"]*)"\\s+data-uploader="(?<author>[^"]*)"\\s+data-approver-id="(?<approver>[^"]*)"\\s+data-rating="(?<rating>[^"]*)"\\s+data-width="(?<width>[^"]*)"\\s+data-height="(?<height>[^"]*)"\\s+data-flags="(?<flags>[^"]*)"\\s+data-parent-id="(?<parent_id>[^"]*)"\\s+data-has-children="(?<has_children>[^"]*)"\\s+data-score="(?<score>[^"]*)"\\s+data-views="[^"]*"\\s+data-fav-count="(?<fav_count>[^"]*)"\\s+data-pixiv-id="[^"]*"\\s+data-file-ext="(?<ext>[^"]*)"\\s+data-source="[^"]*"\\s+data-normalized-source="[^"]*"\\s+data-is-favorited="[^"]*"\\s+data-md5="(?<md5>[^"]*)"\\s+data-file-url="(?<file_url>[^"]*)"\\s+data-large-file-url="(?<sample_url>[^"]*)"\\s+data-preview-file-url="(?<preview_url>[^"]*)"', src).map(completeImage),
                        wiki: Grabber.regexToConst("wiki", '<div id="excerpt"(?:[^>]+)>(?<wiki>.+?)</div>', src),
                        pageCount: Grabber.regexToConst("page", '>(?<page>[0-9]+)</a></li><li[^<]*><a[^>]* rel="next"', src),
                    };
                },
            },
            details: {
                url: (id: number, md5: string): string => {
                    return "/posts/" + id;
                },
                parse: (src: string): IParsedDetails => {
                    return {
                        pools: Grabber.regexToPools('<div class="status-notice" id="pool\\d+">[^<]*Pool:[^<]*(?:<a href="/post/show/(?<previous>\\d+)" >&lt;&lt;</a>)?[^<]*<a href="/pool/show/(?<id>\\d+)" >(?<name>[^<]+)</a>[^<]*(?:<a href="/post/show/(?<next>\\d+)" >&gt;&gt;</a>)?[^<]*</div>', src),
                        tags: Grabber.regexToTags('<li class="category-(?<typeId>[^"]+)">(?:\\s*<a class="wiki-link" href="[^"]+">\\?</a>)?\\s*<a class="search-tag"\\s+[^>]*href="[^"]+"[^>]*>(?<name>[^<]+)</a>\\s*<span class="post-count">(?<count>[^<]+)</span>\\s*</li>', src),
                        imageUrl: Grabber.regexToConst("url", 'Size: <a href="(?<url>[^"]+)\\?download=1"', src),
                    };
                },
            },
            tags: {
                url: (query: any, opts: any): string => {
                    const loginPart = Grabber.loginUrl(auth.url.fields, opts["auth"]);
                    return "/tags?" + loginPart + "limit=" + opts.limit + "&page=" + query.page;
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
                        || src.search(/Running Danbooru <a[^>]*>v2/) !== -1;
                },
            },
        },
    },
};

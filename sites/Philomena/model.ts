function completeImage(img: IImage & { json_uris: string }): IImage {
    if (img.json_uris) {
        const uris = JSON.parse(img.json_uris.replace(/&quot;/g, '"'));
        if ("thumb_small" in uris && uris["thumb_small"].length > 5) {
            img.preview_url = uris["thumb_small"];
        }
        if ("large" in uris && uris["large"].length > 5) {
            img.sample_url = uris["large"];
        }
        if ("full" in uris && uris["full"].length > 5) {
            img.file_url = uris["full"];
        }
    }

    if (!img.preview_url && img.file_url.length >= 5) {
        img.preview_url = img.file_url
            .replace("full", "thumb")
            .replace(".svg", ".png");
    }

    return img;
}

function makeTags(tags: string[], tagIds: number[]): ITag[] {
    const ret: ITag[] = [];
    for (const i in tags) {
        ret.push({
            id: tagIds[i],
            name: tags[i],
        })
    }
    return ret;
}

function searchToArg(search: string): string {
    let sf: string;
    let sd = "desc";
    const tags = [];

    const parts = search.split(" ");
    for (const tag of parts) {
        const part = tag.trim();
        if (part.indexOf("order:") === 0) {
            const orders = part.substr(6).split("_");
            sf = orders[0];
            if (orders.length > 1) {
                sd = orders[1];
            }
        } else {
            tags.push(part);
        }
    }

    let ret = encodeURIComponent(tags.join(" ") || "*");
    if (sf) {
        ret += "&sf=" + sf;
        if (sd) {
            ret += "&sd=" + sd;
        }
    }
    return ret;
}

export const source: ISource = {
    name: "Philomena",
    modifiers: ["faved_by:", "width:", "height:", "uploader:", "source_url:", "description:", "sha512_hash:", "aspect_ratio:"],
    forcedTokens: [],
    tagFormat: {
        case: "lower",
        wordSeparator: " ",
    },
    searchFormat: {
        and: " AND ",
        or: " OR ",
        parenthesis: true,
        precedence: "and",
    },
    auth: {
        url: {
            type: "url",
            fields: [
                {
                    id: "apiKey",
                    key: "key",
                    type: "password",
                },
            ],
        },
    },
    apis: {
        json: {
            name: "JSON",
            auth: [],
            maxLimit: 50,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions): string => {
                    return "/api/v1/json/search/images?per_page=" + opts.limit + "&page=" + query.page + "&q=" + searchToArg(query.search);
                },
                parse: (src: string): IParsedSearch => {
                    const map = {
                        "created_at": "created_at",
                        "source": "source_url",
                        "width": "width",
                        "md5": "sha512_hash",
                        "height": "height",
                        "creator_id": "uploader_id",
                        "id": "id",
                        "ext": "format",
                        "file_url": "representations.full",
                        "sample_url": "representations.large",
                        "preview_url": "representations.thumb",
                        "author": "uploader",
                        "score": "score",
                    };

                    const data = JSON.parse(src);

                    const images: IImage[] = [];
                    for (const image of data["images"]) {
                        const img = Grabber.mapFields(image, map);
                        img.tags = makeTags(image["tags"], image["tag_ids"]);
                        img.has_comments = image["comment_count"] > 0;
                        images.push(completeImage(img));
                    }

                    return {
                        images,
                        imageCount: data.total,
                    };
                },
            },
            details: {
                url: (id: string): string => {
                    return "/api/v1/json/images/" + id;
                },
                parse: (src: string): IParsedDetails => {
                    const data = JSON.parse(src);
                    return {
                        createdAt: data["created_at"],
                        imageUrl: data["representations"]["full"],
                        tags: makeTags(data["tags"], data["tag_ids"]),
                    };
                },
            },
            tags: {
                url: (query: ITagsQuery, opts: IUrlOptions): string => {
                    return "/api/v1/json/search/tags?per_page=" + opts.limit + "&page=" + query.page + "&q=*";
                },
                parse: (src: string): IParsedTags => {
                    const map = {
                        "id": "id",
                        "name": "name",
                        "count": "images",
                        "type": "category",
                    };

                    const data = JSON.parse(src);

                    const tags: ITag[] = [];
                    for (const tag of data["tags"]) {
                        tags.push(Grabber.mapFields(tag, map));
                    }

                    return { tags };
                },
            },
        },
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 15,
            search: {
                url: (query: ISearchQuery): string => {
                    if (!query.search || query.search.length === 0) {
                        return "/images/page/" + query.page;
                    }
                    return "/search?page=" + query.page + "&sbq=" + searchToArg(query.search);
                },
                parse: (src: string): IParsedSearch => {
                    return {
                        images: Grabber.regexToImages('<div class="image-container[^"]*" data-aspect-ratio="[^"]*" data-comment-count="(?<comments>[^"]*)" data-created-at="(?<created_at>[^"]*)" data-downvotes="[^"]*" data-faves="(?<favorites>[^"]*)" data-height="(?<height>[^"]*)" data-image-id="(?<id>[^"]*)" data-image-tag-aliases="(?<tags>[^"]*)" data-image-tags="[^"]*" data-score="(?<score>[^"]*)" data-size="[^"]*" data-source-url="(?<source>[^"]*)" data-upvotes="[^"]*" data-uris="(?<json_uris>[^"]*)" data-width="(?<width>[^"]*)">.*?<a[^>]*><picture><img[^>]* src="(?<preview_url>[^"]*)"[^>]*>', src).map(completeImage),
                        pageCount: Grabber.regexToConst("page", '<a href="(?:/images/page/|/tags/[^\\?]*\\?[^"]*page=|/search/index\\?[^"]*page=)(?<image>\\d+)[^"]*">Last', src),
                        imageCount: Grabber.regexToConst("count", "of <strong>(?<count>[^<]+)</strong> total", src),
                    };
                },
            },
            details: {
                url: (id: string, md5: string): string => {
                    return "/" + id;
                },
                parse: (src: string): IParsedDetails => {
                    return {
                        tags: Grabber.regexToTags('<span class="tag dropdown"(?: data-tag-category="(?<type>[^"]*)")? data-tag-id="(?<id>[^"]+)" data-tag-name="(?<name>[^"]+)" data-tag-slug="[^"]+">', src),
                    };
                },
            },
            tags: {
                url: (query: ITagsQuery): string => {
                    return "/tags?page=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    return {
                        tags: Grabber.regexToTags('<span class="tag dropdown"(?: data-tag-category="(?<type>[^"]+)")? data-tag-id="(?<id>\\d+)" data-tag-name="(?<name>.+?)".+?<span class="tag__count">\\s*\\((?<count>\\d+)\\)</span>', src),
                    };
                },
            },
            check: {
                url: (): string => {
                    return "/";
                },
                parse: (src: string): boolean => {
                    return src.indexOf("philomena project") !== -1;
                },
            },
        },
    },
};

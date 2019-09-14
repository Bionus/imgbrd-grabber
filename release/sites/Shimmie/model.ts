function completeImage(img: IImage): IImage {
    if (img.ext && img.ext[0] === ".") {
        img.ext = img.ext.substr(1);
    }

    const hasMd5 = img.md5 && img.md5.length > 0;

    if (!img.file_url || img.file_url.length < 5) {
        img.file_url = hasMd5
            ? `/_images/${img.md5}.${img.ext || "jpg"}`
            : `/_images/${img.id}.${img.ext || "jpg"}`;
    }

    if (!img.preview_url || img.preview_url.length < 5) {
        img.preview_url = hasMd5
            ? `/_thumbs/${img.md5}.jpg`
            : `/_thumbs/${img.id}.jpg`;
    }

    return img;
}

export const source: ISource = {
    name: "Shimmie",
    modifiers: [],
    forcedTokens: ["filename", "date"],
    tagFormat: {
        case: "lower",
        wordSeparator: "_",
    },
    searchFormat: {
        and: " ",
    },
    auth: {
        session: {
            type: "post",
            url: "/user_admin/login",
            fields: [
                {
                    id: "pseudo",
                    key: "user",
                },
                {
                    id: "password",
                    key: "pass",
                    type: "password",
                },
            ],
            check: {
                type: "cookie",
                key: "shm_session",
            },
        },
    },
    apis: {
        rss: {
            name: "RSS",
            auth: [],
            search: {
                url: (query: any, opts: any, previous: any): IError | string => {
                    if (query.search.length > 0) {
                        return { error: "Tag search is impossible with Shimmie RSS API." };
                    }
                    return "/rss/images/" + query.page;
                },
                parse: (src: string): IParsedSearch => {
                    const data = Grabber.makeArray(Grabber.parseXML(src).rss.channel.item);

                    const images: IImage[] = [];
                    for (const image of data) {
                        const img: IImage = {
                            page_url: image["link"]["#text"],
                            preview_url: image["media:thumbnail"]["#text"] || image["media:thumbnail"]["@attributes"]["url"],
                            file_url: image["media:content"]["#text"] || image["media:content"]["@attributes"]["url"],
                            created_at: image["pubDate"]["#text"],
                        };

                        const txt = image["title"]["#text"];
                        const info = (Array.isArray(txt) ? txt.join(" ") : txt).split(" - ");
                        if (info.length === 2) {
                            img.id = parseInt(info[0], 10);
                            img.tags = info[1].toLowerCase().split(" ");
                        } else {
                            img.id = Grabber.regexToConst("id", "/(?<id>\\d+)", img.page_url);
                        }

                        // Some additional fields can be found parsing the HTML description
                        const desc = image["description"]["#text"];
                        const matches = Grabber.regexMatches(" // (?<width>\\d+)x(?<height>\\d+) // (?<filesize>[^'\" /]*?)(?: // (?<ext>[^'\"&]*))?['\"&]", desc);
                        if (matches && matches.length > 0) {
                            const match = matches[0];
                            img.width = match["width"];
                            img.height = match["height"];
                            img.file_size = match["filesize"];
                            if (match["ext"]) {
                                img.ext = match["ext"];
                            }
                        }

                        images.push(completeImage(img));
                    }

                    return { images };
                },
            },
        },
        html: {
            name: "Regex",
            auth: [],
            search: {
                url: (query: any, opts: any, previous: any): string => {
                    if (query.search.length > 0) {
                        return "/post/list/" + query.search + "/" + query.page;
                    }
                    return "/post/list/" + query.page;
                },
                parse: (src: string): IParsedSearch => {
                    const pageCount = /\| Next \| Last<br>/.test(src)
                        ? Grabber.regexToConst("page", "<a href=['\"]/post/list(?:/[^/]+)?/(?<page>\\d+)['\"]>[^<]+<\/a>(?:<\/b>)? &gt;&gt;", src)
                        : Grabber.regexToConst("page", "<a href=['\"]/post/list(?:/[^/]+)?/(?<page>\\d*)['\"]>Last</a>", src);
                    return {
                        tags: Grabber.regexToTags('<li class="tag-type-(?<type>[^"]+)">[^<]*<a href="[^"]+">[^<]*</a>[^<]*<a href="[^"]+">(?<name>[^<]+)</a>[^<]*</li>|<a class=[\'"]tag_name[\'"] href=[\'"]([^\'"]+)(?:/1)?[\'"]>(?<name_2>[^<]+)</a>(?:</td><td class=[\'"]tag_count_cell[\'"]>[^<]*<span class=[\'"]tag_count[\'"]>(?<count>\\d+)</span>)?', src),
                        images: Grabber.regexToImages("<a(?: class=['\"][^'\"]*['\"])? href=['\"][^'\">]*/post/view/(?<id>[^'\"]+)['\"][^>]*>[^<]*(?<image><img(?: id=['\"](?:[^'\"]*)['\"])? title=['\"](?<tags>[^'\"/]*)(?: // (?<width>[^'\"/]+?)x(?<height>[^'\"/]+?) // (?<filesize>[^'\"/]*?)(?: // (?<ext>[^'\"/]*?))?)?['\"] alt=['\"][^'\"]*['\"](?: height=['\"][^'\"]*['\"])? width=['\"][^'\"]*['\"](?: height=['\"][^'\"]*['\"])?[^>]*(?:src|data-original)=['\"][^'\"]*(?<preview_url>/_thumbs/(?<md5>[^/]*)/[^'\"]*\\.jpg|/thumb/(?<md5_2>[^'\"]*)\\.jpg|questionable\\.jpg)['\"][^>]*>).*?</a>|<a href=['\"][^'\">]*/i(?<id_2>[^'\"]+)['\"](?: class=['\"][^'\"]*['\"])?[^>]*>[^<]*(?<image_2><img(?: id=['\"](?:[^'\"]*)['\"])? src=['\"][^'\"]*(?<preview_url_2>/_thumbs/(?<md5_3>[^'\"]*)(?:_th)?\\.jpg|/thumb/(?<md5_4>[^'\"]*)\\.jpg|questionable\\.jpg)['\"] title=['\"](?<tags_2>[^'\"/]+) // (?<width_2>[^'\"]+)x(?<height_2>[^'\"]+) // (?<filesize_2>[^'\"]*)(?: // (?<ext_2>[^'\"]*))?['\"] alt=['\"][^'\"]*['\"] ?/?>)[^<]*</a>", src).map(completeImage),
                        pageCount,
                    };
                },
            },
            details: {
                url: (id: number, md5: string): string => {
                    return "/post/view/" + id;
                },
                parse: (src: string): IParsedDetails => {
                    let tags: ITag[] | string[];
                    const leftTagBlock = src.match(/<section[^>]*><h3[^>]*>Tags<\/h3>([\s\S]+?)<\/section>/);
                    if (leftTagBlock) {
                        tags = Grabber.regexToTags('<li class="tag-type-(?<type>[^"]+)">[^<]*<a href="[^"]+">[^<]*</a>[^<]*<a href="[^"]+">(?<name>[^<]+)</a>[^<]*</li>|<a class=[\'"]tag_name[\'"] href=[\'"]([^\'"]+)(?:/1)?[\'"]>(?<name_2>[^<]+)</a>(?:</td><td class=[\'"]tag_count_cell[\'"]>[^<]*<span class=[\'"]tag_count[\'"]>(?<count>\\d+)</span>)?', leftTagBlock[1]);
                    } else {
                        const bottomTagsBlock = src.match(/<tr>\s*<th[^>]*>Tags<\/th>\s*<td>([\s\S]*?)<\/td>\s*<\/tr>/);
                        if (bottomTagsBlock) {
                            tags = Grabber.regexToTags("<a[^>]*>(?<name>[^<]+)</a>", bottomTagsBlock[1]);
                        }
                    }
                    return {
                        tags,
                        imageUrl: Grabber.regexToConst("url", "<img.+?id=['\"]main_image['\"] src=['\"](?<url>[^']+)['\"][^>]*>", src),
                        createdAt: Grabber.regexToConst("date", "<time datetime=['\"](?<date>[^'\"]+)['\"]>", src),
                    };
                },
            },
            check: {
                url: (): string => {
                    return "/";
                },
                parse: (src: string): boolean => {
                    return src.indexOf("Running Shimmie") !== -1
                        || src.indexOf("Shimmie version ") !== -1
                        || src.search(/Running <a href=['"][^'"]+['"]>Shimmie<\/a>/) !== -1;
                },
            },
        },
    },
};

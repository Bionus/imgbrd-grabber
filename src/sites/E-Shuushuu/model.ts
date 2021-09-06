function parseTags(str: string, type: string): ITag[] {
    const tags: ITag[] = Grabber.regexToTags(`<span class='tag'>"<a href="/tags/(?<id>\\d+)">(?<name>[^<]+)</a>"</span>`, str);
    for (const tag of tags) {
        tag.type = type;
    }
    return tags;
}

export const source: ISource = {
    name: "E-Shuushuu",
    modifiers: [],
    forcedTokens: ["*"],
    tagFormat: {
        case: "lower",
        wordSeparator: " ",
    },
    searchFormat: {
        and: "+",
    },
    apis: {
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 15,
            search: {
                parseErrors: true,
                url: (query: ISearchQuery): string => {
                    if (query.search.length === 0) {
                        return "/?page=" + query.page;
                    }
                    const tagIds = query.search.split(" ").join("+");
                    return "/search/results/?page=" + query.page + "tags=" + tagIds;
                },
                parse: (src: string): IParsedSearch | IError => {
                    const images = Grabber.regexToImages('<div class="image_block">.+?<a href="/image/(?<id>\\d+)/">.*?<a class="thumb_image" href="(?<file_url>[^"]+)"[^>]*>.*?<img src="(?<preview_url>[^"]+)"[^>]*>.*?<dl>(?<metadata>.+?)</dl>', src);
                    for (const img of images) {
                        const metadata = img.metadata;
                        delete img.metadata;

                        const matches = Grabber.regexMatches("<dt>\\s*([^:<]+):\\s*</dt>\\s*<dd[^>]*>\\s*(.+?)\\s*</dd>", metadata);
                        const tags = [];
                        for (const match of matches) {
                            const key = match[1];
                            const val = match[2];
                            if (key === "Submitted By") {
                                img.author = val.match(/<span[^>]*>([^<]+)<\/span>/)[1];
                            } else if (key === "Submitted On") {
                                img.created_at = val;
                            } else if (key === "File size") {
                                img.created_at = Grabber.fileSizeToInt(val);
                            } else if (key === "Dimensions") {
                                const dims = val.match(/(\d+)x(\d+)/);
                                img.width = dims[1];
                                img.height = dims[2];
                            } else if (key === "Tag" || key === "Tags") {
                                tags.push(...parseTags(val, "general"));
                            } else if (key === "Source" || key === "Sources") {
                                tags.push(...parseTags(val, "copyright"));
                            } else if (key === "Character" || key === "Characters") {
                                tags.push(...parseTags(val, "character"));
                            } else if (key === "Artist" || key === "Artists") {
                                tags.push(...parseTags(val, "artist"));
                            }
                        }
                        img.tags = tags;
                    }
                    return { images };
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
                    return src.indexOf("Contact Webmaster at admin at e-shuushuu.net") !== -1;
                },
            },
        },
    },
};

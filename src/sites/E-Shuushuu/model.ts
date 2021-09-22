function parseTags(str: string, type: string): ITag[] {
    const tags: ITag[] = Grabber.regexToTags(`<span class='tag'>"<a href="/tags/(?<id>\\d+)">(?<name>[^<]+)</a>"</span>`, str);
    for (const tag of tags) {
        tag.type = type;
    }
    return tags;
}

// Transforms a parsed query into a "ID1+ID2" format
function buildQuery(search: IParsedSearchQuery): string {
    return Grabber.visitSearch(
        search,
        (tag: ITag) => {
            if (!tag.id) {
                throw new Error(`Tag ID not found for "${tag.name}"`);
            }
            return String(tag.id)
        },
        (left: string, right: string) => left + "+" + right,
        () => { throw new Error("OR operand not supported"); },
    );
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
                parseInput: true,
                parseErrors: true,
                url: (query: ISearchQuery): string => {
                    if (query.search.length === 0 || !query.parsedSearch) {
                        return "/?page=" + query.page;
                    }
                    return "/search/results/?page=" + query.page + "&tags=" + buildQuery(query.parsedSearch);
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
                    return "/tags/?page=" + query.page + "&type=1&show_aliases=1";
                },
                parse: (src: string): IParsedTags => {
                    return {
                        tags: Grabber.regexToTags('<li><a href="/tags/(?<id>\\d+)">(?<name>[^<]+)</a></li>', src).map((tag: ITag) => {
                            tag.typeId = 1;
                            return tag;
                        }),
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

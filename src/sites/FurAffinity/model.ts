export const source: ISource = {
    name: "FurAffinity",
    tagFormat: {
        case: "lower",
        wordSeparator: "_",
    },
    searchFormat: {
        and: " ",
        or: " | ",
        parenthesis: true,
        precedence: "or",
    },
    apis: {
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 24,
            forcedTokens: ["*"],
            search: {
                parseErrors: true,
                url: (query: ISearchQuery): string | IError => {
                    const perpage = 24; // opts.limit <= 36 ? 24 : (opts.limit > 60 ? 72 : 48);
                    if (query.search.length === 0) {
                        return "/browse/?order-by=date&page=" + query.page + "&perpage=" + perpage;
                    }
                    return "/search/?q=" + encodeURIComponent(query.search) + "&order-by=date&page=" + query.page + "&perpage=" + perpage;
                },
                parse: (src: string, statusCode: number): IParsedSearch | IError => {
                    return {
                        images: Grabber.regexToImages('<figure id="sid-(?<id>\\d+)" class="r-(?<rating>[^"]+) t-image u-(?<author>[^" ]+)[^"]*"[^>]*>.+?<img.+?src="(?<preview_url>[^"]+)"\\s*data-width="(?<preview_width>[0-9.]+)"\\s*data-height="(?<preview_height>[0-9.]+)"', src),
                        imageCount: Grabber.regexToConst("count", "\\(\\d+ - \\d+ of (?<count>\\d+)\\)", src),
                    };
                },
            },
            details: {
                url: (id: string, md5: string): string => {
                    return "/view/" + id + "/";
                },
                parse: (src: string): IParsedDetails => {
                    return {
                        tags: Grabber.regexToTags('<a href="/search/@keywords [^"]+">(?<name>[^<]+)</a>', src),
                        createdAt: Grabber.regexToConst("date", '<strong><span title="(?<date>[^"]+)" class="popup_date">', src),
                        imageUrl: Grabber.regexToConst("url", '<a href="(?<url>[^"]+)">(?:Download|Download Submission)</a>', src),
                    };
                },
            },
            check: {
                url: (): string => {
                    return "/";
                },
                parse: (src: string): boolean => {
                    return src.indexOf("Fur Affinity is &copy;") !== -1
                        || src.indexOf("Fur Affinity [dot] net") !== -1;
                },
            },
        },
    },
};

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
            search: {
                parseErrors: true,
                url: (query: any, opts: any, previous: any): string | IError => {
                    return "/search/?q=" + encodeURIComponent(query.search) + "&order-by=date&page=" + query.page + "&perpage=24";
                },
                parse: (src: string, statusCode: number): IParsedSearch | IError => {
                    return {
                        images: Grabber.regexToImages('<figure id="sid-(?<id>\\d+)" class="r-(?<rating>[^"]+) t-image u-(?<author>[^"]+)">.+?<img.+?src="(?<preview_url>[^"]+)"\\s*data-width="(?<preview_width>[0-9.]+)"\\s*data-height="(?<preview_height>[0-9.]+)"', src),
                        imageCount: Grabber.regexToConst("count", "<strong>Search results</strong> \\(\\d+ - \\d+ of (?<count>\\d+)\\)", src),
                    };
                },
            },
            details: {
                url: (id: number, md5: string): string => {
                    return "/view/" + id + "/";
                },
                parse: (src: string): IParsedDetails => {
                    return {
                        tags: Grabber.regexToTags('<a href="/search/@keywords [^"]+">(?<name>[^<]+)</a>', src),
                        createdAt: Grabber.regexToConst("date", '<b>Posted:</b> <span title="(?<date>[^"]+)" class="popup_date">', src),
                        imageUrl: Grabber.regexToConst("url", '<a href="(?<url>[^"]+)">Download</a>', src),
                    };
                },
            },
            check: {
                url: (): string => {
                    return "/";
                },
                parse: (src: string): boolean => {
                    return src.indexOf("Fur Affinity is &copy;") !== -1;
                },
            },
        },
    },
};

function completeImage(img: IImage): IImage {
    if ((!img.file_url || img.file_url.length < 5) && img.preview_url) {
        img.file_url = img.preview_url
            .replace("/thumbnails/", "/images/")
            .replace("/thumbnail_", "/");
    }

    return img;
}

export const source: ISource = {
    name: "Modified Gelbooru (0.1)",
    modifiers: ["rating:safe", "rating:questionable", "rating:explicit", "user:", "fav:", "fastfav:", "md5:", "source:", "id:", "width:", "height:", "score:", "mpixels:", "filesize:", "date:", "gentags:", "arttags:", "chartags:", "copytags:", "approver:", "parent:", "sub:", "order:id", "order:id_desc", "order:score", "order:score_asc", "order:mpixels", "order:mpixels_asc", "order:filesize", "order:landscape", "order:portrait", "order:favcount", "order:rank", "parent:none", "unlocked:rating"],
    tagFormat: {
        case: "lower",
        wordSeparator: "_",
    },
    searchFormat: {
        and: " ",
    },
    auth: {},
    apis: {
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 20,
            search: {
                url: (query: ISearchQuery): string => {
                    const page: number = query.page - 1;
                    const search = query.search && query.search.length > 0 ? encodeURIComponent(query.search) : "all";
                    return "/index.php?r=posts/index&q=" + search + (page === 0 ? '' : "&page=" + page);
                },
                parse: (src: string): IParsedSearch => {
                    const pageMatch = Grabber.regexMatch('<div class="pagination">\\W*(?:<[ab].*>.*<\\/[ab]>\\W*)*(?:<[ab] href="[^"]+page=(?<page_arg>\\d+)".*>.*<\\/[ab]>|<[ab].*>(?<page_real>\\d+)<\\/[ab]>)\\s*<\\/div>', src);
                    const pageCount = pageMatch && "page_arg" in pageMatch
                        ? parseInt(pageMatch["page_arg"], 10) + 1
                        : pageMatch && "page_real" in pageMatch
                            ? parseInt(pageMatch["page_real"], 10)
                            : undefined;
                    return {
                        images: Grabber.regexToImages('<a id="(?<id>\\d+)" href="[^"]+"><img +src="(?<preview_url>[^"]*thumbnail_(?<md5>[^.]+)\\.[^"]+)" title=" *(?<tags>[^"]*)" .+?</a></div>', src).map(completeImage),
                        pageCount,
                        tags: Grabber.regexToTags('<li><a[^>]*>\\+</a><a [^>]*>-</a> <span [^>]*>\\? <a href="[^"]*">(?<name>[^<]+)</a> (?<count>\\d+)</span></li>', src),
                    };
                },
            },
            details: {
                url: (id: string, md5: string): string => {
                    return "/index.php?r=posts/view&id=" + id;
                },
                parse: (src: string): IParsedDetails => {
                    return {
                        tags: Grabber.regexToTags('<li><a[^>]*>\\+</a><a [^>]*>-</a> <span [^>]*>\\? <a href="[^"]*">(?<name>[^<]+)</a> (?<count>\\d+)</span></li>', src),
                        imageUrl: Grabber.regexToConst("url", '<img[^>]+src="([^"]+)"[^>]+onclick="Note\\.toggle\\(\\);"[^>]*/>', src),
                    };
                },
            },
            check: {
                url: (): string => {
                    return "/";
                },
                parse: (src: string): boolean => {
                    return src.search(/Running modified Gelbooru(?: Beta)? 0\.1/) !== -1
                        || src.search(/Running modified <a[^>]*>Gelbooru<\/a>(?: Beta)? 0\.1/) !== -1;
                },
            },
        },
    },
};

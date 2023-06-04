const orderMap: Record<string, number> = {
    "newest": 4, // 5/6/7
    "popular-8-hours": 10, // 16/17/18
    "popular-24-hours": 11,
    "popular-3-days": 12, // 13
    "popular-1-week": 14,
    "popular-1-month": 15,
    "popular-all-time": 8, // 9
};

function parseSearch(search: string): { query: string, order?: number } {
    let query: string = "";
    let order: number = 0;
    for (const tag of search.split(" ")) {
        if (tag.indexOf("order:") === 0) {
            const val = tag.substr(6);
            const toInt = parseInt(val, 10);
            order = isNaN(toInt) ? orderMap[val] : toInt;
        } else {
            query += (query ? " " : "") + tag;
        }
    }
    return { query, order }
}

function completeImage(img: IImage): IImage {
    if (!img.id && img.page_url) {
        img.id = Grabber.regexToConst("id", "-(?<id>\\d+)$", img.page_url);
    }
    return img;
}

export const source: ISource = {
    name: "DeviantArt",
    forcedTokens: ["file_url"],
    auth: {
        session: {
            type: "post",
            url: "/_sisu/do/signin",
            fields: [
                {
                    id: "pseudo",
                    key: "username",
                },
                {
                    id: "password",
                    key: "password",
                    type: "password",
                },
            ],
            csrf: {
                url: "/users/login",
                fields: ["csrf_token"],
            },
            check: {
                type: "cookie",
                key: "auth",
            },
        },
    },
    apis: {
        rss: {
            name: "RSS",
            auth: [],
            forcedLimit: 60,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions): string | IError => {
                    const parsed = parseSearch(query.search);
                    const offset = (query.page - 1) * opts.limit;
                    const order = parsed.order ? "&order=" + parsed.order : "";
                    return "//backend.deviantart.com/rss.xml?type=deviation&q=" + encodeURIComponent(parsed.query) + order + "&offset=" + offset;
                },
                parse: (src: string): IParsedSearch => {
                    const parsed = Grabber.parseXML(src);
                    const data = Grabber.makeArray(parsed.rss.channel.item);

                    const images: IImage[] = [];
                    for (const image of data) {
                        if (image["media:content"]["@attributes"]["medium"] === "document") {
                            continue;
                        }

                        const thumbnail = Array.isArray(image["media:thumbnail"]) ? image["media:thumbnail"][0] : image["media:thumbnail"];
                        const credit = Array.isArray(image["media:credit"]) ? image["media:credit"][0] : image["media:credit"];
                        const rating = image["media:rating"]["#text"].trim();

                        const img: IImage = {
                            page_url: image["link"]["#text"],
                            created_at: image["pubDate"]["#text"],
                            name: image["media:title"]["#text"],
                            author: credit["#text"],
                            tags: (image["media:keywords"]["#text"] || "").trim().split(", "),
                            preview_url: thumbnail && (thumbnail["#text"] || thumbnail["@attributes"]["url"]),
                            preview_width: thumbnail && thumbnail["@attributes"]["width"],
                            preview_height: thumbnail && thumbnail["@attributes"]["height"],
                            sample_url: image["media:content"]["#text"] || image["media:content"]["@attributes"]["url"],
                            sample_width: image["media:content"]["@attributes"]["width"],
                            sample_height: image["media:content"]["@attributes"]["height"],
                            rating: rating === "nonadult" ? "safe" : (rating === "adult" ? "explicit" : "questionable"),
                        };

                        images.push(completeImage(img));
                    }

                    return { images };
                },
            },
        },
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 24,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions): string | IError => {
                    const parsed = parseSearch(query.search);
                    return "/search/deviations?q=" + encodeURIComponent(parsed.query) + "&page=" + query.page;
                },
                parse: (src: string): IParsedSearch => {
                    return {
                        images: Grabber.regexToImages('<section.*?<a data-hook="deviation_link" href="(?<page_url>[^"]+)"[^>]*>.*?<img[^>]+src="(?<preview_url>[^"]+)"[^>]*>.*?<h2[^<]*>(?<name>[^<]+)</h2>', src).map(completeImage),
                        tags: Grabber.regexToTags('<a href="[^"]*/search/deviations\\?q=[^"]+" data-tag="(?<name>[^"]+)"[^>]*>[^<]+</a>', src),
                        imageCount: Grabber.regexToConst("count", '>(?<count>\\d+) results<', src),
                    };
                },
            },
            details: {
                url: (id: string, md5: string): IError => {
                    return { error: "Not supported (page_url)" };
                },
                parse: (src: string): IParsedDetails | IError => {
                    if (src.indexOf('aria-label="Log in to download"') >= 0) {
                        return { error: "You must login to download this file" };
                    }
                    const imageUrl =
                        Grabber.regexToConst("url", '<a [^>]*href="(?<url>[^"]+)" [^>]*aria-label="Free download"', src)?.replace(/&amp;/g, "&") || // Download link (SWF...)
                        Grabber.regexToConst("url", '<img [^>]*aria-hidden="true"[^>]+src="(?<url>[^"]+)"', src); // Full-size image
                    return {
                        tags: Grabber.regexToTags('<a [^>]*href="[^"]*/tag/(?<name>[^"]+)"', src),
                        imageUrl,
                    };
                },
            },
        },
    },
};

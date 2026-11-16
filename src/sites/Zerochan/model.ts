function formatSearch(query: string): string {
    return encodeURIComponent(query
        .replace(/ /g, ", ")
        .replace(/_/g, " "));
}

function completeImage(img: IImage): IImage {
    if (!img.file_url || img.file_url.length < 5) {
        img.file_url = (img.sample_url || img.preview_url || "")
            .replace(/\/s\d+\.zerochan/, "/static.zerochan")
            .replace(".240.", ".full.")
            .replace(".600.", ".full.")
            .replace("/240/", "/full/")
            .replace("/600/", "/full/");
    }

    if (img.file_size) {
        img.file_size = Grabber.fileSizeToInt(img.file_size);
    }

    return img;
}

function structuredData(html: any): any[] {
    return html.find('script[type="application/ld+json"]').map((script: any) => JSON.parse(script.innerText()));
}

const sortMap: Record<string, string> = {
    "fav-all-time": "fav&t=0",
    "fav-3-months": "fav&t=2",
    "fav-1-week": "fav&t=1",
}
const meta: ISource["meta"] = {
    order: {
        type: "options",
        options: ["id", "fav", "fav-all-time", "fav-3-months", "fav-1-week", "random"],
        default: "id",
    },
};

export const source: ISource = {
    name: "Zerochan",
    modifiers: ["order:id", "order:fav", "order:fav-all-time", "order:fav-3-months", "order:fav-1-week", "order:random"],
    forcedTokens: ["filename", "date"],
    tagFormat: {
        case: "upper",
        wordSeparator: " ",
    },
    searchFormat: {
        and: ", ",
    },
    meta,
    auth: {
        session: {
            type: "post",
            url: "/login",
            fields: [
                {
                    id: "pseudo",
                    key: "name",
                },
                {
                    id: "password",
                    key: "password",
                    type: "password",
                },
                {
                    key: "login",
                    type: "const",
                    value: "Login",
                },
                {
                    key: "ref",
                    type: "const",
                    value: "ref",
                },
            ],
            check: {
                type: "cookie",
                key: "z_hash",
            },
        },
    },
    apis: {
        rss: {
            name: "RSS",
            auth: [],
            forcedLimit: 100,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string | IError => {
                    try {
                        const search = Grabber.parseSearchQuery(query.search, meta);
                        const sortPart = search.order ? (search.order in sortMap ? sortMap[search.order] : search.order) : "id";
                        const pagePart = Grabber.pageUrl(query.page, previous, 100, "p={page}", "o={max}", "o={min}");
                        return "/" + formatSearch(search.query) + "?s=" + sortPart + "&xml&" + pagePart;
                    } catch (e: any) {
                        return { error: e.message };
                    }
                },
                parse: (src: string): IParsedSearch => {
                    const fixedSrc = src.replace(/<div.*?>[\s\S]+?<\/div>/, "") // Fix malformed XML for multi-tag search
                    const parsed = Grabber.parseXML(fixedSrc);
                    const data = Grabber.makeArray(parsed.rss.channel.item);

                    const images: IImage[] = [];
                    for (const image of data) {
                        const img: IImage = {
                            page_url: image["link"]["#text"],
                            name: image["title"]["#text"],
                            tags: image["media:keywords"]["#text"].trim().split(", "),
                            preview_url: image["media:thumbnail"]["#text"] || image["media:thumbnail"]["@attributes"]["url"],
                        };
                        img.id = Grabber.regexToConst("id", "/(?<id>\\d+)", img.page_url);
                        if (image["media:content"]["@attributes"]["expression"] === "sample") {
                            img.sample_url = image["media:content"]["#text"] || image["media:content"]["@attributes"]["url"];
                            img.sample_width = image["media:content"]["@attributes"]["width"];
                            img.sample_height = image["media:content"]["@attributes"]["height"];
                        } else {
                            img.file_url = image["media:content"]["#text"] || image["media:content"]["@attributes"]["url"];
                            img.width = image["media:content"]["@attributes"]["width"];
                            img.height = image["media:content"]["@attributes"]["height"];
                        }
                        images.push(completeImage(img));
                    }

                    const imageCount = parsed.rss.channel.description
                        ? Grabber.countToInt(Grabber.regexToConst("count", "has (?<count>[0-9,]+) .+? anime images", parsed.rss.channel.description["#text"]))
                        : undefined;

                    return {
                        images,
                        imageCount,
                    };
                },
            },
        },
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 48,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string | IError => {
                    try {
                        const search = Grabber.parseSearchQuery(query.search, meta);
                        const sortPart = search.order ? (search.order in sortMap ? sortMap[search.order] : search.order) : "id";
                        const pagePart = Grabber.pageUrl(query.page, previous, 100, "p={page}", "o={max}", "o={min}");
                        return "/" + formatSearch(search.query) + "?s=" + sortPart + "&" + pagePart;
                    } catch (e: any) {
                        return { error: e.message };
                    }
                },
                parse: (src: string): IParsedSearch => {
                    const html = Grabber.parseHTML(src);

                    // Parsing of images in the HTML body
                    const htmlById: Record<string, IImage> = {};
                    for (const link of html.find("#thumbs2 > li a.thumb[href]")) {
                        const thumbnail = link.find("img")[0];
                        const id = link.attr("href").slice(1);
                        const title = thumbnail.attr("title");
                        const info = title.match(/^(\d+)[x×✕](\d+)\s+([\d.,]+\s*(?:kb|mb|b))(?:\s+(\w+))?/i);
                        const uploader = title.match(/(?:^|\n)Uploaded by ([^\r\n]+)/);
                        const image: IImage = {
                            preview_url: thumbnail.attr("data-src") || thumbnail.attr("src"),
                            name: thumbnail.attr("alt"),
                            author: uploader ? uploader[1] : undefined,
                        };
                        if (info) {
                            image.width = parseInt(info[1], 10);
                            image.height = parseInt(info[2], 10);
                            image.file_size = Grabber.fileSizeToInt(info[3].replace(/,/g, ""));
                            image.ext = info[4];
                        }
                        htmlById[id] = image;
                    }

                    // Several HTML images can share one thumbnail
                    const multiples = src.match(/(?:^|\n)\s*multiples\s*=\s*\{([\s\S]*?)\n\s*\}/);
                    for (const object of multiples ? multiples[1].match(/\{[^\r\n]*\}/g) || [] : []) {
                        const alternative = JSON.parse(object);
                        htmlById[alternative.id] = {
                            ...htmlById[alternative.id],
                            width: parseInt(alternative.x, 10),
                            height: parseInt(alternative.y, 10),
                            file_size: Grabber.fileSizeToInt(alternative.size + "kb"),
                            author: alternative.uname,
                        };
                    }

                    // Parsing of the images via the "ld+json" <script> tag
                    const jsonById: Record<string, IImage> = {};
                    const jsonImages: IImage[] = [];
                    for (const entry of structuredData(html)) {
                        for (const item of entry.itemListElement || []) {
                            const match = (item.url || "").match(/\.full\.(\d+)\.([a-z0-9]+)/i);
                            if (!match) {
                                continue;
                            }
                            const id = match[1];
                            if (jsonById[id]) {
                                continue;
                            }
                            const image: IImage = { id, file_url: item.url, ext: match[2] };
                            if (item.name) {
                                image.name = item.name;
                            }
                            if (item.thumbnailUrl) {
                                image.preview_url = item.thumbnailUrl;
                            }
                            jsonById[id] = image;
                            jsonImages.push(image);
                        }
                    }

                    // Merge the JSON and HTML parsed images
                    const images: IImage[] = jsonImages.map(image => {
                        const id = String(image.id);
                        return { ...htmlById[id], ...jsonById[id] };
                    });

                    const countText = html.find("#content > p").map((p: any) => p.innerText()).join("\n");
                    const bodyImageCount = Grabber.regexToConst("count", "(?:^|\\n)(?<count>[0-9,]+) images\\b", countText);
                    const titleImageCount = Grabber.regexToConst("count", "has (?<count>[0-9,]+) .+? anime images", src);
                    let wiki = Grabber.regexToConst("wiki", "<!--.*?shareaholic-canvas.*?-->\\s*<p>(?<wiki>.+?)</p>", src);
                    wiki = wiki ? wiki.replace(/href="\/([^"]+)"/g, 'href="$1"') : wiki;
                    return {
                        tags: Grabber.regexToTags("<li[^>]*>\\s*<a [^>]+>(?<name>[^>]+)</a>\\s+(?:<span>(?<type>[^<]+) (?<count>[0-9]+)</span>|(?<type_2>[^<]*))\\s*</li>", src),
                        images,
                        pageCount: Grabber.countToInt(Grabber.regexToConst("page", "page (?:[0-9,]+) of (?<page>[0-9,]+)", src)),
                        imageCount: Grabber.countToInt(bodyImageCount || titleImageCount || ""),
                        urlNextPage: html.find('a[rel="next"]')[0]?.attr("href"),
                        urlPrevPage: html.find('a[rel="prev"]')[0]?.attr("href"),
                        wiki,
                    };
                },
            },
            details: {
                url: (id: string, md5: string): string => {
                    return "/" + id;
                },
                parse: (src: string): IParsedDetails => {
                    const html = Grabber.parseHTML(src);
                    const image = structuredData(html).filter(entry => entry["@type"] === "ImageObject" && entry.contentUrl)[0];
                    const tags = html.find("#tags > li > a[data-tag]").map((link: any) => ({
                        name: link.attr("data-tag"),
                        type: link.parent().attr("class").split(/\s+/)[0],
                    }));
                    return {
                        tags,
                        imageUrl: image?.contentUrl,
                        createdAt: image?.datePublished,
                    };
                },
            },
            check: {
                url: (): string => {
                    return "/";
                },
                parse: (src: string): boolean => {
                    return src.search(/© [0-9]+-[0-9]+ Zerochan/) !== -1;
                },
            },
        },
    },
};

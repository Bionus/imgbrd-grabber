const THUMBNAILS_DOMAIN = "t4.nhentai.net";
const IMAGES_DOMAIN = "i4.nhentai.net";

const extensionMap: any = {
    j: "jpg",
    p: "png",
    g: "gif",
    w: "webp",
};

function makeGallery(gallery: any): IImage {
    gallery.type = "gallery";
    gallery.tags = gallery["tag_ids"].split(" ").map((id: string) => ({ id: parseInt(id.trim(), 10) }));
    return gallery;
}

function makeImage(image: any): IImage {
    image["file_url"] = image["preview_url"]
        .replace(/https:\/\/t([\d]?)\./, "https://i$1.")
        .replace(/t.jpg$/, ".jpg")
        .replace(/t.png$/, ".png")
        .replace(/t.gif$/, ".gif")
        .replace(/t.webp$/, ".webp");
    return image;
}

const tagTypeMap: Record<string, string> = {
    tag: "general",
    language: "meta",
    category: "general",
    character: "character",
    parody: "copyright",
    artist: "artist",
    group: "artist",
};

function makeTag(tag: any): ITag {
    return {
        id: tag["id"],
        type: tag["type"] in tagTypeMap ? tagTypeMap[tag["type"]] : undefined,
        name: tag["name"],
        count: tag["count"],
    };
}

const meta: Record<string, MetaField> = {
    sort: {
        type: "options",
        default: "date",
        options: ["popular", "popular-week", "popular-today", "date"],
    }
};
export const source: ISource = {
    name: "NHentai",
    modifiers: ["parodies:", "tag:"],
    forcedTokens: ["*"],
    searchFormat: {
        and: " ",
    },
    meta,
    apis: {
        json: {
            name: "JSON",
            auth: [],
            forcedLimit: 25,
            search: {
                url: (query: ISearchQuery): string => {
                    const search = Grabber.parseSearchQuery(query.search, meta);
                    if (search.query.length > 0) {
                        return "/api/galleries/search?page=" + query.page + "&sort=" + search.sort + "&query=" + encodeURIComponent(search.query);
                    }
                    if (search.sort !== "date") {
                        console.warn("Sorting is not supported without search.");
                    }
                    return "/api/galleries/all?page=" + query.page;
                },
                parse: (src: string): IParsedSearch => {
                    const data = JSON.parse(src);
                    const results = Array.isArray(data) ? data : data["result"];

                    const images = results.map((gallery: any) => {
                        const thumb = gallery["images"]["thumbnail"];
                        const img: IImage = {
                            type: "gallery",
                            gallery_count: gallery["num_pages"],
                            id: gallery["id"],
                            name: gallery["title"]["english"],
                            created_at: gallery["upload_date"],
                            tags: gallery["tags"].map(makeTag),
                            preview_url: "https://" + THUMBNAILS_DOMAIN + "/galleries/" + gallery["media_id"] + "/thumb." + extensionMap[thumb["t"]],
                            preview_width: thumb["w"],
                            preview_height: thumb["h"],
                        };
                        return img;
                    });

                    return {
                        images,
                        pageCount: data["num_pages"],
                    };
                },
            },
            gallery: {
                url: (query: IGalleryQuery): string => {
                    return "/api/gallery/" + query.id;
                },
                parse: (src: string): IParsedGallery => {
                    const data = JSON.parse(src);
                    const pages = data["images"]["pages"];

                    const images: IImage[] = [];
                    for (const page in pages) {
                        const image = pages[page];
                        const index = parseInt(page, 10) + 1;
                        images.push({
                            created_at: data["upload_date"],
                            tags: data["tags"].map(makeTag),
                            file_url: "https://" + IMAGES_DOMAIN + "/galleries/" + data["media_id"] + "/" + index + "." + extensionMap[image["t"]],
                            width: image["w"],
                            height: image["h"],
                            preview_url: "https://" + THUMBNAILS_DOMAIN + "/galleries/" + data["media_id"] + "/" + index + "t." + extensionMap[image["t"]],
                        });
                    }

                    return {
                        images,
                        pageCount: 1,
                        imageCount: data["num_pages"],
                    };
                },
            },
        },
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 25,
            search: {
                url: (query: ISearchQuery): string => {
                    const search = Grabber.parseSearchQuery(query.search, meta);
                    if (search.query.length > 0) {
                        return "/search/?page=" + query.page + "&sort=" + search.sort + "&q=" + encodeURIComponent(search.query);
                    }
                    if (search.sort !== "date") {
                        console.warn("Sorting is not supported without search.");
                    }
                    return "/?page=" + query.page;
                },
                parse: (src: string): IParsedSearch => {
                    const matches = Grabber.regexMatches('<div class="gallery" data-tags="(?<tag_ids>[0-9 ]+)"><a href="(?<page_url>/g/(?<id>[0-9]+)/)" class="cover"[^>]*><img[^>]*><noscript><img src="(?<preview_url>[^"]+)" width="(?<preview_width>[0-9]+)" height="(?<preview_height>[0-9]+)"[^>]*></noscript><div class="caption">(?<name>[^<]+)</div>', src);
                    const images = matches.map(makeGallery);
                    return {
                        images,
                        pageCount: Grabber.regexToConst("page", '<a href="[^"]+page=(?<page>[0-9]+)[^"]*" class="last">', src),
                        imageCount: Grabber.countToInt(Grabber.regexToConst("count", "<h2>(?<count>[0-9,]+) Results</h2>", src)),
                    };
                },
            },
            gallery: {
                url: (query: IGalleryQuery): string => {
                    return "/g/" + query.id + "/";
                },
                parse: (src: string): IParsedGallery => {
                    const matches = Grabber.regexMatches('<a class="gallerythumb"[^>]*>\\s*<img[^>]*><noscript><img src="(?<preview_url>[^"]+)" width="(?<preview_width>[0-9]+)" height="(?<preview_height>[0-9]+)"[^>]*>', src);
                    const images = matches.map(makeImage);
                    return {
                        images,
                        pageCount: 1,
                        imageCount: Grabber.countToInt(Grabber.regexToConst("count", '<p class="gpc">Showing [0-9,]+ - [0-9,]+ of (?<count>[0-9,]+) images</p>', src)),
                    };
                },
            },
        },
    },
};

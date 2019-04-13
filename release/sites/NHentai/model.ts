const extensionMap: any = {
    j: "jpg",
    p: "png",
};

function makeGallery(gallery: any): IImage {
    gallery.type = "gallery";
    gallery.tags = gallery["tag_ids"].split(" ").map((id: string) => ({ id: parseInt(id.trim(), 10) }));
    return gallery;
}

function makeImage(image: any): IImage {
    image["file_url"] = image["preview_url"]
        .replace("https://t.", "https://i.")
        .replace(/t.jpg$/, ".jpg")
        .replace(/t.png$/, ".png");
    return image;
}

function makeTag(tag: any): ITag {
    return {
        id: tag["id"],
        name: tag["name"],
        count: tag["count"],
    };
}

export const source: ISource = {
    name: "NHentai",
    modifiers: ["parodies:", "tag:"],
    forcedTokens: ["*"],
    searchFormat: {
        and: " ",
    },
    apis: {
        json: {
            name: "JSON",
            auth: [],
            forcedLimit: 25,
            search: {
                url: (query: any, opts: any, previous: any): string => {
                    if (query.search.length > 0) {
                        return "/api/galleries/search?page=" + query.page + "&sort=date&query=" + encodeURIComponent(query.search);
                    }
                    return "/api/galleries/all?page=" + query.page + "&sort=date";
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
                            date: gallery["upload_date"],
                            tags: gallery["tags"].map(makeTag),
                            preview_url: "https://t.nhentai.net/galleries/" + gallery["media_id"] + "/thumb." + extensionMap[thumb["t"]],
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
                url: (query: any, opts: any): string => {
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
                            date: data["upload_date"],
                            tags: data["tags"].map(makeTag),
                            file_url: "https://i.nhentai.net/galleries/" + data["media_id"] + "/" + index + "." + extensionMap[image["t"]],
                            width: image["w"],
                            height: image["h"],
                            preview_url: "https://t.nhentai.net/galleries/" + data["media_id"] + "/" + index + "t." + extensionMap[image["t"]],
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
                url: (query: any, opts: any, previous: any): string => {
                    if (query.search.length > 0) {
                        return "/search/?page=" + query.page + "&q=" + encodeURIComponent(query.search);
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
                url: (query: any, opts: any): string => {
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

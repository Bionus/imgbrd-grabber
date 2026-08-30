interface IWallHavenSearch {
    query: string;
    purity: string;
    category: string;
    order: string;
    sort: string;
    ratios: string[];
    resolutions: string[];
    atleast?: string;
    colors?: string;
    topRange?: string;
}

function parseDimensions(value: string): string[] {
    return value
        .split(",")
        .map(dimension => dimension.replace(/[:/]/g, "x").toLowerCase())
        .filter(dimension => /^\d+x\d+$/i.test(dimension));
}

function parseSearch(search: string): IWallHavenSearch {
    let query: string = "";
    let purity: string = "111";
    let category: string = "111";
    let order: string = "date_added";
    let sort: string = "desc";
    const ratios: string[] = [];
    let resolutions: string[] = [];
    let atleast: string | undefined;
    let colors: string | undefined;
    let topRange: string | undefined;
    for (const tag of search.split(" ")) {
        const lowerTag = tag.toLowerCase();
        if (lowerTag.indexOf("rating:") === 0) {
            const val = lowerTag.substr(7);
            purity = val === "s" || val === "safe" ? "100" : (val === "e" || val === "explicit" ? "001" : "010");
        } else if (lowerTag.indexOf("-rating:") === 0) {
            const val = lowerTag.substr(8);
            purity = val === "s" || val === "safe" ? "011" : (val === "e" || val === "explicit" ? "110" : "101");
        } else if (lowerTag.indexOf("category:") === 0) {
            const val = lowerTag.substr(9);
            category = val === "anime" ? "010" : (val === "people" ? "001" : "100");
        } else if (lowerTag.indexOf("-category:") === 0) {
            const val = lowerTag.substr(10);
            category = val === "anime" ? "101" : (val === "people" ? "110" : "011");
        } else if (lowerTag.indexOf("order:") === 0) {
            const val = lowerTag.substr(6);
            if (val.substr(-5) === "_desc") {
                order =  val.substr(0, val.length - 5);
                sort = "desc";
            } else if (val.substr(-4) === "_asc") {
                order =  val.substr(0, val.length - 4);
                sort = "asc";
            } else {
                order = val;
            }
        } else if (lowerTag.indexOf("ratio:") === 0) {
            const val = tag.substr(6);
            ratios.push(...parseDimensions(val));
        } else if (lowerTag.indexOf("resolution:") === 0 || lowerTag.indexOf("res:") === 0) {
            const val = tag.substr(lowerTag.indexOf("resolution:") === 0 ? 11 : 4);
            const parsed = parseDimensions(val);
            if (parsed.length > 0) {
                resolutions.push(...parsed);
                atleast = undefined;
            }
        } else if (lowerTag.indexOf("atleast:") === 0) {
            const parsed = parseDimensions(tag.substr(8));
            if (parsed.length > 0) {
                atleast = parsed[0];
                resolutions = [];
            }
        } else if (lowerTag.indexOf("color:") === 0 || lowerTag.indexOf("colors:") === 0) {
            const val = tag.substr(lowerTag.indexOf("colors:") === 0 ? 7 : 6).replace(/^#/, "");
            if (/^[0-9a-f]{6}$/i.test(val)) {
                colors = val.toLowerCase();
            }
        } else if (lowerTag.indexOf("toprange:") === 0) {
            const val = lowerTag.substr(9);
            const ranges: Record<string, string> = { "1d": "1d", "3d": "3d", "1w": "1w", "1m": "1M", "3m": "3M", "6m": "6M", "1y": "1y" };
            if (ranges[val]) {
                topRange = ranges[val];
            }
        } else {
            query += (query ? " " : "") + tag;
        }
    }
    return { query, purity, category, order, sort, ratios, resolutions, atleast, colors, topRange };
}

export const source: ISource = {
    name: "WallHaven",
    modifiers: ["rating:s", "rating:safe", "rating:q", "rating:questionable", "rating:e", "rating:explicit", "order:relevance", "order:random", "order:date_added", "order:views", "order:favorites",  "order:toplist", "order:hot", "category:general", "category:anime", "category:people", "ratio:16x9", "resolution:1920x1080", "atleast:1920x1080", "color:000000", "topRange:1M"],
    forcedTokens: ["tags"],
    auth: {
        url: {
            type: "url",
            fields: [
                {
                    id: "apiKey",
                    key: "apikey",
                },
            ],
        },
    },
    apis: {
        json: {
            name: "JSON",
            auth: [],
            forcedLimit: 24,
            search: {
                url: (query: ISearchQuery): string => {
                    const search = parseSearch(query.search);
                    const params: Record<string, any> = {
                        q: search.query,
                        purity: search.purity,
                        categories: search.category,
                        page: query.page,
                        sorting: search.order,
                        order: search.sort,
                    };
                    if (search.ratios.length > 0) {
                        params["ratios"] = search.ratios.join(",");
                    }
                    if (search.resolutions.length > 0) {
                        params["resolutions"] = search.resolutions.join(",");
                    }
                    if (search.atleast) {
                        params["atleast"] = search.atleast;
                    }
                    if (search.colors) {
                        params["colors"] = search.colors;
                    }
                    if (search.topRange && search.order === "toplist") {
                        params["topRange"] = search.topRange;
                    }
                    return "/api/v1/search?" + Grabber.buildQueryParams(params);
                },
                parse: (src: string): IParsedSearch => {
                    const map = {
                        "id": "id",
                        // "page_url": "url",
                        "source": "source",
                        "width": "dimension_x",
                        "height": "dimension_y",
                        "file_size": "file_size",
                        "created_at": "created_at",
                        "file_url": "path",
                        "sample_url": "path",
                        "preview_url": "thumbs.original",
                    };

                    const data = JSON.parse(src);

                    const images: IImage[] = [];
                    for (const image of data["data"]) {
                        const img: IImage = Grabber.mapFields(image, map);
                        img.page_url = "/api/v1/w/" + img.id;
                        img.rating = image.purity === "sfw" ? "safe" : (image.purity === "nsfw" ? "explicit" : "questionable");
                        img.ext = image.file_type === "image/png" ? "png" : (image.file_type === "image/jpeg" ? "jpg" : undefined);
                        images.push(img);
                    }

                    return {
                        images,
                        pageCount: data["meta"]["last_page"],
                        imageCount: data["meta"]["total"],
                    };
                },
            },
            details: {
                url: (id: string, md5: string): string => {
                    return "/api/v1/w/" + id;
                },
                parse: (src: string): IParsedDetails => {
                    const data = JSON.parse(src)["data"];
                    return {
                        tags: data["tags"].map((tag: any): ITag => ({
                            id: tag["id"],
                            name: tag["name"],
                            type: tag["category"],
                            typeId: tag["category_id"],
                        })),
                        imageUrl: data["path"],
                        createdAt: data["created_at"],
                    };
                },
            },
        },
    },
};

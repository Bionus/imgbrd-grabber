function parseSearch(search: string): { query: string, purity: string, order: string, sort: string } {
    let query: string = "";
    let purity: string = "111";
    let order: string = "date_added";
    let sort: string = "desc";
    for (const tag of search.split(" ")) {
        if (tag.indexOf("rating:") === 0) {
            const val = tag.substr(7);
            purity = val === "s" || val === "safe" ? "100" : (val === "e" || val === "explicit" ? "001" : "010");
        } else if (tag.indexOf("-rating:") === 0) {
            const val = tag.substr(8);
            purity = val === "s" || val === "safe" ? "011" : (val === "e" || val === "explicit" ? "110" : "101");
        }  else if (tag.indexOf("order:") === 0) {
            const val = tag.substr(6);
            if (val.substr(-5) === "_desc") {
                order =  val.substr(0, val.length - 5);
                sort = "desc";
            } else if (val.substr(-4) === "_asc") {
                order =  val.substr(0, val.length - 4);
                sort = "asc";
            } else {
                order = val;
            }
        } else {
            query += (query ? " " : "") + tag;
        }
    }
    return { query, purity, order, sort }
}

export const source: ISource = {
    name: "WallHaven",
    modifiers: ["rating:s", "rating:safe", "rating:q", "rating:questionable", "rating:e", "rating:explicit", "order:relevance", "order:random", "order:date_added", "order:views", "order:favorites",  "order:toplist", "order:hot"],
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
                    return "/api/v1/search?q=" + encodeURIComponent(search.query) + "&purity=" + search.purity + "&page=" + query.page + "&sorting=" + search.order + "&order=" + search.sort;
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

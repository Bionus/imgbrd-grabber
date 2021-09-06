
export const source: ISource = {
    name: "WallHaven",
    modifiers: [],
    forcedTokens: ["tags"],
    auth: {
        url: {
            type: "url",
            fields: [
                {
                    id: "apikey",
                    key: "apiKey",
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
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string | IError => {
                    return "/api/v1/search?q" + encodeURIComponent(query.search) + "&purity=111&page=" + query.page;
                },
                parse: (src: string): IParsedSearch | IError => {
                    const map = {
                        "id": "id",
                        "page_url": "url",
                        "source": "source",
                        "width": "dimension_x",
                        "height": "dimension_y",
                        "file_size": "file_size",
                        "created_at": "created_at",
                        "file_url": "path",
                        "preview_url": "thumbs.original",
                    };

                    const data = JSON.parse(src);

                    const images: IImage[] = [];
                    for (const image of data["data"]) {
                        const img: IImage = Grabber.mapFields(image, map);
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

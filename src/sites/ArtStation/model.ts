function buildImage(raw: any): IImage {
    const map = {
        "id": "id",
        "md5": "hash_id",
        "name": "title",
        "author": "user.username",
        "tags": "tags",
        "createdAt": "created_at",
    };

    const img = Grabber.mapFields(raw, map);
    img.rating = raw.hide_as_adult ? "explicit" : "safe";
    img.preview_url = raw.smaller_square_cover_url || raw.cover?.thumb_url;
    img.authorid = raw.user_id || raw.user?.id;

    if (raw.icons?.multiple_images) {
        img.type = "gallery";
    }

    return img;
}

function buildGalleryImage(raw: any, base: IImage): IImage {
    const map = {
        "id": "id",
        "width": "width",
        "height": "height",
        "sample_url": "image_url",
        "file_url": "image_url",
        "name": "title",
    };

    const asset = Grabber.mapFields(raw, map);
    if (raw.asset_type === "video" || raw.asset_type === "video_clip") {
        delete asset.file_url;
    } else {
        asset.file_url = asset.sample_url.replace("/large/", "/4k/");
    }

    const img = { ...base, ...asset };
    if (!img.preview_url && img.sample_url) {
        img.preview_url = img.sample_url.replace("/large/", "/smaller_square/");
    }

    return img;
}

const meta: Record<string, MetaField> = {
    user: {
        type: "input",
    },
};

export const source: ISource = {
    name: "Slushe",
    modifiers: [],
    forcedTokens: [],
    meta,
    apis: {
        json: {
            name: "JSON",
            auth: [],
            maxLimit: 1000,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions): IError | string => {
                    const search = Grabber.parseSearchQuery(query.search, meta);
                    if (search.query) {
                        /*const url = "/api/v2/search/projects.json";
                        const data = {
                            filters: [{
                                field: "tags",
                                method: "include",
                                value: query.search.split(" "),
                            }],
                            page: query.page,
                            per_page: opts.limit,
                            query: query.search,
                            sorting: "relevance", // likes, date, rank
                        };*/
                        return { error: "Search not supported" };
                    }
                    if (search.user) {
                        return "/users/" + search.user + "/projects.json?page=" + query.page + "&per_page=" + opts.limit;
                    }
                    return "/api/v2/community/explore/projects/latest.json?page=" + query.page + "&per_page=" + opts.limit;
                },
                parse: (src: string): IParsedSearch | IError => {
                    const data = JSON.parse(src);
                    if (!data.data || typeof data.data === "string") {
                        return { error: data.data || "Error" };
                    }

                    return {
                        images: data.data.map(buildImage),
                        imageCount: data.total_count,
                    };
                },
            },
            gallery: {
                url: (query: IGalleryQuery): string => {
                    return "/projects/" + query.md5 + ".json";
                },
                parse: (src: string): IParsedGallery | IError => {
                    const data = JSON.parse(src);
                    if (!data.assets) {
                        return { error: "Not a gallery" };
                    }

                    const images: IImage[] = [];
                    const base = buildImage(data);
                    for (const asset of data.assets) {
                        images.push(buildGalleryImage(asset, base));
                    }

                    return {
                        images,
                        imageCount: images.length,
                    };
                },
            },
            details: {
                fullResults: true,
                url: (id: string, md5: string): string => {
                    return "/projects/" + md5 + ".json";
                },
                parse: (src: string): IImage | IError => {
                    const data = JSON.parse(src);
                    if (!data.assets) {
                        return { error: "Error" };
                    }
                    if (data.assets.length > 1) {
                        return { error: "Not an image (gallery)" };
                    }

                    const base = buildImage(data);
                    return buildGalleryImage(data.assets[0], base);
                },
            },
        },
    },
};

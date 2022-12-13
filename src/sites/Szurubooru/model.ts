const map: Record<string, string> = {
    "id": "id",
    "created_at": "creationTime",
    "md5": "checksumMD5",
    "source": "source",
    "rating": "safety", // FIXME
    "width": "canvasWidth",
    "height": "canvasHeight",
    "file_url": "contentUrl",
    "preview_url": "thumbnailUrl",
    "tags": "tags", // FIXME (names, category, usages)
    "author": "user.name",
    "score": "score",
};
const safetyMap: Record<string, string> = {
    "safe": "safe",
    "sketchy": "questionable",
    "unsafe": "explicit",
};

function buildImage(data: any): IImage {
    const img: IImage = Grabber.mapFields(data, map);
    img.tags = data["tags"].map((tag: any) => ({
        name: tag.names[0],
        type: tag.category,
        count: tag.usages,
    }));
    img.rating = safetyMap[data["safety"]]
    return img;
}

export const source: ISource = {
    name: "Szurubooru",
    modifiers: [],
    auth: {
        basic: {
            type: "http_basic",
            passwordType: "password",
        },
        token: {
            type: "http_basic",
            passwordType: "apiKey",
            tokenType: "Token",
        },
    },
    apis: {
        json: {
            name: "JSON",
            auth: ["basic", "token"],
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions): IRequest => {
                    const offset = (query.page - 1) * opts.limit;
                    return {
                        url: "/api/posts/?offset=" + offset + "&limit=" + opts.limit + "&query=" + query.search,
                        headers: {
                            "Accept": "application/json",
                        },
                    };
                },
                parse: (src: string): IParsedSearch => {
                    const data = JSON.parse(src);
                    return {
                        images: data.results.map(buildImage),
                        imageCount: data.total
                    };
                },
            },
            details: {
                fullResults: true,
                url: (id: string): IRequest => {
                    return {
                        url: "/api/post/" + id,
                        headers: {
                            "Accept": "application/json",
                        },
                    };
                },
                parse: (src: string): IImage => {
                    const data = JSON.parse(src);
                    return buildImage(data);
                },
            },
        },
    },
};

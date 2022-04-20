function buildImage(raw: any): IImage {
    const map = {
        "id": "id",
        "name": "title",
        "page_url": "url",
        "preview_url": "main_thumb",
    };

    const img = Grabber.mapFields(raw, map);
    img.tags = raw["keywords"].split(";");

    if (raw.type === "gallery" || img.page_url.includes("/galleries/")) {
        img.type = "gallery";
    }

    return img;
}

export const source: ISource = {
    name: "Slushe",
    modifiers: [],
    forcedTokens: [],
    apis: {
        json: {
            name: "JSON",
            auth: [],
            maxLimit: 1000,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions): string => {
                    const offset = (query.page - 1) * opts.limit;
                    const type = "videos";
                    if (query.search) {
                        return "/api/?output=json&command=media.search&q=" + encodeURIComponent(query.search) + "&type=" + type + "&offset=" + offset + "&amount=" + opts.limit;
                    }
                    return "/api/?output=json&command=media.newest&type=" + type + "&offset=" + offset + "&amount=" + opts.limit;
                },
                parse: (src: string): IParsedSearch | IError => {
                    const data = JSON.parse(src);
                    if (!data.success || !data.data) {
                        return { error: data.error || "Error" };
                    }

                    const images: IImage[] = data.data.map(buildImage);
                    return {
                        images,
                        imageCount: data.total_results,
                    };
                },
            },
            // This does not return any proper image information
            /*gallery: {
                url: (query: IGalleryQuery): string => {
                    return "/api/?output=json&command=media.data&id=" + query.id;
                },
                parse: (src: string): IParsedGallery | IError => {
                    const data = JSON.parse(src);
                    if (!data.success || !data.data) {
                        return { error: data.error || "Error" };
                    }

                    return buildImage(data.data);
                },
            },*/
            // This does not return any proper image information
            /*details: {
                fullResults: true,
                url: (id: string): string => {
                    return "/api/?output=json&command=media.data&id=" + id;
                },
                parse: (src: string): IImage | IError => {
                    const data = JSON.parse(src);
                    if (!data.success || !data.data) {
                        return { error: data.error || "Error" };
                    }

                    return buildImage(data.data);
                },
            },*/
        },
    },
};

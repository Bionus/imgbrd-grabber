const map = {
    "id": "id",
    "created_at": "created_at",
    "author": "account.username",
};
function parseImage(raw: any, media?: any): IImage {
    const img: IImage = Grabber.mapFields(raw, map);
    img.rating = raw["sensitive"] ? "explicit" : "safe";

    const medias = media ? [media] : raw["media_attachments"];
    if (medias.length >= 1) {
        img.file_url = medias[0].remote_url || medias[0].url;
        img.preview_url = medias[0].preview_remote_url || medias[0].preview_url;
        if (medias[0].meta) {
            img.width = medias[0].meta.original.width;
            img.height = medias[0].meta.original.height;
            img.preview_width = medias[0].meta.small.width;
            img.preview_height = medias[0].meta.small.height;
        }
    }

    if (medias.length > 1) {
        img.type = "gallery";
        img.gallery_count = medias.length;
    }

    return img;
}

const meta: Record<string, MetaField> = {
    user_id: {
        type: "input",
    },
}

export const source: ISource = {
    name: "Mastodon",
    modifiers: ["user:"],
    forcedTokens: [],
    meta,
    auth: {
        oauth2: {
            type: "oauth2",
            authType: "authorization_code",
            authorizationUrl: "/oauth/authorize",
            tokenUrl: "/oauth/token",
            scopes: ["read"],
        },
    },
    apis: {
        json: {
            name: "JSON",
            auth: [],
            maxLimit: 40,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions): string | IError => {
                    const search = Grabber.parseSearchQuery(query.search, meta);

                    // Search
                    if (search.query) {
                        if (!opts.loggedIn) {
                            return { error: "You must be logged in to use search" };
                        }
                        const userId = search.user_id ? "&account_id=" + search.user_id : "";
                        return "/api/v2/search?type=statuses&q=" + encodeURIComponent(search.query) + userId + "&limit=" + opts.limit;
                    }

                    // User ID
                    if (search.user_id) {
                        return "/api/v1/accounts/" + search.user_id + "/statuses?only_media=true&limit=" + opts.limit;
                    }

                    // Public timeline
                    return "/api/v1/timelines/public?only_media=true&limit=" + opts.limit;
                },
                parse: (src: string): IParsedSearch => {
                    const data: any[] = JSON.parse(src);
                    const images = data.map((raw: any) => parseImage(raw));
                    return {
                        images,
                    };
                },
            },
            gallery: {
                url: (query: IGalleryQuery): string => {
                    return "/api/v1/statuses/" + query.id;
                },
                parse: (src: string): IParsedGallery | IError => {
                    let data = JSON.parse(src);
                    if ("statuses" in data) {
                        data = data["statuses"];
                    }

                    const medias = data["media_attachments"];
                    if (medias.length <= 1) {
                        return { error: "Not a gallery" };
                    }

                    const images = medias.map((media: any) => parseImage(data, media));
                    return {
                        images,
                        imageCount: images.length,
                    };
                },
            },
            details: {
                fullResults: true,
                url: (id: string, md5: string): string => {
                    return "/api/v1/statuses/" + id;
                },
                parse: (src: string): IImage => {
                    const data = JSON.parse(src);
                    const img = parseImage(data);

                    // For galleries, we should trust the original information from the gallery endpoint, not the new one which is always the first page
                    if (img.type === "gallery") {
                        delete img.file_url;
                        delete img.sample_url;
                        delete img.preview_url;
                    }

                    return img;
                },
            },
            check: {
                url: (): string => {
                    return "/about";
                },
                parse: (src: string): boolean => {
                    return src.indexOf("joinmastodon.org") !== -1;
                },
            },
        },
    },
};

function urlSampleToThumbnail(url: string): string {
    return url.replace("/img-master/", "/c/150x150/img-master/");
}

function parseSearch(search: string): { mode: string, tags: string[] } {
    const modes = {
        "partial": "partial_match_for_tags",
        "full": "exact_match_for_tags",
        "tc": "title_and_caption",
    };

    let mode = "partial_match_for_tags";
    const tags = [];

    const parts = search.split(" ");
    for (const tag of parts) {
        const part = tag.trim();
        if (part.indexOf("mode:") === 0) {
            const tmode = part.substr(5);
            if (tmode in modes) {
                mode = (modes as any)[tmode];
                continue;
            }
        }
        tags.push(part);
    }

    return { mode, tags };
}

export const source: ISource = {
    name: "Pixiv",
    modifiers: ["mode:partial", "mode:full", "mode:tc"],
    forcedTokens: [],
    searchFormat: {
        and: " ",
    },
    auth: {
        oauth2: {
            type: "oauth2",
            authType: "password",
            tokenUrl: "https://oauth.secure.pixiv.net/auth/token",
        },
    },
    apis: {
        json: {
            name: "JSON",
            auth: [],
            maxLimit: 1000, // Actual max limit is higher but unnecessary, slow, and unreliable
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions): string | IError => {
                    if (!opts.loggedIn) {
                        return { error: "You need to be logged in to use the Pixiv source." };
                    }
                    const search = parseSearch(query.search);
                    const illustParams: string[] = [
                        "word=" + encodeURIComponent(search.tags.join(" ")),
                        "offset=" + ((query.page - 1) * 30),
                        "search_target=" + search.mode,
                        "sort=date_desc", // date_desc, date_asc
                        "filter=for_ios",
                        "image_sizes=small,medium,large",
                    ];
                    return "https://app-api.pixiv.net/v1/search/illust?" + illustParams.join("&");
                },
                parse: (src: string): IParsedSearch => {
                    const map = {
                        "name": "title",
                        "file_url": "image_urls.large",
                        "sample_url": "image_urls.medium",
                        "preview_url": "image_urls.small",
                        "width": "width",
                        "parent_id": "parent_id",
                        "height": "height",
                        "creator_id": "user.id",
                        "id": "id",
                        "tags": "tags",
                        "author": "user.name",
                    };

                    const data = JSON.parse(src);

                    const images: IImage[] = [];
                    for (const image of (data["response"] || data["illusts"])) {
                        const img = Grabber.mapFields(image, map);
                        if (image["age_limit"] === "all-age") {
                            img.rating = "safe";
                        } else if (image["age_limit"] === "r18") {
                            img.rating = "explicit";
                        }
                        if (image["is_manga"]) {
                            img.type = "gallery";
                            img.gallery_count = image["page_count"];
                        }
                        if (image["meta_pages"] && image["meta_pages"].length > 1) {
                            img.type = "gallery";
                            img.gallery_count = image["meta_pages"].length;
                        }
                        img.created_at = image["created_time"] || image["create_date"];
                        if (image["caption"]) {
                            img.description = image["caption"];
                        }
                        if (!img.preview_url) {
                            img.preview_url = image["image_urls"]["medium"];
                        }
                        images.push(img);
                    }

                    if (data["response"]) {
                        return {
                            images,
                            imageCount: data["pagination"]["total"],
                            pageCount: data["pagination"]["pages"],
                        };
                    } else {
                        return {
                            images,
                            urlNextPage: data["next_url"],
                        };
                    }
                },
            },
            gallery: {
                url: (query: IGalleryQuery): string => {
                    return "https://public-api.secure.pixiv.net/v1/works/" + query.id + ".json?image_sizes=large";
                },
                parse: (src: string): IParsedGallery => {
                    const data = JSON.parse(src)["response"][0];
                    return {
                        images: data["metadata"]["pages"].map((page: any): IImage => {
                            return {
                                file_url: page["image_urls"]["large"],
                                sample_url: page["image_urls"]["medium"],
                                preview_url: urlSampleToThumbnail(page["image_urls"]["medium"]),
                            };
                        }),
                        tags: data["tags"],
                        imageCount: data["page_count"],
                    };
                },
            },
            details: {
                url: (id: number, md5: string): string => {
                    return "https://public-api.secure.pixiv.net/v1/works/" + id + ".json?image_sizes=large";
                },
                parse: (src: string): IParsedDetails => {
                    const data = JSON.parse(src)["response"][0];
                    return {
                        imageUrl: data["image_urls"]["large"],
                        tags: data["tags"],
                        createdAt: data["created_time"],
                    };
                },
            },
        },
    },
};

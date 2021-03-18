// Reference:
// - https://github.com/upbit/pixivpy/blob/master/pixivpy3/aapi.py
// - https://github.com/akameco/pixiv-app-api/blob/master/src/index.ts

function urlSampleToThumbnail(url: string): string {
    return url.replace("/img-master/", "/c/150x150/img-master/");
}

function parseSearch(search: string): { mode: string, tags: string[], bookmarks?: number, user?: number, startDate?: string, endDate?: string } {
    const modes = {
        "partial": "partial_match_for_tags",
        "full": "exact_match_for_tags",
        "tc": "title_and_caption",
    };

    let mode = "partial_match_for_tags";
    let bookmarks = undefined;
    let user = undefined;
    const tags = [];
    let startDate = undefined;
    let endDate = undefined;

    const parts = search.split(" ");
    for (const tag of parts) {
        const part = tag.trim();
        if (part.length === 0) {
            continue;
        }
        if (part.indexOf("mode:") === 0) {
            const tmode = part.substr(5);
            if (tmode in modes) {
                mode = (modes as any)[tmode];
                continue;
            }
        }
        if (part.indexOf("bookmarks:") === 0) {
            bookmarks = parseInt(part.substr(10), 10);
        }
        if (part.indexOf("user:") === 0) {
            user = parseInt(part.substr(5), 10);
        }

        if (part.indexOf("date:") === 0) {
            const datePart = part.substr(5);
            if (datePart.indexOf("..") !== -1) {
                const split = datePart.split("..");
                startDate = split[0];
                endDate = split[1];
            } else if (datePart.substr(0, 2) === ">=") {
                startDate = datePart.substr(2);
            } else if (datePart.substr(0, 2) === "<=") {
                endDate = datePart.substr(2);
            } else {
                startDate = datePart;
                endDate = datePart;
            }
            continue;
        }

        tags.push(part);
    }

    return { mode, tags, bookmarks, user, startDate, endDate };
}

function parseImage(image: any, fromGallery: boolean): IImage {
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

    const img = Grabber.mapFields(image, map);
    if (image["age_limit"] === "all-age") {
        img.rating = "safe";
    } else if (image["age_limit"] === "r18") {
        img.rating = "explicit";
    }
    img.created_at = image["created_time"] || image["create_date"];
    if (image["caption"]) {
        img.description = image["caption"];
    }
    if (!fromGallery) {
        if (image["is_manga"]) {
            img.type = "gallery";
            img.gallery_count = image["page_count"];
        }
        if (image["meta_pages"] && image["meta_pages"].length > 1) {
            img.type = "gallery";
            img.gallery_count = image["meta_pages"].length;
        }
        if (image["meta_single_page"] && image["meta_single_page"]["original_image_url"]) {
            img.file_url = image["meta_single_page"]["original_image_url"];
        }
    }
    if (!img.preview_url) {
        img.preview_url = urlSampleToThumbnail(image["image_urls"]["medium"]);
    }

    return img;
}

export const source: ISource = {
    name: "Pixiv",
    modifiers: ["mode:partial", "mode:full", "mode:tc", "bookmarks:", "user:", "date:"],
    forcedTokens: [],
    searchFormat: {
        and: " ",
    },
    auth: {
        oauth2: {
            type: "oauth2",
            authType: "refresh_token",
            tokenUrl: "https://oauth.secure.pixiv.net/auth/token",
        },
    },
    apis: {
        json: {
            name: "JSON",
            auth: [],
            forcedLimit: 30,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions): string | IError => {
                    if (!opts.loggedIn) {
                        return { error: "You need to be logged in to use the Pixiv source." };
                    }

                    const search = parseSearch(query.search);
                    const illustParams: string[] = [
                        "offset=" + ((query.page - 1) * 30),
                        "filter=for_ios",
                        "image_sizes=small,medium,large",
                    ];

                    // Date option
                    if (search.startDate) {
                        illustParams.push("start_date=" + search.startDate);
                    }
                    if (search.endDate) {
                        illustParams.push("end_date=" + search.endDate);
                    }

                    // User's bookmarks
                    if (search.bookmarks !== undefined && search.bookmarks > 0) {
                        if (query.page > 1) {
                            return { error: "Cannot jump to arbitrary bookmark pages, have to browse from the first page." };
                        }
                        illustParams.push("user_id=" + search.bookmarks);
                        illustParams.push("restrict=public");
                        return "https://app-api.pixiv.net/v1/user/bookmarks/illust?" + illustParams.join("&");
                    }

                    // User's illusts
                    if (search.user !== undefined && search.user > 0) {
                        illustParams.push("user_id=" + search.user);
                        illustParams.push("type=illust");
                        return "https://app-api.pixiv.net/v1/user/illusts?" + illustParams.join("&");
                    }

                    // Newest (when no tag is provided)
                    if (search.tags.length === 0) {
                        illustParams.push("content_type=illust");
                        return "https://app-api.pixiv.net/v1/illust/new?" + illustParams.join("&");
                    }

                    // Search by tag
                    illustParams.push("word=" + encodeURIComponent(search.tags.join(" ")));
                    illustParams.push("search_target=" + search.mode);
                    illustParams.push("sort=date_desc"); // date_desc, date_asc
                    return "https://app-api.pixiv.net/v1/search/illust?" + illustParams.join("&");
                },
                parse: (src: string): IParsedSearch => {
                    const data = JSON.parse(src);

                    const images: IImage[] = [];
                    for (const image of (data["response"] || data["illusts"])) {
                        images.push(parseImage(image, false));
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
                        images: data["metadata"]["pages"].map((page: any) => parseImage({ ...data, ...page }, true)),
                        tags: data["tags"],
                        imageCount: data["page_count"],
                    };
                },
            },
            details: {
                url: (id: string, md5: string): string => {
                    if (id === "" || id === "0") { return ""; } // Gallery images don't have an ID
                    return "https://public-api.secure.pixiv.net/v1/works/" + id + ".json?image_sizes=large";
                },
                parse: (src: string): IParsedDetails => {
                    const data = JSON.parse(src)["response"][0];
                    return {
                        imageUrl: data["is_manga"] ? undefined : data["image_urls"]["large"],
                        tags: data["tags"],
                        createdAt: data["created_time"],
                    };
                },
            },
        },
    },
};

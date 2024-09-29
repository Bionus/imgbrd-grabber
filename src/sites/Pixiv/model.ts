// Reference:
// - https://github.com/upbit/pixivpy/blob/master/pixivpy3/aapi.py
// - https://github.com/akameco/pixiv-app-api/blob/master/src/index.ts

// Set to true to use translated tag names when available
const TRANSLATED_TAG_NAMES = false;

function urlSampleToThumbnail(url: string): string {
    return url.replace("/img-master/", "/c/150x150/img-master/");
}

const meta: ISource["meta"] = {
    mode: {
        type: "options",
        options: ["partial", "full",  "tc"],
        default: "partial",
    },
    bookmarks: {
        type: "input",
        parser: parseInt,
    },
    user: {
        type: "input",
        parser: parseInt,
    },
    type: {
        type: "options",
        options: ["illust", "manga"],
    },
};

function parseSearch(search: string): { mode: string, tags: string[], bookmarks?: number, user?: number, followed: boolean, startDate?: string, endDate?: string, type?: string } {
    const modes: any = {
        "partial": "partial_match_for_tags",
        "full": "exact_match_for_tags",
        "tc": "title_and_caption",
    };

    const parsed = Grabber.parseSearchQuery(search, meta);
    const mode = modes[parsed.mode];
    const bookmarks = parsed.bookmarks;
    const user = parsed.user;
    const type = parsed.type;

    const tags = [];
    let followed = false;
    let startDate = undefined;
    let endDate = undefined;

    const parts = parsed.query.split(" ");
    for (const tag of parts) {
        const part = tag.trim();
        if (part.length === 0) {
            continue;
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
        if (part === "is:followed") {
            followed = true;
            continue;
        }

        tags.push(part);
    }

    return { mode, tags, bookmarks, user, followed, startDate, endDate, type };
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
    img.identity = {"id": img["id"]};
    if (image["age_limit"] === "all-age") {
        img.rating = "safe";
    } else if (image["age_limit"] === "r18") {
        img.rating = "explicit";
    }
    if (!("age_limit" in image)) {
        img.rating = image["x_restrict"] === 1 ? "explicit" : "safe";
    }
    img.created_at = image["created_time"] || image["create_date"];
    if (image["caption"]) {
        img.description = image["caption"];
    }
    if (image["image_urls"] && image["image_urls"]["original"]) {
        img.file_url = image["image_urls"]["original"];
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

    // Ugoira replace first frame by ZIP path
    if (/ugoira0/.test(img.file_url)){
        img.sample_url = img.file_url;
        img.file_url = img.file_url
            .replace("img-original", "img-zip-ugoira")
            .replace(/ugoira0\.(jpe?g|png|webp|gif)/, "ugoira1920x1080.zip");
    }

    if (TRANSLATED_TAG_NAMES) {
        img.tags = img.tags.map((tag: any) => tag["translated_name"] || tag["name"]);
    }

    return img;
}

export const source: ISource = {
    name: "Pixiv",
    modifiers: ["mode:partial", "mode:full", "mode:tc", "bookmarks:", "user:", "date:", "is:followed"],
    forcedTokens: [],
    searchFormat: {
        and: " ",
    },
    meta,
    auth: {
        oauth2: {
            type: "oauth2",
            authType: "refresh_token",
            tokenUrl: "https://oauth.secure.pixiv.net/auth/token",
        },
        oauth2_pkce: {
            type: "oauth2",
            authType: "pkce",
            authorizationUrl: "https://app-api.pixiv.net/web/v1/login",
            tokenUrl: "https://oauth.secure.pixiv.net/auth/token",
            redirectUrl: "https://app-api.pixiv.net/web/v1/users/auth/pixiv/callback",
            urlProtocol: "pixiv",
        },
    },
    apis: {
        json: {
            name: "JSON",
            auth: ["oauth2", "oauth2_pkce"],
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
                        if (search.type) {
                            console.warn("Type filtering does not apply to user bookmarks");
                        }
                        return "https://app-api.pixiv.net/v1/user/bookmarks/illust?" + illustParams.join("&");
                    }

                    // User's illusts
                    if (search.user !== undefined && search.user > 0) {
                        illustParams.push("user_id=" + search.user);
                        if (search.type) {
                            illustParams.push("type=" + search.type);
                        }
                        return "https://app-api.pixiv.net/v1/user/illusts?" + illustParams.join("&");
                    }

                    // User's follows
                    if (search.followed) {
                        illustParams.push("restrict=public");
                        return "https://app-api.pixiv.net/v2/illust/follow?" + illustParams.join("&");
                    }

                    // Newest (when no tag is provided)
                    if (search.tags.length === 0) {
                        if (search.type) {
                            illustParams.push("content_type=" + search.type);
                        }
                        return "https://app-api.pixiv.net/v1/illust/new?" + illustParams.join("&");
                    }

                    // Search by tag
                    illustParams.push("word=" + encodeURIComponent(search.tags.join(" ")));
                    illustParams.push("search_target=" + search.mode);
                    illustParams.push("sort=date_desc"); // date_desc, date_asc
                    if (search.type) {
                        console.warn("Type filtering does not apply to search");
                    }
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
                    return "https://app-api.pixiv.net/v1/illust/detail?illust_id=" + query.id + "&image_sizes=large";
                },
                parse: (src: string): IParsedGallery => {
                    const data = JSON.parse(src)["illust"];
                    return {
                        images: data["meta_pages"].map((page: any) => parseImage({ ...data, ...page }, true)),
                        tags: data["tags"],
                        imageCount: data["page_count"],
                    };
                },
            },
            details: {
                fullResults: true,
                url: (id: string, md5: string): string => {
                    if (id === "" || id === "0") { return ""; } // Gallery images don't have an ID
                    return "https://app-api.pixiv.net/v1/illust/detail?illust_id=" + id + "&image_sizes=large";
                },
                parse: (src: string): IImage => {
                    const data = JSON.parse(src)["illust"];
                    const img = parseImage(data, false);

                    // For galleries, we should trust the original information from the gallery endpoint, not the new one which is always the first page
                    if (img.type === "gallery") {
                        delete img.file_url;
                        delete img.sample_url;
                        delete img.preview_url;
                    }

                    return img;
                },
            },
            endpoints: {
                ugoira_details: {
                    input: {
                        id: {
                            type: "input",
                        },
                    },
                    url: (query: Record<"id", number>): string => {
                        return "https://app-api.pixiv.net/v1/ugoira/metadata?illust_id=" + String(query.id);
                    },
                    parse: (src: string): IParsedUgoiraDetails => {
                        return JSON.parse(src)["ugoira_metadata"];
                    },
                }
            }
        },
    },
};

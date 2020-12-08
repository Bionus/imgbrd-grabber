interface IRedditSearch {
    tags: string[];
    subreddit?: string;
    user?: string;
    sort?: string;
    since?: string;
}

function parseSearch(search: string): IRedditSearch {
    const ret: IRedditSearch = { tags: [] };
    const parts = search.split(" ").map((p) => p.trim()).filter((p) => p.length > 0);
    for (const part of parts) {
        if (part.indexOf("subreddit:") === 0) {
            ret.subreddit = part.substr(10);
        } else if (part.indexOf("user:") === 0) {
            ret.user = part.substr(5);
        } else if (part.indexOf("sort:") === 0) {
            ret.sort = part.substr(5);
        } else if (part.indexOf("since:") === 0) {
            ret.since = part.substr(6);
        } else {
            ret.tags.push(part);
        }
    }
    return ret;
}

function makeArgs(args: { [key: string]: any }): string {
    let ret = "";
    for (const key in args) {
        if (args[key] !== undefined && args[key] !== null) {
            ret += (ret.length === 0 ? "?" : "&") + key + "=" + encodeURIComponent(args[key]);
        }
    }
    return ret;
}

export const source: ISource = {
    name: "Reddit",
    modifiers: ["subreddit:", "user:", "sort:hot", "sort:new", "sort:top", "sort:rising", "sort:relevance", "sort:comments", "since:hour", "since:day", "since:week", "since:month", "since:year", "since:all}"],
    apis: {
        json: {
            name: "JSON",
            auth: [],
            maxLimit: 100,
            search: {
                parseErrors: true,
                url: (query: ISearchQuery, opts: IUrlOptions): string => {
                    const search = parseSearch(query.search);
                    const prefix = search.subreddit ? `/r/${search.subreddit}` : "";
                    if (search.tags.length > 0) {
                        const args = {
                            q: search.tags.join(" "),
                            sort: search.sort,
                            t: search.since || "all",
                            restrict_sr: search.subreddit ? 1 : undefined,
                            limit: opts.limit,
                            raw_json: 1,
                         };
                        return prefix + "/search.json" + makeArgs(args);
                    } else {
                        const args = {
                            t: search.since || "all",
                            limit: opts.limit,
                            raw_json: 1,
                        };
                        return prefix + (search.sort ? `/${search.sort}` : "") + ".json" + makeArgs(args);
                    }
                },
                parse: (src: string): IParsedSearch | IError => {
                    const map = {
                        "author": "author",
                        "name": "title",
                        "source": "url",
                        "file_url": "url",
                        "created_at": "created_utc",
                        "preview_url": "thumbnail",
                        "preview_width": "thumbnail_width",
                        "preview_height": "thumbnail_height",
                        "page_url": "permalink",
                        "score": "score",
                    };

                    const data = JSON.parse(src);
                    if (data.kind !== "Listing") {
                        return { error: "No listing found in response" };
                    }

                    const images: IImage[] = [];
                    for (const child of data.data.children) {
                        // Ignore non-link posts
                        if (child.kind !== "t3") { // 1=comment, 2=account, 3=link, 4=message, 5=subreddit, 6=award
                            continue;
                        }

                        const raw = child.data;

                        // Ignore text-only posts
                        if (raw.thumbnail === "self") {
                            continue;
                        }

                        const img: IImage = Grabber.mapFields(raw, map);

                        // Galleries
                        if (raw.is_gallery === true) {
                            img.type = "gallery";
                            if (raw.gallery_data?.items) {
                                img.gallery_count = raw.gallery_data.items.length;
                            }
                        }

                        // Try to get the biggest preview and use it as a sample
                        if (raw.preview?.images?.[0]?.source) {
                            img.sample_url = raw.preview.images[0].source.url.replace("&amp;", "&");
                            img.sample_width = raw.preview.images[0].source.width;
                            img.sample_height = raw.preview.images[0].source.height;
                        }

                        // Videos
                        if (raw.secure_media?.oembed) {
                            img.width = raw.secure_media.oembed.width;
                            img.height = raw.secure_media.oembed.height;
                        }
                        if (raw.is_video && raw.secure_media?.reddit_video) {
                            img.file_url = raw.secure_media.reddit_video.fallback_url; // FIXME: should use the HD url instead, but Grabber doesn't support playlist-based files
                            img.width = raw.secure_media.reddit_video.width;
                            img.height = raw.secure_media.reddit_video.height;
                        }

                        // Rating
                        img.rating = raw.over_18 ? "explicit" : "safe";

                        images.push(img);
                    }

                    return { images };
                },
            },
        },
    },
};

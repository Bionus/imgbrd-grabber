function getExtension(url: string): string {
    const index = url.lastIndexOf(".");
    if (index >= 0 && index > url.length - 10) {
        return url.substr(index + 1);
    }
    return "";
}

interface ISearch {
    user: string;
    retweets: boolean;
    replies: boolean;
}

function parseSearch(search: string[]): ISearch {
    let user = "";
    let retweets = true;
    let replies = true;
    for (let tag of search) {
        tag = tag.trim();
        if (tag.substr(0, 9) === "retweets:") {
            retweets = tag.substr(9) === "yes";
        } else if (tag.substr(0, 8) === "replies:") {
            replies = tag.substr(8) === "yes";
        } else {
            user = tag;
        }
    }
    return {
        user,
        retweets,
        replies,
    };
}

function parseTweetMedia(sc: any, original: any, media: any): any {
    const d: IImage = {} as any;
    const sizes = media["sizes"];

    // Meta-data
    d.id = media["id_str"];
    d.author = sc["user"]["screen_name"];
    d.author_id = sc["user"]["id_str"];
    d.created_at = sc["created_at"];
    d.tags = sc["entities"]["hashtags"].map((hashtag: any) => hashtag["text"]);

    // Images
    d.width = sizes["large"]["w"];
    d.height = sizes["large"]["h"];
    if ("thumb" in sizes) {
        d.preview_url = media["media_url_https"] + ":thumb";
        d.preview_width = sizes["thumb"]["w"];
        d.preview_height = sizes["thumb"]["h"];
    }
    if ("medium" in sizes) {
        d.sample_url = media["media_url_https"] + ":medium";
        d.sample_width = sizes["medium"]["w"];
        d.sample_height = sizes["medium"]["h"];
    }

    // Full-size link
    if ("video_info" in media) {
        let maxBitrate = -1;
        for (const variantInfo of media["video_info"]["variants"]) {
            const bitrate = variantInfo["bitrate"];
            if (bitrate > maxBitrate) {
                maxBitrate = bitrate;
                d.file_url = variantInfo["url"];
                d.ext = getExtension(variantInfo["url"]);
            }
        }
    } else {
        d.file_url = media["media_url_https"] + ":large";
        d.ext = getExtension(media["media_url_https"]);
    }

    // Additional tokens
    d.tokens = {};
    d.tokens["tweet_id"] = sc["id_str"];
    d.tokens["original_tweet_id"] = original["id_str"];
    d.tokens["original_author"] = original["user"]["screen_name"];
    d.tokens["original_author_id"] = original["user"]["id_str"];
    d.tokens["original_date"] = "date:" + original["created_at"];

    return d;
}

function parseTweet(sc: any, gallery: boolean): IImage[] | IImage | boolean {
    const original = sc;
    if ("retweeted_status" in sc) {
        sc = sc["retweeted_status"];
    }
    if (!("extended_entities" in sc)) {
        return false;
    }

    const entities = sc["extended_entities"];
    if (!("media" in entities)) {
        return false;
    }

    const medias = entities["media"];
    if (!medias || medias.length === 0) {
        return false;
    }

    if (medias.length > 1) {
        if (gallery) {
            return medias.map((media: any) => parseTweetMedia(sc, original, media));
        }

        const d = parseTweetMedia(sc, original, medias[0]);
        d.type = "gallery";
        d.gallery_count = medias.length;
        d.id = original["id_str"];
        return d;
    }

    return parseTweetMedia(sc, original, medias[0]);
}

export const source: ISource = {
    name: "Twitter",
    modifiers: ["retweets:yes", "retweets:no", "replies:yes", "replies:no"],
    tokens: ["tweet_id", "original_tweet_id", "original_author", "original_author_id", "original_date"],
    auth: {
        oauth2: {
            type: "oauth2",
            authType: "header_basic",
            tokenUrl: "/oauth2/token",
        },
    },
    apis: {
        json: {
            name: "JSON",
            auth: ["oauth2"],
            maxLimit: 200,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string | IError => {
                    try {
                        const search = parseSearch(query.search.split(" "));
                        const pageUrl = Grabber.pageUrl(query.page, previous, 1, "", "&since_id={max}", "&max_id={min-1}");
                        const params = [
                            "include_rts=" + (search.retweets ? "true" : "false"),
                            "exclude_replies=" + (!search.replies ? "true" : "false"),
                            "tweet_mode=extended",
                            "screen_name=" + encodeURIComponent(search.user),
                        ];
                        return "/1.1/statuses/user_timeline.json?" + params.join("&") + pageUrl;
                    } catch (e) {
                        return { error: e.message };
                    }
                },
                parse: (src: string): IParsedSearch => {
                    const data = JSON.parse(src);

                    const images: IImage[] = [];
                    for (const i in data) {
                        const img = parseTweet(data[i], false);
                        if (img !== false) {
                            images.push(img as any);
                        }
                    }

                    return { images };
                },
            },
            gallery: {
                url: (query: IGalleryQuery, opts: IUrlOptions): string => {
                    return "/1.1/statuses/show.json?id=" + query.id;
                },
                parse: (src: string): IParsedGallery => {
                    const data = JSON.parse(src);
                    const images = Grabber.makeArray(parseTweet(data, true));
                    return {
                        images,
                        imageCount: images.length,
                        pageCount: 1,
                    };
                },
            },
        },
    },
};

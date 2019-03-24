function getExtension(url: string): string {
    const index = url.lastIndexOf(".");
    if (index >= 0 && index > url.length - 10) {
        return url.substr(index + 1);
    }
    return "";
}

function parseTweetMedia(sc: any, media: any): any {
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

    return d;
}

function parseTweet(sc: any, gallery: boolean): IImage[] | IImage | boolean {
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
            return medias.map((media: any) => parseTweetMedia(sc, media));
        }

        const d = parseTweetMedia(sc, medias[0]);
        d.type = "gallery";
        d.gallery_count = medias.length;
        return d;
    }

    return parseTweetMedia(sc, medias[0]);
}

export const source: ISource = {
    name: "Twitter",
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
                url: (query: any, opts: any, previous: any): string | IError => {
                    try {
                        const pageUrl = Grabber.pageUrl(query.page, previous, 1, "", "&since_id={max}", "&max_id={min-1}");
                        return "/1.1/statuses/user_timeline.json?include_rts=true&exclude_replies=false&tweet_mode=extended&screen_name=" + encodeURIComponent(query.search) + pageUrl;
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
                url: (query: any, opts: any): string => {
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

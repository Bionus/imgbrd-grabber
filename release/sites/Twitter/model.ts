export const source: ISource = {
    name: "Twitter",
    auth: {
        oauth2: {
            type: "oauth2",
            token_url: "/oauth2/token",
            fields: [
                {
                    key: "consumerKey",
                    type: "username",
                },
                {
                    key: "consumerSecret",
                    type: "password",
                },
            ],
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
                        const pageUrl = Grabber.pageUrl(query.page, previous, 1, "", "since_id={max}", "max_id={min-1}");
                        return "/1.1/statuses/user_timeline.json?include_rts=true&screen_name=" + query.search + pageUrl;
                    } catch (e) {
                        return { error: e.message };
                    }
                },
                parse: (src: string): IParsedSearch => {
                    const data = JSON.parse(src);

                    const images: IImage[] = [];
                    for (const i in data) {
                        const sc = data[i];
                        const d: any = {};

                        if (!("extended_entities" in sc)) {
                            continue;
                        }

                        const entities = sc["extended_entities"];
                        if (!("media" in entities)) {
                            continue;
                        }

                        const medias = entities["media"];
                        if (!medias || medias.length === 0) {
                            continue;
                        }

                        const media = medias[0];
                        const sizes = media["sizes"];

                        d["id"] = sc["id_str"];
                        d["created_at"] = sc["created_at"];
                        d["preview_url"] = media["media_url_https"] + ":thumb";
                        if ("medium" in sizes) {
                            d["sample_url"] = media["media_url_https"] + ":medium";
                        }

                        const size = sizes["large"];
                        d["width"] = size["w"];
                        d["height"] = size["h"];

                        if (media.contains("video_info")) {
                            let maxBitrate = -1;
                            for (const variantInfo of media["video_info"]["variants"]) {
                                const bitrate = variantInfo["bitrate"];
                                if (bitrate > maxBitrate) {
                                    maxBitrate = bitrate;
                                    d["file_url"] = variantInfo["url"];
                                }
                            }
                        } else {
                            d["file_url"] = media["media_url_https"] + ":large";
                        }

                        images.push(d);
                    }

                    return { images };
                },
            },
        },
    },
};

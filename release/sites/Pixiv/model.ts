function urlSampleToFull(url: string): string {
    return url
        .replace("img-master", "img-original")
        .replace(/_master\d+\./, ".");
}

export const source: ISource = {
    name: "Pixiv",
    modifiers: [],
    forcedTokens: [],
    searchFormat: {
        and: " ",
    },
    auth: {},
    apis: {
        json: {
            name: "JSON",
            auth: [],
            maxLimit: 1000, // Actual max limit is higher but unnecessary, slow, and unreliable
            search: {
                url: (query: any, opts: any, previous: any): string => {
                    const params: string[] = [
                        "q=" + query.search,
                        "page=" + query.page,
                        "per_page=" + opts.limit,
                        "period=all",
                        "order=desc",
                        "mode=caption",
                        "sort=date",
                        "image_sizes=small,medium,large",
                    ];
                    return "https://public-api.secure.pixiv.net/v1/search/works.json?" + params.join("&");
                },
                parse: (src: string): IParsedSearch => {
                    const map = {
                        "name": "title",
                        "created_at": "created_time",
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
                    for (const image of data["response"]) {
                        const img = Grabber.mapFields(image, map);
                        if (image["age_limit"] === "all-age") {
                            img["rating"] = "safe";
                        } else if (image["age_limit"] === "r18") {
                            img["rating"] = "explicit";
                        }
                        if (image["is_manga"]) {
                            img["type"] = "gallery";
                            img["gallery_count"] = image["page_count"];
                        }
                        images.push(img);
                    }

                    return {
                        images,
                        imageCount: data["pagination"]["total"],
                        pageCount: data["pagination"]["pages"],
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
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 40,
            search: {
                url: (query: any, opts: any, previous: any): string => {
                    return "/search.php?s_mode=s_tag&order=date_d&p=" + query.page + "&word=" + encodeURIComponent(query.search);
                },
                parse: (src: string): IParsedSearch => {
                    // Page data is stored in the data attributes of a hidden React container
                    const rawData = src.match(/<input type="hidden"\s*id="js-mount-point-search-result-list"\s*data-items="([^"]+)"\s*data-related-tags="([^"]+)"/i);
                    const rawItems = JSON.parse(Grabber.htmlDecode(rawData[1]));
                    const rawTags = JSON.parse(Grabber.htmlDecode(rawData[2]));

                    // Parse tags, giving translation the priority to allow user to use other languages than Japanese
                    const tags: string[] = rawTags.map((data: any): string => {
                        return data["tag_translation"] || data["tag"];
                    });

                    // Parse images
                    const imgMap = {
                        id: "illustId",
                        name: "illustTitle",
                        author: "userName",
                        creator_id: "userId",
                        tags: "tags",
                        width: "width",
                        height: "height",
                        preview_url: "url",
                    };
                    const images: IImage[] = rawItems.map((data: any): IImage => {
                        const img = Grabber.mapFields(data, imgMap);
                        img["sample_url"] = img["preview_url"].replace(/\/c\/\d+x\d+\/img-master\//g, "/img-master/");
                        img["file_url"] = urlSampleToFull(img["sample_url"]);
                        return img;
                    });

                    return {
                        images,
                        tags,
                        imageCount: Grabber.countToInt(Grabber.regexToConst("count", '<span class="count-badge">(?<count>\\d+)[^<]+</span>', src)),
                    };
                },
            },
            details: {
                url: (id: number, md5: string): string => {
                    return "/member_illust.php?mode=medium&illust_id=" + id;
                },
                parse: (src: string): IParsedDetails => {
                    const data = Grabber.regexMatch('<div class="img-container">\\s*<a[^>]+>\\s*<img\\s+src="(?<sample_url>[^"]+)"\\s*alt="(?<title>[^"]+)/(?<author>[^"]+)"', src);
                    const tags = Grabber.regexToTags('<a href="/tags\\.php[^"]+" class="text">(?<name>[^<]+)<', src);

                    // Page data is stored in a JS call when logged in
                    // const rawData = src.match(/}\)\(([^)]+)\)/)[1];

                    return {
                        imageUrl: urlSampleToFull(data["sample_url"]),
                        tags,
                    };
                },
            },
        },
    },
};

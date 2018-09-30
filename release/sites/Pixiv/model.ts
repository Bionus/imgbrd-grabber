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

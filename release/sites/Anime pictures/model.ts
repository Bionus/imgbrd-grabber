function completeImage(img: IImage): IImage {
    if (img["ext"] && img["ext"][0] === ".") {
        img["ext"] = img["ext"].substring(1);
    }

    img["file_url"] = `/pictures/download_image/${img["id"]}.${img["ext"] || "jpg"}`;

    if ((!img["sample_url"] || img["sample_url"].length < 5) && img["preview_url"] && img["preview_url"].length >= 5) {
        img["sample_url"] = img["preview_url"]
            .replace("_cp.", "_bp.")
            .replace("_sp.", "_bp.");
    }

    img["file_url"] = img["file_url"].replace(".jpg.webp", ".jpg");
    img["sample_url"] = (img["sample_url"] || "").replace(".jpg.webp", ".jpg");
    img["preview_url"] = (img["preview_url"] || "").replace(".jpg.webp", ".jpg");

    return img;
}

function sizeToUrl(size: string, key: string, ret: string[]): void {
    let op: number;
    if (size.indexOf("<=") === 0) {
        size = size.substr(2);
        op = 0;
    } else if (size.indexOf(">=") === 0) {
        size = size.substr(2);
        op = 1;
    } else if (size[0] === "<") {
        size = String(parseInt(size.substr(1), 10) - 1);
        op = 0;
    } else if (size[0] === ">") {
        size = String(parseInt(size.substr(1), 10) + 1);
        op = 1;
    }
    ret.push(key + "=" + size);
    if (op !== undefined) {
        ret.push(key + "_n=" + op);
    }
}

function searchToUrl(search: string): string {
    const parts = search.split(" ");
    const tags: string[] = [];
    const denied: string[] = [];
    const ret: string[] = [];
    for (const tag of parts) {
        const part = tag.trim();
        if (part.indexOf("width:") === 0) {
            sizeToUrl(part.substr(6), "ret_x", ret);
        } else if (part.indexOf("height:") === 0) {
            sizeToUrl(part.substr(7), "res_y", ret);
        } else if (part.indexOf("ratio:") === 0) {
            ret.push("aspect=" + part.substr(6));
        } else if (part.indexOf("order:") === 0) {
            ret.push("order_by=" + part.substr(6));
        } else if (part.indexOf("filetype:") === 0) {
            const ext = part.substr(9);
            ret.push("ext_" + ext + "=" + ext);
        } else if (part[0] === "-") {
            denied.push(encodeURIComponent(tag.substr(1)));
        } else {
            tags.push(encodeURIComponent(tag));
        }
    }
    ret.unshift("search_tag=" + tags.join(" "));
    ret.unshift("denied_tags=" + denied.join(" "));
    return ret.join("&");
}

const auth: { [id: string]: IAuth } = {
    session: {
        type: "post",
        url: "/login/submit",
        fields: [
            {
                key: "login",
                type: "username",
            },
            {
                key: "password",
                type: "password",
            },
        ],
        check: {
            type: "cookie",
            key: "asian_server",
        },
    },
};

export const source: ISource = {
    name: "Anime pictures",
    modifiers: ["width:", "height:", "ratio:", "order:", "filetype:"],
    forcedTokens: [],
    tagFormat: {
        case: "lower",
        wordSeparator: " ",
    },
    searchFormat: {
        and: " && ",
        or: " || ",
        parenthesis: false,
        precedence: "and",
    },
    auth,
    apis: {
        json: {
            name: "JSON",
            auth: [],
            search: {
                url: (query: any, opts: any, previous: any): string => {
                    const page = query.page - 1;
                    return "/pictures/view_posts/" + page + "?" + searchToUrl(query.search) + "&posts_per_page=" + opts.limit + "&lang=en&type=json";
                },
                parse: (src: string): IParsedSearch => {
                    const map = {
                        "created_at": "pubtime",
                        "preview_url": "small_preview",
                        "width": "width",
                        "md5": "md5",
                        "height": "height",
                        "id": "id",
                        "score": "score_number",
                        "sample_url": "big_preview",
                        "ext": "ext",
                        "file_size": "size",
                    };

                    const data = JSON.parse(src);

                    const images: IImage[] = [];
                    for (const image of data.posts) {
                        images.push(completeImage(Grabber.mapFields(image, map)));
                    }

                    return {
                        images,
                        imageCount: data["posts_count"],
                        pageCount: data["max_pages"],
                    };
                },
            },
            details: {
                url: (id: number, md5: string): string => {
                    return "/pictures/view_post/" + id + "?lang=en&type=json";
                },
                parse: (src: string): IParsedDetails => {
                    const data = JSON.parse(src);

                    const tags: ITag[] = data["tags_full"].map((tag: any) => {
                        return {
                            name: tag["name"],
                            count: tag["num"],
                            typeId: tag["type"],
                        };
                    });

                    return {
                        tags,
                        createdAt: data["pubtime"],
                        imageUrl: data["file_url"],
                    };
                },
            },
        },
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 80,
            search: {
                url: (query: any, opts: any, previous: any): string => {
                    const page = query.page - 1;
                    return "/pictures/view_posts/" + page + "?" + searchToUrl(query.search) + "&lang=en";
                },
                parse: (src: string): IParsedSearch => {
                    let wiki = Grabber.regexToConst("wiki", '<div class="posts_body_head">\\s*<h2>[^<]+</h2>\\s*(?<wiki>.+?)\s*</div>', src);
                    wiki = wiki ? wiki.replace(/href="\/pictures\/view_posts\/0\?search_tag=([^"&]+)(?:&[^"]+)?"/g, 'href="$1"') : wiki;
                    return {
                        images: Grabber.regexToImages('<span[^>]*data-pubtime="(?<created_at>[^"]+)">\\s*<a href="(?<page_url>/pictures/view_post/(?<id>\\d+)[^"]+)"(?:\\s*title="Anime picture (?<width>\\d+)x(?<height>\\d+)")?[^>]*>\\s*(?:<picture[^>]*>\\s*<source[^>]*>\\s*<img\\s*id="[^"]*"\\s*class="img_sp"\\s*src="(?<preview_url>[^"]+)"[^>]*>)?', src).map(completeImage),
                        pageCount: Grabber.regexToConst("page", "page of (?<page>\\d+)", src),
                        wiki,
                    };
                },
            },
            details: {
                url: (id: number, md5: string): string => {
                    return "/pictures/view_post/" + id + "?lang=en";
                },
                parse: (src: string): IParsedDetails => {
                    return {
                        tags: Grabber.regexToTags('<li id="tag_li_[^"]+"[^>]*>\\s*<a[^>]*">(?<name>[^<]+)</a>\\s*<span>(?<count>[^<]+)</span>\\s*</li>', src),
                    };
                },
            },
            tags: {
                url: (query: any, opts: any): string => {
                    const page = query.page - 1;
                    return "/pictures/view_all_tags/" + page + "?lang=en";
                },
                parse: (src: string): IParsedTags => {
                    return {
                        tags: Grabber.regexToTags("<tr>\\s*<td>(?<id>\\d+)</td>\\s*<td>\\s*<a.+?>(?<name>.+?)</a>.*?</td>\\s*<td>.*?</td>\\s*<td>.*?</td>\\s*<td>(?<type>.+?)</td>\\s*<td>(?<count>\\d+)</td>\\s*</tr>", src),
                    };
                },
            },
            check: {
                url: (): string => {
                    return "/";
                },
                parse: (src: string): boolean => {
                    return src.indexOf("mailto:stalkerg@gmail.com") !== -1;
                },
            },
        },
    },
};

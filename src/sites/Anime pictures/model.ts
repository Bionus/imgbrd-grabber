const jsonMap = {
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

function noWebpAvif(url: string): string {
    return url.replace(/(\.\w{3,4})\.(?:webp|avif)/, "$1");
}

function completeImage(img: IImage, raw: any): IImage {
    // Replace ".jpg" by just "jpg" in the extension
    if (img.ext && img.ext[0] === ".") {
        img.ext = img.ext.substring(1);
    }

    // Remove the ".webp" and ".avif" suffix to previews
    img.sample_url = noWebpAvif(img.sample_url || "");
    img.preview_url = noWebpAvif(img.preview_url || "");

    // If no URL is passed at all, build it ourselves
    if (!img.preview_url && !img.sample_url && !img.file_url) {
        const domain = "anime-pictures.net";
        const md5Part = img.md5?.substring(0, 3) + "/" + img.md5;
        const previewExt = raw["have_alpha"] === true ? "png" : "jpg";
        img.preview_url = `//opreviews.${domain}/${md5Part}_sp.${previewExt}`;
        img.sample_url = `//opreviews.${domain}/${md5Part}_bp.${previewExt}`;
        img.file_url = `//oimages.${domain}/${md5Part}.${img.ext}`;
    }

    if ((!img.sample_url || img.sample_url.length < 5) && img.preview_url && img.preview_url.length >= 5) {
        img.sample_url = img.preview_url
            .replace("_cp.", "_bp.")
            .replace("_sp.", "_bp.");
    }

    if ((!img.file_url || img.file_url.length < 5) && img.sample_url && img.sample_url.length >= 5) {
        img.file_url = img.sample_url
            .replace(/\/cdn\./, "/images.")
            .replace(/\/opreviews\./, "/oimages.")
            .replace(/\/previews\//, "/")
            .replace(/_[scb]p.\w{2,5}$/, "." + img.ext);
    }

    return img;
}

function sizeToUrl(size: string, key: string, ret: string[]): void {
    let op: number | undefined;
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

function searchToUrl(page: number, search: string, previous: IPreviousSearch | undefined): string {
    const parts = search.split(" ");
    const tags: string[] = [];
    const denied: string[] = [];
    const ret: string[] = [];
    for (const tag of parts) {
        const part = tag.trim();
        if (part.indexOf("width:") === 0) {
            sizeToUrl(part.substr(6), "res_x", ret);
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
        } else if (tag.length > 0) {
            tags.push(encodeURIComponent(tag));
        }
    }
    if (tags.length > 0) {
        ret.unshift("search_tag=" + tags.join(" "));
    }
    if (denied.length > 0) {
        ret.unshift("denied_tags=" + denied.join(" "));
    }
    if (previous && previous.minDate && previous.page === page - 1) {
        ret.push("last_page=" + (previous.page - 1));
        ret.push("last_post_date=" + previous.minDate);
    }
    return ret.join("&");
}

export const source: ISource = {
    name: "Anime pictures",
    modifiers: ["width:", "height:", "ratio:", "order:", "filetype:"],
    forcedTokens: ["tags"],
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
    auth: {
        session: {
            type: "post",
            url: "/login/submit",
            fields: [
                {
                    id: "pseudo",
                    key: "login",
                },
                {
                    id: "password",
                    key: "password",
                    type: "password",
                },
            ],
            check: {
                type: "cookie",
                key: "asian_server",
            },
        },
    },
    apis: {
        json: {
            name: "JSON",
            auth: [],
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string => {
                    const baseUrl = opts.baseUrl
                        .replace("//anime-pictures.", "//api.anime-pictures.")
                        .replace("//www.anime-pictures.", "//api.anime-pictures.");
                    const page = query.page - 1;
                    return baseUrl + "/api/v3/posts?page=" + page + "&" + searchToUrl(query.page, query.search, previous) + "&posts_per_page=" + opts.limit + "&lang=en";
                },
                parse: (src: string): IParsedSearch => {
                    const data = JSON.parse(src);

                    const images: IImage[] = [];
                    for (const image of data.posts) {
                        images.push(completeImage(Grabber.mapFields(image, jsonMap), image));
                    }

                    return {
                        images,
                        imageCount: data["posts_count"],
                        pageCount: data["max_pages"] + 1, // max_pages is an index, not a count, and pages start at 0
                    };
                },
            },
            details: {
                url: (id: string, md5: string, opts: IUrlDetailsOptions): string => {
                    const baseUrl = opts.baseUrl
                        .replace("//anime-pictures.", "//api.anime-pictures.")
                        .replace("//www.anime-pictures.", "//api.anime-pictures.");
                    return baseUrl + "/api/v3/posts/" + id + "?lang=en";
                },
                parse: (src: string): IParsedDetails => {
                    const data = JSON.parse(src);

                    const tags: ITag[] = data["tags"].map((tag: any) => ({
                        name: tag["tag"]["tag"],
                        count: tag["tag"]["num"],
                        typeId: tag["tag"]["type"],
                    }));

                    const img: IImage = completeImage(Grabber.mapFields(data["post"], jsonMap), data["post"]);

                    return {
                        tags,
                        createdAt: data["post"]["pubtime"],
                        imageUrl: img.file_url,
                    };
                },
            },
        },
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 80,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string => {
                    const page = query.page - 1;
                    return "/pictures/view_posts/" + page + "?" + searchToUrl(query.page, query.search, previous) + "&lang=en";
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
                url: (id: string, md5: string): string => {
                    return "/pictures/view_post/" + id + "?lang=en";
                },
                parse: (src: string): IParsedDetails => {
                    return {
                        tags: Grabber.regexToTags('<li id="tag_li_[^"]+"[^>]*>\\s*<a[^>]*">(?<name>[^<]+)</a>\\s*<span>(?<count>[^<]+)</span>\\s*</li>', src),
                    };
                },
            },
            tags: {
                url: (query: ITagsQuery): string => {
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

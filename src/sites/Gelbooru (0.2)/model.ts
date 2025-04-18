function completeImage(img: IImage): IImage {
    img.author = (img as any).owner;

    if ((!img.file_url || img.file_url.length < 5) && img.preview_url) {
        img.file_url = img.preview_url
            .replace("/thumbnails/", "/images/")
            .replace("/thumbnail_", "/");
    }

    return img;
}

export const source: ISource = {
    name: "Gelbooru (0.2)",
    modifiers: ["rating:general", "rating:safe", "rating:questionable", "rating:explicit", "user:", "fav:", "fastfav:", "md5:", "source:", "id:", "width:", "height:", "score:", "mpixels:", "filesize:", "date:", "gentags:", "arttags:", "chartags:", "copytags:", "approver:", "parent:", "sub:", "order:id", "order:id_desc", "order:score", "order:score_asc", "order:mpixels", "order:mpixels_asc", "order:filesize", "order:landscape", "order:portrait", "order:favcount", "order:rank", "parent:none", "unlocked:rating", "sort:updated", "sort:id", "sort:score", "sort:rating", "sort:user", "sort:height", "sort:width", "sort:parent", "sort:source", "sort:updated"],
    tagFormat: {
        case: "lower",
        wordSeparator: "_",
    },
    searchFormat: {
        and: " ",
    },
    auth: {
        url:  {
            type: "url",
            fields: [
                {
                    id: "userId",
                    key: "user_id",
                },
                {
                    id: "apiKey",
                    key: "api_key",
                },
            ],
        },
        session: {
            type: "post",
            url: "/index.php?page=account&s=login&code=00",
            fields: [
                {
                    id: "pseudo",
                    key: "user",
                },
                {
                    id: "password",
                    key: "pass",
                    type: "password",
                },
            ],
            check: {
                type: "cookie",
                key: "user_id",
            },
        },
    },
    apis: {
        xml: {
            name: "XML",
            auth: [],
            maxLimit: 100,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions): string | IError => {
                    const page: number = query.page - 1;
                    const search: string = query.search.replace(/(^| )order:/gi, "$1sort:");
                    const fav = search.match(/(?:^| )fav:(\d+)(?:$| )/);
                    if (fav) {
                        return { error: "XML API cannot search favorites" };
                    }
                    return "/index.php?page=dapi&s=post&q=index&limit=" + opts.limit + "&pid=" + page + "&tags=" + encodeURIComponent(search);
                },
                parse: (src: string): IParsedSearch | IError => {
                    const parsed = Grabber.parseXML(src);

                    // Handle error messages
                    if ("response" in parsed && parsed["response"]["@attributes"]["success"] === "false") {
                        return { error: parsed["response"]["@attributes"]["reason"] };
                    }

                    const data = Grabber.makeArray(parsed.posts.post);
                    const images: IImage[] = [];
                    for (const image of data) {
                        if (image && "id" in image) {
                            images.push(completeImage(Grabber.typedXML(image)));
                        } else if (image && "@attributes" in image) {
                            images.push(completeImage(image["@attributes"]));
                        }
                    }

                    return {
                        images,
                        imageCount: parsed.posts["@attributes"]["count"],
                    };
                },
            },
        },
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 42,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string | IError => {
                    try {
                        const baseUrl = opts.baseUrl.replace("//api.", "//");
                        const search: string = query.search.replace(/(^| )order:/gi, "$1sort:");
                        const fav = search.match(/(?:^| )fav:(\d+)(?:$| )/);
                        if (fav) {
                            const pagePart = Grabber.pageUrl(query.page, previous, 20000, "&pid={page}", "&pid={page}", " id:<{min}&p=1", (p: number) => (p - 1) * 50);
                            return baseUrl + "/index.php?page=favorites&s=view&id=" + fav[1] + pagePart;
                        } else {
                            const pagePart = Grabber.pageUrl(query.page, previous, 20000, "&pid={page}", "&pid={page}", " id:<{min}&p=1", (p: number) => (p - 1) * 42);
                            return baseUrl + "/index.php?page=post&s=list&tags=" + encodeURIComponent(search) + pagePart;
                        }
                    } catch (e: any) {
                        return { error: e.message };
                    }
                },
                parse: (src: string): IParsedSearch | IError => {
                    if (src.indexOf("Unable to search this deep") !== -1) {
                        return { error: "Page too far" };
                    }
                    const pageCountRaw = Grabber.regexMatch('<a href="[^"]+pid=(?<page>\\d+)[^"]*"[^>]*>[^<]+</a>\\s*(?:<b>(?<page_2>\\d+)</b>\\s*)?(?:</div>|<br ?/>)|<a href="[^"]+pid=(?<page_3>\\d+)[^"]*"[^>]*>(?:&raquo;|>>)</a>', src);
                    const pageCount = pageCountRaw && pageCountRaw["page"];
                    const images = Grabber.regexToImages('(?:<span[^>]*(?:\\sid="?\\w(?<id>\\d+)"?)?>\\s*)?<a[^>]*(?:\\sid="?\\w(?<id_2>\\d+)"?)[^>]*>\\s*<img [^>]*(?:src|data-original)="(?<preview_url>[^"]+/thumbnail_(?<md5>[^.]+)\\.[^"]+)" [^>]*title="\\s*(?<tags>[^"]+)"[^>]*/?>\\s*</a>|<img\\s+class="preview"\\s+src="(?<preview_url_2>[^"]+/thumbnail_(?<md5_2>[^.]+)\\.[^"]+)" [^>]*title="\\s*(?<tags_2>[^"]+)"[^>]*/?>', src);
                    for (const img of images) {
                        const json = src.match(new RegExp("posts\\[" + img.id + "\\]\\s*=\\s*({.+?})"))?.[1];
                        if (json) {
                            img.rating = json.match(/'rating'\s*:\s*'([^']+)'/)?.[1].toLowerCase();
                            img.score = json.match(/'score'\s*:\s*(\d+)/)?.[1];
                            img.author = json.match(/'user'\s*:\s*'([^']+)'/)?.[1];
                        }
                    }
                    return {
                        images: images.map(completeImage),
                        tags: Grabber.regexToTags('<li class="tag-type-(?<type>[^"]+)">(?:[^<]*<a[^>]*>[^<]*</a>)*[^<]*<a[^>]*>(?<name>[^<]*)</a>[^<]*<span[^>]*>\\s*(?<count>\\d+)\\s*</span>', src),
                        pageCount: pageCount ? parseInt(pageCount, 10) / 42 + 1 : undefined,
                    };
                },
            },
            details: {
                url: (id: string, md5: string, opts: IUrlDetailsOptions): string => {
                    const baseUrl = opts.baseUrl.replace("//api.", "//");
                    return baseUrl + "/index.php?page=post&s=view&id=" + id;
                },
                parse: (src: string): IParsedDetails => {
                    return {
                        tags: Grabber.regexToTags('<li class="tag-type-(?<type>[^"]+)">(?:[^<]*(?:<span[^>]*>[^<]*)?<a[^>]*>[^<]*</a>(?:[^<]*</span>)?)*[^<]*<a[^>]*>(?<name>[^<]*)</a>[^<]*<span[^>]*>(?<count>\\d+)</span>[^<]*</li>', src),
                        imageUrl: Grabber.regexToConst("url", '<img[^>]+src="([^"]+)"[^>]+onclick="Note\\.toggle\\(\\);"[^>]*/>', src),
                        createdAt: src.match(/>Posted:\s*([^<]+)</i)?.[1],
                        source: src.match(/>Source:\s*<a href="([^"]+)"/i)?.[1],
                    };
                },
            },
            endpoints: {
                pool_list: {
                    name: "Pools",
                    input: {},
                    url: (query: Record<never, string>, opts: IUrlOptions): string => {
                        const pid = (opts.page - 1) * 25;
                        return "/index.php?page=pool&s=list&pid=" + String(pid);
                    },
                    parse: (src: string): IParsedSearch => {
                        const html = Grabber.parseHTML(src);

                        const images: IImage[] = [];

                        const rows = html.find("table tr");
                        for (const row of rows) {
                            const parts = row.find("td");
                            const link = parts[1].find("a")[0];
                            const id = link.attr("href").match(/id=(\d+)/)[1];

                            images.push({
                                id,
                                name: link.innerText(),
                                type: "gallery",
                                gallery_count: parts[2].innerText().match(/(\d+)\s+Images/)[1],
                                details_endpoint: {
                                    endpoint: "pool_details",
                                    input: { id },
                                },
                            })
                        }

                        return { images };
                    },
                },
                pool_details: {
                    input: {
                        id: {
                            type: "input",
                        },
                    },
                    url: (query: Record<"id", number>): string => {
                        return "/index.php?page=pool&s=show&id=" + String(query.id);
                    },
                    parse: (src: string): IParsedGallery => {
                        const images = Grabber.regexToImages('<span[^>]*(?: id="?\\w(?<id>\\d+)"?)?>\\s*<a[^>]*(?: id="?\\w(?<id_2>\\d+)"?)[^>]*>\\s*<img [^>]*(?:src|data-original)="(?<preview_url>[^"]+/thumbnail_(?<md5>[^.]+)\\.[^"]+)" [^>]*title="\\s*(?<tags>[^"]+)"[^>]*/?>\\s*</a>|<img\\s+class="preview"\\s+src="(?<preview_url_2>[^"]+/thumbnail_(?<md5_2>[^.]+)\\.[^"]+)" [^>]*title="\\s*(?<tags_2>[^"]+)"[^>]*/?>', src);
                        return {
                            images: images.map(completeImage),
                        };
                    },
                },
            },
            tagTypes: false,
            tags: {
                url: (query: ITagsQuery, opts: IUrlOptions): string => {
                    const baseUrl = opts.baseUrl.replace("//api.", "//");
                    const sorts = { count: "desc", date: "asc", name: "asc" };
                    const orderBys = { count: "index_count", date: "updated", name: "tag" };
                    const page: number = (query.page - 1) * 50;
                    return baseUrl + "/index.php?page=tags&s=list&pid=" + page + "&sort=" + sorts[query.order] + "&order_by=" + orderBys[query.order];
                },
                parse: (src: string): IParsedTags => {
                    return {
                        tags: Grabber.regexToTags('(?:<td>(?<count_2>\\d+)</td>\\s*)?<td><span class="tag-type-(?<type>[^"]+)"><a[^>]+>(?<name>.+?)</a></span>(?:\\s*<span class="tag-count">(?<count>\\d+)</span>)?</td>', src),
                    };
                },
            },
            check: {
                url: (): string => {
                    return "/";
                },
                parse: (src: string): boolean => {
                    return src.search(/Running Gelbooru(?: Beta)? 0\.2/) !== -1
                        || src.search(/Running <a[^>]*>Gelbooru<\/a>(?: Beta)? 0\.2/) !== -1;
                },
            },
        },
    },
};

const makeTag = (match: any): ITag => {
    match["count"] = Grabber.countToInt(match["count"]);
    return match;
};

__source = {
    name: "Gelbooru (0.2)",
    modifiers: ["rating:safe", "rating:questionable", "rating:explicit", "user:", "fav:", "fastfav:", "md5:", "source:", "id:", "width:", "height:", "score:", "mpixels:", "filesize:", "date:", "gentags:", "arttags:", "chartags:", "copytags:", "approver:", "parent:", "sub:", "order:id", "order:id_desc", "order:score", "order:score_asc", "order:mpixels", "order:mpixels_asc", "order:filesize", "order:landscape", "order:portrait", "order:favcount", "order:rank", "parent:none", "unlocked:rating"],
    auth: {},
    apis: {
        xml: {
            name: "XML",
            auth: [],
            maxLimit: 1000,
            search: {
                url: (query: any, opts: any, previous: any): IUrl | IError | string => {
                    return "/index.php?page=dapi&s=post&q=index&limit=" + opts.limit + "&pid=" + query.page + "&tags=" + query.search;
                },
                parse: (src: string): IParsedSearch => {
                    const data = Grabber.parseXML(src).posts.post;

                    const images: IImage[] = [];
                    for (const image of data) {
                        images.push(image["@attributes"]);
                    }

                    return { images };
                },
            },
        },
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 42,
            search: {
                url: (query: any, opts: any, previous: any): IUrl | IError | string => {
                    const pagePart = Grabber.pageUrl(query.page, previous, 476, "&pid={pid}", " id:<{min}&p=1", "&pid={pid}");
                    return "/index.php?page=post&s=list&tags=" + query.search + pagePart;
                },
                parse: (src: string): IParsedSearch => {
                    // Tags
                    const tags: { [name: string]: ITag } = {};
                    const tagMatches = Grabber.regexMatches('<li class="tag-type-(?<type>[^"]+)">(?:[^<]*<a[^>]*>[^<]*</a>)*[^<]*<a[^>]*>(?<name>[^<]*)</a>[^<]*<span[^>]*>(?<count>\\d+)</span>[^<]*</li>', src);
                    for (const tagMatch of tagMatches) {
                        if (!(tagMatch["name"] in tags)) {
                            tags[tagMatch["name"]] = makeTag(tagMatch);
                        }
                    }

                    // Images
                    const images: IImage[] = [];
                    const imgMatches = Grabber.regexMatches('<span[^>]*(?: id="?\\w(?<id>\\d+)"?)?>\\s*<a[^>]*(?: id="?\\w(?<id_2>\\d+)"?)[^>]*>\\s*<img [^>]*(?:src|data-original)="(?<preview_url>[^"]+/thumbnail_(?<md5>[^.]+)\\.[^"]+)" [^>]*title="\\s*(?<tags>[^"]+)"[^>]*/?>\\s*</a>|<img\\s+class="preview"\\s+src="(?<preview_url_2>[^"]+/thumbnail_(?<md5_2>[^.]+)\\.[^"]+)" [^>]*title="\\s*(?<tags_2>[^"]+)"[^>]*/?>', src);
                    for (const imgMatch of imgMatches) {
                        if ("json" in imgMatch) {
                            const json = JSON.parse(imgMatch["json"]);
                            for (const key in json) {
                                imgMatch[key] = json[key];
                            }
                        }
                        images.push(imgMatch);
                    }

                    // Last page
                    let pageCount: number;
                    const lastPageMatches = Grabber.regexMatches('<a href="[^"]+pid=(?<page>\\d+)[^"]*" alt="last page"[^>]*>');
                    for (const lastPageMatch of lastPageMatches) {
                        pageCount = lastPageMatch["page"];
                    }

                    return { images, tags, pageCount };
                },
            },
            details: {
                url: (id: number, md5: string): IUrl | IError | string => {
                    return "/index.php?page=post&s=view&id=" + id;
                },
                parse: (src: string): IParsedDetails => {
                    // Tags
                    const tags: { [name: string]: ITag } = {};
                    const tagMatches = Grabber.regexMatches('<li class="tag-type-(?<type>[^"]+)">(?:[^<]*<a[^>]*>[^<]*</a>)*[^<]*<a[^>]*>(?<name>[^<]*)</a>[^<]*<span[^>]*>(?<count>\\d+)</span>[^<]*</li>', src);
                    for (const tagMatch of tagMatches) {
                        if (!(tagMatch["name"] in tags)) {
                            tags[tagMatch["name"]] = makeTag(tagMatch);
                        }
                    }

                    // Image url
                    let imageUrl: string;
                    const imageUrlMatches = Grabber.regexMatches('<img[^>]+src="([^"]+)"[^>]+onclick="Note\\.toggle\\(\\);"[^>]*/>', src);
                    for (const imageUrlMatch of imageUrlMatches) {
                        imageUrl = imageUrlMatch["url"];
                    }

                    return { tags, imageUrl };
                },
            },
            tags: {
                url: (query: any, opts: any): IUrl | IError | string => {
                    return "/index.php?page=tags&s=list&pid=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    // Tags
                    const tags: ITag[] = [];
                    const tagMatches = Grabber.regexMatches('<tr>\\s*<td>(?<count>\\d+)</td>\\s*<td><span class="tag-type-(?<type>[^"]+)"><a[^>]+>(?<name>.+?)</a></span></td>', src);
                    for (const tagMatch of tagMatches) {
                        tags.push(makeTag(tagMatch));
                    }

                    return { tags };
                },
            },
        },
    },
};

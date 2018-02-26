export const source: ISource = {
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
                    return {
                        images: Grabber.regexToImages('<span[^>]*(?: id="?\\w(?<id>\\d+)"?)?>\\s*<a[^>]*(?: id="?\\w(?<id_2>\\d+)"?)[^>]*>\\s*<img [^>]*(?:src|data-original)="(?<preview_url>[^"]+/thumbnail_(?<md5>[^.]+)\\.[^"]+)" [^>]*title="\\s*(?<tags>[^"]+)"[^>]*/?>\\s*</a>|<img\\s+class="preview"\\s+src="(?<preview_url_2>[^"]+/thumbnail_(?<md5_2>[^.]+)\\.[^"]+)" [^>]*title="\\s*(?<tags_2>[^"]+)"[^>]*/?>', src),
                        tags: Grabber.regexToTags('<li class="tag-type-(?<type>[^"]+)">(?:[^<]*<a[^>]*>[^<]*</a>)*[^<]*<a[^>]*>(?<name>[^<]*)</a>[^<]*<span[^>]*>(?<count>\\d+)</span>[^<]*</li>', src),
                        pageCount: Grabber.regexToConst("page", '<a href="[^"]+pid=(?<page>\\d+)[^"]*" alt="last page"[^>]*>'),
                    };
                },
            },
            details: {
                url: (id: number, md5: string): IUrl | IError | string => {
                    return "/index.php?page=post&s=view&id=" + id;
                },
                parse: (src: string): IParsedDetails => {
                    return {
                        tags: Grabber.regexToTags('<li class="tag-type-(?<type>[^"]+)">(?:[^<]*<a[^>]*>[^<]*</a>)*[^<]*<a[^>]*>(?<name>[^<]*)</a>[^<]*<span[^>]*>(?<count>\\d+)</span>[^<]*</li>', src),
                        imageUrl: Grabber.regexToConst("url", '<img[^>]+src="([^"]+)"[^>]+onclick="Note\\.toggle\\(\\);"[^>]*/>', src),
                    };
                },
            },
            tags: {
                url: (query: any, opts: any): IUrl | IError | string => {
                    return "/index.php?page=tags&s=list&pid=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    return {
                        tags: Grabber.regexToTags('<tr>\\s*<td>(?<count>\\d+)</td>\\s*<td><span class="tag-type-(?<type>[^"]+)"><a[^>]+>(?<name>.+?)</a></span></td>', src),
                    };
                },
            },
        },
    },
};

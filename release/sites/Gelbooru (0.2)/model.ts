function completeImage(img: IImage): IImage {
    if (!img["file_url"] || img["file_url"].length < 5) {
        img["file_url"] = img["preview_url"]
            .replace("/thumbnails/", "/images/")
            .replace("/thumbnail_", "/");
    }

    return img;
}

export const source: ISource = {
    name: "Gelbooru (0.2)",
    modifiers: ["rating:safe", "rating:questionable", "rating:explicit", "user:", "fav:", "fastfav:", "md5:", "source:", "id:", "width:", "height:", "score:", "mpixels:", "filesize:", "date:", "gentags:", "arttags:", "chartags:", "copytags:", "approver:", "parent:", "sub:", "order:id", "order:id_desc", "order:score", "order:score_asc", "order:mpixels", "order:mpixels_asc", "order:filesize", "order:landscape", "order:portrait", "order:favcount", "order:rank", "parent:none", "unlocked:rating", "sort:updated", "sort:id", "sort:score", "sort:rating", "sort:user", "sort:height", "sort:width", "sort:parent", "sort:source", "sort:updated"],
    tagFormat: {
        case: "lower",
        wordSeparator: "_",
    },
    searchFormat: {
        and: " ",
    },
    auth: {
        session: {
            type: "post",
            url: "/index.php?page=account&s=login&code=00",
            fields: [
                {
                    key: "user",
                    type: "username",
                },
                {
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
            maxLimit: 1000,
            search: {
                url: (query: any, opts: any, previous: any): string | IError => {
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
                        if (image && "@attributes" in image) {
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
                url: (query: any, opts: any, previous: any): string | IError => {
                    try {
                        const page: number = (query.page - 1) * 42;
                        const search: string = query.search.replace(/(^| )order:/gi, "$1sort:");
                        const pagePart = Grabber.pageUrl(page, previous, 20000, "&pid={page}", " id:<{min}&p=1", "&pid={page}");
                        const fav = search.match(/(?:^| )fav:(\d+)(?:$| )/);
                        if (fav) {
                            const pageFav: number = (query.page - 1) * 50;
                            const pagePartFav = Grabber.pageUrl(pageFav, previous, 20000, "&pid={page}", " id:<{min}&p=1", "&pid={page}");
                            return "/index.php?page=favorites&s=view&id=" + fav[1] + pagePartFav;
                        }
                        return "/index.php?page=post&s=list&tags=" + encodeURIComponent(search) + pagePart;
                    } catch (e) {
                        return { error: e.message };
                    }
                },
                parse: (src: string): IParsedSearch => {
                    const pageCount = Grabber.regexToConst("page", '<a href="[^"]+pid=(?<page>\\d+)[^"]*"[^>]*>[^<]+</a>(?:</div>|<br ?/>)', src);
                    return {
                        images: Grabber.regexToImages('<span[^>]*(?: id="?\\w(?<id>\\d+)"?)?>\\s*<a[^>]*(?: id="?\\w(?<id_2>\\d+)"?)[^>]*>\\s*<img [^>]*(?:src|data-original)="(?<preview_url>[^"]+/thumbnail_(?<md5>[^.]+)\\.[^"]+)" [^>]*title="\\s*(?<tags>[^"]+)"[^>]*/?>\\s*</a>|<img\\s+class="preview"\\s+src="(?<preview_url_2>[^"]+/thumbnail_(?<md5_2>[^.]+)\\.[^"]+)" [^>]*title="\\s*(?<tags_2>[^"]+)"[^>]*/?>', src).map(completeImage),
                        tags: Grabber.regexToTags('<li class="tag-type-(?<type>[^"]+)">(?:[^<]*<a[^>]*>[^<]*</a>)*[^<]*<a[^>]*>(?<name>[^<]*)</a>[^<]*<span[^>]*>(?<count>\\d+)</span>[^<]*</li>', src),
                        pageCount: pageCount && parseInt(pageCount, 10) / 42 + 1,
                    };
                },
            },
            details: {
                url: (id: number, md5: string): string => {
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
                url: (query: any, opts: any): string => {
                    return "/index.php?page=tags&s=list&pid=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    return {
                        tags: Grabber.regexToTags('<tr>\\s*<td>(?<count>\\d+)</td>\\s*<td><span class="tag-type-(?<type>[^"]+)"><a[^>]+>(?<name>.+?)</a></span></td>', src),
                    };
                },
            },
            check: {
                url: (): string => {
                    return "/";
                },
                parse: (src: string): boolean => {
                    return src.search(/Running Gelbooru Beta 0\.2/) !== -1
                        || src.search(/Running <a href=['"][^'"]+['"]>Gelbooru<\/a> Beta 0\.2/) !== -1;
                },
            },
        },
    },
};

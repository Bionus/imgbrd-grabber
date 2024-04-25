function parseCommonImage(item: any, pageUrl: any, preview: any): IImage {
    const parsedUrl = Grabber.regexMatch("/g/(?<id>\\d+)/(?<token>[^/]+)/", pageUrl);

    return {
        type: "gallery",
        page_url: pageUrl,
        id: parsedUrl["id"],
        md5: parsedUrl["id"] + "/" + parsedUrl["token"],
        tokens: {
            token: parsedUrl["token"],
            category: item.find(".cn, .cs")[0].innerText(),
        },
        preview_url: preview.attr("data-src") || preview.attr("src"),
        created_at: item.find("[id^=posted_]")[0].innerText(),
        name: item.find(".glink")[0].innerText(),
    };
}

function parseCompactImage(item: any): IImage | null {
    // Skip header
    if (item.find("th").length > 0) {
        return null;
    }

    // Skip ads
    if (item.find("script").length > 0) {
        return null;
    }

    const pageUrl = item.find(".glname a")[0].attr("href");
    const preview = item.find(".glthumb img")[0];

    return {
        ...parseCommonImage(item, pageUrl, preview),
        gallery_count: item.find(".glthumb")[0].innerHTML().match(/>(\d+) pages?</)[1],
        author: item.find(".glhide a")[0]?.innerText(), // Can be missing for "Disowned" galleries
        tags: item.find(".glname .gt, .glname .gtl").map((tag: any) => tag.attr("title")),
    };
}

function parseThumbnailImage(item: any): IImage {
    const pageUrl = item.find("a")[0].attr("href");
    const preview = item.find("img")[0];

    return {
        ...parseCommonImage(item, pageUrl, preview),
        gallery_count: item.innerHTML().match(/>(\d+) pages?</)[1],
    };
}

function parseExtendedImage(item: any): IImage | null {
    // Skip ads
    if (item.find("script").length > 0) {
        return null;
    }

    const pageUrl = item.find(".gl1e a")[0].attr("href");
    const preview = item.find(".gl1e img")[0];

    return {
        ...parseCommonImage(item, pageUrl, preview),
        gallery_count: item.find(".gl3e")[0].innerHTML().match(/>(\d+) pages?</)[1],
        author: item.find(".gl3e a")[0]?.innerText(), // Can be missing for "Disowned" galleries
        tags: item.find(".gl4e .gt, .gl4e .gtl").map((tag: any) => tag.attr("title")),
    };
}

function cssToObject(css: string): any {
    const ret: any = {};
    css.split(";").map((style: string) => {
        style = style.trim();
        const index: number = style.indexOf(":");
        if (index < 0) {
            return;
        }
        ret[style.substr(0, index).trim()] = style.substr(index + 1).trim();
    });
    return ret;
}

function sizeToInt(size: string): number {
    const match = size.match(/^(-?)(\d+)\w*$/);
    if (!match) {
        return 0;
    }
    const val = parseInt(match[2], 10);
    if (match[1].length > 0) {
        return -val;
    }
    return val;
}

function parseSearch(srch: string): { cats: string, search: string } {
    let cats = "0";
    const tags: string[] = [];

    const parts = srch.split(" ");
    for (const tag of parts) {
        const part = tag.trim();
        if (part.indexOf("cats:") === 0) {
            cats = part.substr(5);
        } else {
            tags.push(part);
        }
    }

    const search = tags.join(" ");
    return { cats, search };
}

export const source: ISource = {
    name: "E-Hentai",
    modifiers: ["cats:"],
    forcedTokens: ["*"],
    searchFormat: {
        and: " ",
    },
    auth: {
        post: {
            type: "post",
            url: "https://forums.e-hentai.org/index.php?act=Login&CODE=01",
            fields: [
                {
                    id: "pseudo",
                    key: "UserName",
                },
                {
                    id: "password",
                    key: "PassWord",
                    type: "password",
                },
                {
                    type: "const",
                    key: "CookieDate",
                    value: "1",
                },
                {
                    type: "const",
                    key: "b",
                    value: "d",
                },
                {
                    type: "const",
                    key: "bt",
                    value: "1-1",
                },
            ],
            check: {
                type: "cookie",
                key: "ipb_member_id",
            },
        },
    },
    apis: {
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 25,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string => {
                    const s = parseSearch(query.search);
                    const pagePart = Grabber.pageUrl(query.page, previous, 1, "", "prev={max}", "next={min}");
                    return "/?" + pagePart + "&f_cats=" + s.cats + "&f_search=" + encodeURIComponent(s.search);
                },
                parse: (src: string): IParsedSearch | IError => {
                    const html = Grabber.parseHTML(src);

                    // Check display mode
                    const modeOption = html.find("#dms select option[selected], .searchnav select option[selected]");
                    let mode = "l";
                    if (!modeOption || !Array.isArray(modeOption) || modeOption.length === 0) {
                        console.warn("Parsing mode not found, falling back to 'Compact'"); // tslint:disable-line: no-console
                    } else {
                        mode = modeOption[0].attr("value");
                        if (["m", "p", "l", "e", "t"].indexOf(mode) === -1) {
                            console.warn(`Unknown parsing mode "${mode}", falling back to 'Compact'`); // tslint:disable-line: no-console
                            mode = "l"
                        }
                    }

                    // Use different parser depending on the display mode
                    let itemQuery;
                    let parseFunction: (item: any) => IImage | null;
                    if (mode === "m" || mode === "p" || mode === "l") {
                        itemQuery = "table.itg > tbody > tr, table.itg > tr";
                        parseFunction = parseCompactImage;
                    } else if (mode === "e") {
                        itemQuery = "table.itg > tbody > tr, table.itg > tr";
                        parseFunction = parseExtendedImage;
                    } else if (mode === "t") {
                        itemQuery = "div.itg > div.gl1t";
                        parseFunction = parseThumbnailImage;
                    }

                    // Parse all images
                    const images: IImage[] = [];
                    for (const item of html.find(itemQuery)) {
                        try {
                            const image = parseFunction!(item);
                            if (image) {
                                images.push(image);
                            }
                        } catch (e) {
                            console.warn("Error parsing image: " + e + " / " + item.innerHTML()); // tslint:disable-line: no-console
                        }
                    }

                    return {
                        images,
                        pageCount: Grabber.countToInt(Grabber.regexToConst("page", ">(?<page>[0-9,]+)</a></td><td[^>]*>(?:&gt;|<a[^>]*>&gt;</a>)</td>", src)),
                        imageCount: Grabber.countToInt(Grabber.regexToConst("count", ">Showing (?<count>[0-9,]+) results<|Found about (?<count_2>[0-9,]+) results.", src)),
                        urlNextPage: Grabber.regexToConst("url", '<a id="unext" href="(?<url_2>[^"]+)">', src),
                        urlPrevPage: Grabber.regexToConst("url", '<a id="uprev" href="(?<url_2>[^"]+)">', src),
                    };
                },
            },
            gallery: {
                url: (query: IGalleryQuery): string => {
                    return "/g/" + query.md5 + "/?p=" + (query.page - 1);
                },
                parse: (src: string): IParsedGallery => {
                    const html = Grabber.parseHTML(src);

                    const posted = src.match(/>Posted:<\/td>\s*<td.*?>(.+?)</)?.[1];
                    const author = html.find("#gdn a")[0].innerText();

                    /*const tags: ITag[] = [];
                    const tagGroups = html.find("#taglist")[0].find("tr");
                    for (const tagGroup of tagGroups) {
                        const td = tagGroup.find("td");
                        let type = td[0].innerText();
                        type = type.substr(0, type.length - 1);
                        const list = td[1].find("a").map((tag: any) => tag.innerText());
                        tags.push(...list.map((name: string) => ({ type, name })));
                    }*/

                    const images: IImage[] = [];
                    let matches = Grabber.regexMatches('<div class="gdtm"[^>]*><div style="(?<div_style>[^"]+)"><a href="(?<page_url>[^"]+)"><img[^>]*></a></div>', src);
                    if (matches.length < 1) {
                        matches = Grabber.regexMatches('<div class="gdtl"[^>]*><a href="(?<page_url>[^"]+)"><img[^>]*src="(?<preview_url>[^"]+)"[^>]*></a></div>', src);
                    }
                    for (const match of matches) {
                        if ("div_style" in match) {
                            const styles = cssToObject(match["div_style"]);
                            delete match["div_style"];

                            const background = styles["background"].match(/url\(([^)]+)\) ([^ ]+) ([^ ]+)/);
                            match["preview_url"] = background[1];
                            match["preview_rect"] = [
                                -sizeToInt(background[2]),
                                -sizeToInt(background[3]),
                                sizeToInt(styles["width"]),
                                sizeToInt(styles["height"]),
                            ].join(";"); // x;y;w;h
                        }

                        match["created_at"] = posted;
                        match["author"] = author;
                        // match["tags"] = tags;
                        images.push(match);
                    }

                    return {
                        images,
                        pageCount: Grabber.countToInt(Grabber.regexToConst("page", ">(?<page>[0-9,]+)</a></td><td[^>]*>(?:&gt;|<a[^>]*>&gt;</a>)</td>", src)),
                        imageCount: Grabber.countToInt(Grabber.regexToConst("count", '<p class="gpc">Showing [0-9,]+ - [0-9,]+ of (?<count>[0-9,]+) images</p>', src)),
                        urlNextPage: Grabber.regexToConst("url", '<td[^>]*><a[^>]+href="(?<url>[^"]+)"[^>]*>&gt;</a></td>', src),
                        urlPrevPage: Grabber.regexToConst("url", '<td[^>]*><a[^>]+href="(?<url>[^"]+)"[^>]*>&lt;</a></td>', src),
                    };
                },
            },
            details: {
                url: (id: string, md5: string): IError => {
                    return { error: "Not supported (view token)" };
                },
                parse: (src: string): IParsedDetails => {
                    // Grabber.regexMatches("<div>(?<filename>[^:]*) :: (?<width>\\d+) x (?<height>\\d+) :: (?<filesize>[^ ]+ [KM]B)</div>", src);
                    return {
                        imageUrl: Grabber.regexToConst("url", '<img id="img" src="(?<url>[^"]+)"', src),
                    };
                },
            },
        },
    },
};

function completeImage(img: IImage): IImage {
    const resourcesURL: string = "https://static1.e621.net/data";
    const md5PartOne: string = img.md5.slice(0, 2);
    const md5PartTwo: string = img.md5.slice(2, 4);

    if (img.ext && img.ext[0] === ".") {
        img.ext = img.ext.substr(1);
    }

    if (!img.file_url || img.file_url.length < 5) {
        img.file_url = `${resourcesURL}/${md5PartOne}/${md5PartTwo}/${img.md5}.${img.ext || "jpg"}`;
    } else {
        img.file_url = img.file_url
            .replace("/preview/", "/")
            .replace("/ssd/", "/")
            .replace("/sample/[^.]*sample-", "/");
    }

    if (!img.sample_url || img.sample_url.length < 5) {
        img.sample_url = `${resourcesURL}/sample/${md5PartOne}/${md5PartTwo}/${img.md5}.${img.ext || "jpg"}`;
    }

    if (!img.preview_url || img.preview_url.length < 5) {
        img.preview_url = `${resourcesURL}/preview/${md5PartOne}/${md5PartTwo}/${img.md5}.${img.ext || "jpg"}`;
    }

    return img;
}

export const source: ISource = {
    name: "E621",
    modifiers: ["rating:safe", "rating:questionable", "rating:explicit", "rating:s", "rating:q", "rating:e", "user:", "fav:", "fastfav:", "md5:", "source:", "id:", "width:", "height:", "score:", "mpixels:", "filesize:", "date:", "gentags:", "arttags:", "chartags:", "copytags:", "approver:", "parent:", "sub:", "status:any", "status:deleted", "status:active", "status:flagged", "status:pending", "order:id", "order:id_desc", "order:score", "order:score_asc", "order:mpixels", "order:mpixels_asc", "order:filesize", "order:landscape", "order:portrait", "order:favcount", "order:rank", "order:change", "order:change_desc", "parent:none", "unlocked:rating"],
    forcedTokens: ["filename"],
    tagFormat: {
        case: "lower",
        wordSeparator: "_",
    },
    searchFormat: {
        and: " ",
        or: {
            separator: " ",
            prefix: "~",
        },
        parenthesis: false,
        precedence: "or",
    },
    auth: {
        url: {
            type: "url",
            fields: [
                {
                    id: "pseudo",
                    key: "login",
                },
                {
                    id: "password",
                    type: "password",
                },
                {
                    key: "password_hash",
                    type: "hash",
                    hash: "sha1",
                    salt: "choujin-steiner--%password%--",
                },
            ],
        },
        session: {
            type: "post",
            url: "/session",
            fields: [
                {
                    id: "pseudo",
                    key: "name",
                },
                {
                    id: "password",
                    key: "password",
                    type: "password",
                },
            ],
            check: {
                type: "cookie",
                key: "password_hash",
            },
        },
    },
    apis: {
        json: {
            name: "JSON",
            auth: [],
            maxLimit: 200,
            search: {
                parseErrors: true,
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string | IError => {
                    try {
                        const pagePart = Grabber.pageUrl(query.page, previous, 1000, "{page}", "a{max}", "b{min}");
                        return "/posts.json?limit=" + opts.limit + "&page=" + pagePart + "&tags=" + encodeURIComponent(query.search);
                    } catch (e) {
                        return { error: e.message };
                    }
                },
                parse: (src: string): IParsedSearch | IError => {
                    const map = {
                        "created_at": "created_at",
                        "status": "status",
                        "source": "source",
                        "has_comments": "has_comments",
                        "file_url": "file_url",
                        "sample_url": "large_file_url",
                        "change": "change_seq",
                        "sample_width": "sample_width",
                        "has_children": "has_children",
                        "preview_url": "preview_file_url",
                        "width": "image_width",
                        "md5": "md5",
                        "preview_width": "preview_width",
                        "sample_height": "sample_height",
                        "parent_id": "parent_id",
                        "height": "image_height",
                        "has_notes": "has_notes",
                        "creator_id": "uploader_id",
                        "file_size": "file_size",
                        "id": "id",
                        "preview_height": "preview_height",
                        "rating": "rating",
                        "tags": "tag_string",
                        "author": "uploader_name",
                        "score": "score",
                        "tags_artist": "tag_string_artist",
                        "tags_character": "tag_string_character",
                        "tags_copyright": "tag_string_copyright",
                        "tags_general": "tag_string_general",
                        "tags_meta": "tag_string_meta",
                    };

                    let data = JSON.parse(src);

                    if ("success" in data && data["success"] === false && "message" in data) {
                        return { error: data["message"] };
                    }

                    //Data in "posts" array
                    data = data["posts"];

                    const images: IImage[] = [];
                    for (const image of data) {
                        const img = Grabber.mapFields(image, map);

                        img.ext = image.file.ext;
                        img.created_at = Math.floor(Date.parse(image.created_at) / 1000);

                        //Determine flags
                        img.status = "active";
                        if(image.flags.pending === true) {
                            img.status = "pending";
                        } else if(image.flags.flagged === true) {
                            img.status = "flagged";
                        } else if(image.flags.deleted === true) {
                            img.status = "deleted";
                        }

                        img.has_comments = image.comment_count > 0 ? true : false;
                        img.file_url = image.file.url;
                        img.sample_url = image.sample.url;
                        img.has_children = image.relationships.has_children;
                        img.preview_url = image.preview.url;
                        img.width = image.file.width;
                        img.md5 = image.file.md5;
                        img.preview_width = image.preview.width;
                        img.sample_height = image.sample.height;
                        img.parent_id = image.relationships.parent_id;
                        img.height = image.file.height;
                        img.file_size = image.file.size;
                        img.preview_height = image.preview.height;
                        img.score = image.score.total;

                        //If multiple sources exists, use first one.
                        if(image.sources.length == 0) {
                            img.source = null;
                        } else {
                            img.source = image.sources[0];
                        }

                        //Tags to string
                        const tags_array: string[] = [];
                        const tags_artist_array: string[] = [];
                        const tags_character_array: string[] = [];
                        const tags_copyright_array: string[] = [];
                        const tags_general_array: string[] = [];
                        const tags_meta_array: string[] = [];

                        Object.keys(image.tags).forEach(function(key){
                            Array.prototype.push.apply(tags_array, image.tags[key]);
                            switch(key) {
                                case "general":
                                    Array.prototype.push.apply(tags_general_array, image.tags[key]);
                                    break;
                                case "artist":
                                    Array.prototype.push.apply(tags_artist_array, image.tags[key]);
                                    break;
                                case "character":
                                    Array.prototype.push.apply(tags_character_array, image.tags[key]);
                                    break;
                                case "copyright":
                                    Array.prototype.push.apply(tags_copyright_array, image.tags[key]);
                                    break;
                                case "meta":
                                    Array.prototype.push.apply(tags_meta_array, image.tags[key]);
                                    break;
                            }
                        });

                        img.tags = tags_array.sort().join(" ");
                        img.tags_general = tags_general_array.sort().join(" ");
                        img.tags_artist = tags_artist_array.sort().join(" ");
                        img.tags_character = tags_character_array.sort().join(" ");
                        img.tags_copyright = tags_copyright_array.sort().join(" ");
                        img.tags_meta = tags_meta_array.sort().join(" ");

                        if (!image.file.md5 || image.file.md5.length === 0) {
                            continue;
                        }

                        images.push(completeImage(img));
                    }

                    return { images };
                },
            },
            tags: {
                url: (query: ITagsQuery, opts: IUrlOptions): string => {
                    return "/tags.json?limit=" + opts.limit + "&page=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    const map = {
                        "id": "id",
                        "name": "name",
                        "count": "post_count",
                        "typeId": "category",
                        "related": "related_tags",
                    };

                    const data = JSON.parse(src);

                    const tags: ITag[] = [];
                    for (const tag of data) {
                        const ret = Grabber.mapFields(tag, map);
                        if (ret.related) {
                            ret.related = ret.related.split(" ").filter((_: string, i: number) => i % 2 === 0);
                        }
                        tags.push(ret);
                    }

                    return { tags };
                },
            },
        },
        html: {
            name: "Regex",
            auth: [],
            maxLimit: 200,
            search: {
                parseErrors: true,
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string | IError => {
                    try {
                        const pagePart = Grabber.pageUrl(query.page, previous, 1000, "{page}", "a{max}", "b{min}");
                        return "/posts?limit=" + opts.limit + "&page=" + pagePart + "&tags=" + encodeURIComponent(query.search);
                    } catch (e) {
                        return { error: e.message };
                    }
                },
                parse: (src: string, statusCode: number): IParsedSearch | IError => {
                    const match = src.match(/<div id="page">\s*<p>([^<]+)<\/p>\s*<\/div>/m);
                    if (match) {
                        return { error: match[1] };
                    }

                    // Broken due to -status:deleted added to search query
                    //let wiki = Grabber.regexToConst("wiki", '<div id="excerpt"(?:[^>]+)>(?<wiki>.+?)</div>', src);
                    //wiki = wiki ? wiki.replace(/href="\/wiki_pages\/show_or_new\?title=([^"]+)"/g, 'href="$1"') : wiki;
                    const wiki: string = "<p>This feature is now broken due to '-status:deleted' is added to search tag.</p>";
                    return {
                        tags: Grabber.regexToTags('<li class="category-(?<typeId>[^"]+)">(?:\\s*<a class="wiki-link" rel="nofollow" href="[^"]+">\\?</a>)?\\s*<a rel="nofollow" class="search-tag"\\s+[^>]*href="[^"]+"[^>]*>(?<name>[^<]+)</a>\\s*<span class="post-count">(?<count>[^<]+)</span>\\s*</li>', src),
                        images : Grabber.regexToImages(' '),
                        //Not enough infomation in HTML
                        //images: Grabber.regexToImages('<article[^>]* id="[^"]*" class="[^"]*"\\s+data-id="(?<id>[^"]*)"\\s+data-has-sound="[^"]*"\\s+data-tags="(?<tags>[^"]*)"\\s+data-rating="(?<rating>[^"]*)"\\s+data-flags="(?<flags>[^"]*)"\\s+data-uploader-id="[^"]*"(?:\\s+data-uploader="(?<author>[^"]*)")?\\s+data-file-url="(?<file_url>[^"]*)"\\s+data-large-file-url="(?<sample_url>[^"]*)"\\s+data-preview-file-url="(?<preview_url>[^"]*)"', src).map(completeImage),
                        wiki,
                        pageCount: Grabber.regexToConst("page", '>(?<page>\\d+)</(?:a|span)></li><li[^<]*><(?:a|span)[^>]*>(?:&gt;&gt;|<i class="[^"]+"></i>)<', src),
                    };
                },
            },
            details: {
                url: (id: string, md5: string): string => {
                    return "/posts/" + id;
                },
                parse: (src: string): IParsedDetails => {
                    return {
                        /* pool regex
                        <div id="pool-nav">[^<]*<ul>[^<]*<li id="nav-link-for-pool-\d+" class="pool-\w+-\w+ pool-\w+-\w+">[^<]*(?:<a class="first" title="to page 1" href=".*?">.*?<\/a>|<span class="first">.*?<\/span>)[^<]*(?:<a rel="prev" class="prev" title="to page \d+" href="\/posts\/(?<previous>\d+)\?pool_id=\d+">.*?<\/a>|<span class="prev">.*?<\/span>)?[^<]*<span class="pool-name">[^<]*<a title="page \d+\/\d+" href="\/pools\/(?<id>\d+)">Pool: (?<name>[^<]+)<\/a>[^<]*<\/span>[^<]*(?:<a rel="next" class="next" title="to page \d+" href="\/posts\/(?<next>\d+)\?pool_id=\d+">.*?<\/a>|<span class="next">.*?<\/span>)?[^<]*(?:<a class="last" title="to page \d+" href=".*?">.*?<\/a>|<span class="last">.*?<\/span>)[^<]*<\/li>[^<]*<\/ul>[^<]*<\/div>
                        */
                        pools: Grabber.regexToPools('<div id="pool-nav">[^<]*<ul>[^<]*<li id="nav-link-for-pool-\\d+" class="pool-\\w+-\\w+ pool-\\w+-\\w+">[^<]*(?:<a class="first" title="to page 1" href=".*?">.*?</a>|<span class="first">.*?</span>)[^<]*(?:<a rel="prev" class="prev" title="to page \\d+" href="/posts/(?<previous>\\d+)\\?pool_id=\\d+">.*?</a>|<span class="prev">.*?</span>)?[^<]*<span class="pool-name">[^<]*<a title="page \\d+/\\d+" href="/pools/(?<id>\\d+)">Pool: (?<name>[^<]+)</a>[^<]*</span>[^<]*(?:<a rel="next" class="next" title="to page \\d+" href="/posts/(?<next>\\d+)\\?pool_id=\\d+">.*?</a>|<span class="next">.*?</span>)?[^<]*(?:<a class="last" title="to page \\d+" href=".*?">.*?</a>|<span class="last">.*?</span>)[^<]*</li>[^<]*</ul>[^<]*</div>', src),
                        tags: Grabber.regexToTags('<li class="category-(?<typeId>[^"]+)">(?:\\s*<a class="wiki-link" rel="nofollow" href="[^"]+">\\?</a>)?\\s*<a rel="nofollow" class="search-tag"\\s+[^>]*href="[^"]+"[^>]*>(?<name>[^<]+)</a>\\s*<span class="post-count">(?<count>[^<]+)</span>\\s*</li>', src),
                        imageUrl: Grabber.regexToConst("url", 'Size: <a href="(?<url>[^"]+?)(?:\\?download=1[^"]*)?"', src),
                    };
                },
            },
            tagTypes: {
                url: (): string => {
                    return "/tags";
                },
                parse: (src: string): IParsedTagTypes => {
                    const contents = src.match(/<select[^>]* name="search\[category\]"[^>]*>([\s\S]+)<\/select>/)[1];
                    const results = Grabber.regexMatches('<option value="(?<id>\\d+)">(?<name>[^<]+)</option>', contents);
                    const types = results.map((r: any) => ({
                        id: r.id,
                        name: r.name.toLowerCase(),
                    }));
                    return { types };
                },
            },
            tags: {
                url: (query: ITagsQuery, opts: IUrlOptions): string => {
                    return "/tags?limit=" + opts.limit + "&page=" + query.page;
                },
                parse: (src: string): IParsedTags => {
                    return {
                        tags: Grabber.regexToTags('<tr[^>]*>\\s*<td[^>]*>(?<count>\\d+)</td>\\s*<td class="category-(?<typeId>\\d+)">\\s*<a[^>]+>\\?</a>\\s*<a[^>]+>(?<name>.+?)</a>\\s*</td>\\s*<td[^>]*>\\s*(?:<a href="/tags/(?<id>\\d+)/[^"]+">)?', src),
                    };
                },
            },
            check: {
                url: (): string => {
                    return "/";
                },
                parse: (src: string): boolean => {
                    return src.indexOf("Running e621") !== -1;
                },
            },
        },
    },
};

const map = {
    "id": "id",
    "author_id": "user",
    "name": "title",
    "file_url": "file.path",
    "created_at": "added",
};
function parseJsonImage(data: any): IImage {
    const img: IImage = Grabber.mapFields(data, map);
    img.identity = {
        service: data["service"],
        user: data["user"],
        id: data["id"],
    }
    if (data["attachments"].length > 1) {
        img.type = "gallery";
        img.gallery_count = data["attachments"].length;
    }
    return img;
}

function completeImage(img: IImage): IImage {
    if (img.file_url) {
        if (img.file_url.indexOf('/data/') === -1 && img.file_url.substr(0, 4) !== 'http' && img.file_url[0] === '/') {
            img.file_url = '/data' + img.file_url;
        }
        img.preview_url = img.file_url.replace('/data/', '/thumbnail/data/');
    }
    return img;
}

export const source: ISource = {
    name: "Kemono",
    auth: {
        session: {
            type: "post",
            url: "/account/login",
            fields: [
                {
                    id: "pseudo",
                    key: "username",
                },
                {
                    id: "password",
                    key: "password",
                    type: "password",
                },
            ],
            check: {
                type: "redirect",
                url: "logged_in=yes",
            },
        },
    },
    apis: {
        json: {
            name: "JSON",
            auth: [],
            maxLimit: 50,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions): string | IError => {
                    const offset = (query.page - 1) * opts.limit;
                    if (query.search) {
                        return {error: "The JSON API does not support arbitrary search."};
                    }
                    return "/api/recent?limit=" + opts.limit + "&o=" + offset; // + "&q=" + encodeURIComponent(query.search);
                },
                parse: (src: string): IParsedSearch | IError => {
                    const data = JSON.parse(src);
                    const images: IImage[] = data.map((img: any) => completeImage(parseJsonImage(img)));
                    return { images };
                },
            },
            gallery: {
                url: (query: IGalleryQuery): string => {
                    const identity = query.identity!;
                    return `/api/${identity["service"]}/user/${identity["user"]}/post/${identity["id"]}`;
                },
                parse: (src: string): IParsedGallery => {
                    const data = JSON.parse(src)[0];
                    const image = parseJsonImage(data);

                    // Duplicate the root data for each attachment
                    const images: IImage[] = data["attachments"].map((attachment: any) => completeImage({
                        ...image,
                        file_url: attachment["path"],
                        type: "image",
                        gallery_count: undefined,
                    }));

                    return {
                        images,
                        imageCount: data["attachments"].length,
                        pageCount: 1,
                    };
                },
            },
            /*details: {
                fullResults: true,
                url: (id: string, md5: string, opts: IUrlDetailsOptions, identity?: IImageIdentity): string | IError => {
                    return `/api/${identity!["service"]}/user/${identity!["user"]}/post/${identity!["id"]}`;
                },
                parse: (src: string): IImage => {
                    const data = JSON.parse(src)[0];
                    return completeImage(parseJsonImage(data));
                },
            },*/
            check: {
                url: (): string => {
                    return "/";
                },
                parse: (src: string): boolean => {
                    return src.indexOf("https://github.com/OpenYiff") !== -1;
                },
            },
        },
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 50,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions): string | IError => {
                    const offset = (query.page - 1) * opts.limit;
                    return "/posts?o=" + offset + "&q=" + encodeURIComponent(query.search);
                },
                parse: (src: string): IParsedSearch | IError => {
                    const html = Grabber.parseHTML(src);
                    const articles = html.find("article.post-card");

                    const images: IImage[] = [];
                    for (const article of articles) {
                        // Basic attributes
                        const identity = {
                            service: article.attr("data-service"),
                            user: article.attr("data-user"),
                            id: article.attr("data-id"),
                        };
                        const image: IImage = {
                            identity,
                            id: identity["id"],
                            author_id: identity["user"],
                            name: article.find("header")[0].innerText().trim(),
                            created_at: article.find("time")[0].attr("datetime"),
                        };

                        // Not all posts have an image
                        const img = article.find("img");
                        if (img.length > 0) {
                            image.preview_url = img[0].attr("src");
                        }

                        // Detect galleries with multiple files
                        const attachmentCount = parseInt(Grabber.regexToConst("count", "(?<count>\\d+) attachments?", article.innerHTML()), 10)
                        if (attachmentCount > 1) {
                            image.type = "gallery";
                            image.gallery_count = attachmentCount;
                        }

                        images.push(image);
                    }

                    return {
                        images,
                        imageCount: Grabber.regexToConst("count", "Showing \\d+ - \\d+ of (?<count>\\d+)", src),
                    };
                },
            },
        },
    },
};

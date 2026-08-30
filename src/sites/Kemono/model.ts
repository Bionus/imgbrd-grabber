const map = {
    "id": "id",
    "author_id": "user",
    "name": "title",
    "file_url": "file.path",
    "created_at": "added",
};

const apiHeaders = {
    "Accept": "text/css",
};
const apiPageSize = 50;
const userRegex = /(?:^|\s)user:([a-z0-9_-]+):(\d+)(?=\s|$)/i;

function apiRequest(url: string): IRequest {
    return { url, headers: apiHeaders };
}

function postFiles(data: any): any[] {
    const files: any[] = [];
    const paths: Record<string, boolean> = {};
    const add = (file: any): void => {
        const path = file && file["path"];
        if (path && !paths[path]) {
            paths[path] = true;
            files.push(file);
        }
    };

    add(data && data["file"]);
    for (const attachment of (data && data["attachments"]) || []) {
        add(attachment);
    }
    return files;
}

function parseJsonImage(data: any): IImage {
    const img: IImage = Grabber.mapFields(data, map);
    img.identity = {
        service: data["service"],
        user: data["user"],
        id: data["id"],
    };
    img.page_url = `/${data["service"]}/user/${data["user"]}/post/${data["id"]}`;
    img.created_at = data["published"] || data["added"];

    const files = postFiles(data);
    if (files.length > 1) {
        img.type = "gallery";
        img.gallery_count = files.length;
    }
    return img;
}

function dataUrl(url: string): string {
    if (url.indexOf('/data/') === -1 && url.substr(0, 4) !== 'http' && url[0] === '/') {
        return '/data' + url;
    }
    return url;
}

function completeImage(img: IImage): IImage {
    if (img.file_url) {
        img.file_url = dataUrl(img.file_url);
        img.preview_url = img.preview_url
            ? dataUrl(img.preview_url)
            : img.file_url.replace('/data/', '/thumbnail/data/');
    }
    return img;
}

export const source: ISource = {
    name: "Kemono",
    modifiers: ["user:"],
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
            forcedLimit: apiPageSize,
            search: {
                url: (query: ISearchQuery): IRequest => {
                    const offset = (query.page - 1) * apiPageSize;
                    const user = query.search.match(userRegex);
                    const search = query.search.replace(userRegex, " ").trim();
                    if (user) {
                        return apiRequest(`/api/v1/${user[1]}/user/${user[2]}/posts?o=${offset}&q=${encodeURIComponent(search)}`);
                    }
                    return apiRequest(`/api/v1/posts?o=${offset}&q=${encodeURIComponent(search)}`);
                },
                parse: (src: string): IParsedSearch | IError => {
                    const data = JSON.parse(src);
                    if (data && data["error"]) {
                        return { error: data["error"] };
                    }

                    const posts = Array.isArray(data) ? data : data && data["posts"];
                    if (!Array.isArray(posts)) {
                        return { error: "Invalid Kemono API response (no posts found)" };
                    }

                    const images: IImage[] = posts.map((img: any) => completeImage(parseJsonImage(img)));
                    return { images };
                },
            },
            gallery: {
                url: (query: IGalleryQuery): IRequest => {
                    const identity = query.identity!;
                    return apiRequest(`/api/v1/${identity["service"]}/user/${identity["user"]}/post/${identity["id"]}`);
                },
                parse: (src: string): IParsedGallery | IError => {
                    const data = JSON.parse(src);
                    if (data && data["error"]) {
                        return { error: data["error"] };
                    }

                    const post = data && (data["post"] || (Array.isArray(data) ? data[0] : data));
                    if (!post) {
                        return { error: "Invalid Kemono API response (no post found)" };
                    }

                    // The main file and post attachments are separate in API v1.3.
                    // The top-level `attachments` array instead describes files
                    // which have generated previews, so it must not replace the
                    // post's own attachments.
                    const image = parseJsonImage(post);
                    const files = postFiles(post);
                    const previewFiles = data["attachments"] || [];
                    const previews = data["previews"] || [];

                    const images: IImage[] = files.map((file: any) => {
                        const previewIndex = previewFiles.findIndex((previewFile: any) =>
                            previewFile["path"] === file["path"] || previewFile["name"] === file["name"]);
                        return completeImage({
                            ...image,
                            file_url: file["path"],
                            preview_url: previewIndex >= 0 && previews[previewIndex] ? previews[previewIndex]["path"] : undefined,
                            name: file["name"] || image.name,
                            type: "image",
                            gallery_count: undefined,
                        });
                    });

                    return {
                        images,
                        imageCount: images.length,
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
                url: (): IRequest => {
                    return apiRequest("/");
                },
                parse: (src: string): boolean => {
                    return /<title>\s*Kemono\s*<\/title>/i.test(src);
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

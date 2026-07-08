// Kemono requires this Accept header on API calls, otherwise some
// anti-scraping rules reject the request.
const kemonoHeaders = {
    "Accept": "text/css",
};

const map = {
    "id": "id",
    "author_id": "user",
    "name": "title",
    "created_at": "published",
    "tags": "tags",
};

// A post has a main "file" plus a list of "attachments", any of which can be
// missing or overlap. Returns the de-duplicated list of all files of a post.
function listFiles(data: any): any[] {
    const files: any[] = [];
    const seen: { [path: string]: boolean } = {};
    const add = (file: any) => {
        if (file && file["path"] && !seen[file["path"]]) {
            seen[file["path"]] = true;
            files.push(file);
        }
    };
    add(data["file"]);
    const attachments = data["attachments"] || [];
    for (const attachment of attachments) {
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
    const files = listFiles(data);
    if (files.length > 0) {
        img.file_url = files[0]["path"];
    }
    if (files.length > 1) {
        img.type = "gallery";
        img.gallery_count = files.length;
    }
    return img;
}

function completeImage(img: IImage): IImage {
    if (img.file_url) {
        if (img.file_url.indexOf("/data/") === -1 && img.file_url.substr(0, 4) !== "http" && img.file_url[0] === "/") {
            img.file_url = "/data" + img.file_url;
        }
        img.preview_url = img.file_url.replace("/data/", "/thumbnail/data/");
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
            forcedLimit: 50,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions): IRequest | IError => {
                    const offset = (query.page - 1) * opts.limit;
                    let url = "/api/v1/posts?o=" + offset;
                    if (query.search) {
                        url += "&q=" + encodeURIComponent(query.search);
                    }
                    return { url, headers: kemonoHeaders };
                },
                parse: (src: string): IParsedSearch | IError => {
                    const data = JSON.parse(src);
                    const posts = Array.isArray(data) ? data : (data["posts"] || []);
                    const images: IImage[] = posts.map((img: any) => completeImage(parseJsonImage(img)));
                    const result: IParsedSearch = { images };
                    if (!Array.isArray(data) && data["count"]) {
                        result.imageCount = data["count"];
                    }
                    return result;
                },
            },
            gallery: {
                url: (query: IGalleryQuery): IRequest => {
                    const identity = query.identity!;
                    return {
                        url: `/api/v1/${identity["service"]}/user/${identity["user"]}/post/${identity["id"]}`,
                        headers: kemonoHeaders,
                    };
                },
                parse: (src: string): IParsedGallery | IError => {
                    const raw = JSON.parse(src);
                    const data = Array.isArray(raw) ? raw[0] : (raw["post"] || raw);
                    const image = parseJsonImage(data);
                    const files = listFiles(data);

                    // Duplicate the root data for each file of the post
                    const images: IImage[] = files.map((file: any) => completeImage({
                        ...image,
                        file_url: file["path"],
                        type: "image",
                        gallery_count: undefined,
                    }));

                    return {
                        images,
                        imageCount: images.length,
                        pageCount: 1,
                    };
                },
            },
            check: {
                url: (): IRequest => {
                    return { url: "/api/v1/posts?o=0", headers: kemonoHeaders };
                },
                parse: (src: string): boolean => {
                    return src.indexOf("\"posts\"") !== -1 || src[0] === "[";
                },
            },
        },
    },
};

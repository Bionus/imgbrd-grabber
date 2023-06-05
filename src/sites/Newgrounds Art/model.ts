const map = {
    "id": "id",
    "name": "title",
    "file_url": "file.path",
    "created_at": "created_at",
    "tags": "tags",
    // "page_url": "url",
};
const mediaMap = {
    "url": "url",
    "file_size": "filesize",
    "width": "width",
    "height": "height",
}
const ratingMap: Record<string, string> = {
    e: "general",
    t: "safe",
    m: "questionable",
    a: "explicit",
}
function parseJsonImage(data: any): IImage {
    const img: IImage = Grabber.mapFields(data, map);
    img.rating = ratingMap[data["suitability"]];

    const user = data["users"].filter((u: any) => u["primary"]);
    img.author = user["user_name"];
    img.author_id = user["id"];

    img.medias = data["icons"]
        .concat(data["files"])
        .map((media: any) => Grabber.mapFields(media, mediaMap));
    console.log("img.medias", img.medias);

    return img;
}

const meta: ISource["meta"] = {
    order: {
        type: "options",
        options: ["date", "score",  "views"],
        default: "date",
    },
    approved: {
        type: "bool",
    },
};

export const source: ISource = {
    name: "Newgrounds Art",
    meta,
    apis: {
        json: {
            name: "JSON",
            auth: [],
            forcedLimit: 28,
            search: {
                url: (query: ISearchQuery, opts: IUrlOptions): IRequest | IError => {
                    const parsed = Grabber.parseSearchQuery(query.search, meta);
                    if (parsed.query) {
                        return {error: "Cannot search in JSON mode."};
                    }

                    const offset = (query.page - 1) * opts.limit;
                    const artistType = parsed.approved === true ? "approved" : (parsed.approved === false ? "undiscovered" : "all");
                    return {
                        url: "/browse?interval=all&sort=" + parsed.order + "&genre=0&artist-type=" + artistType + "&offset=" + offset,
                        headers: {
                            "X-Requested-With": "XMLHttpRequest",
                        },
                    };
                },
                parse: (src: string): IParsedSearch => {
                    console.log(src, "src");
                    const data = JSON.parse(src);
                    return {
                        images: data["total"]["hits"].map((img: any) => parseJsonImage(img["_source"])),
                        imageCount: data["total"]["total"]["relation"] === "eq" ? data["total"]["total"]["value"] : undefined,
                    };
                },
            },
            check: {
                url: (): string => {
                    return "/";
                },
                parse: (src: string): boolean => {
                    return src.indexOf("www.newgrounds.com") !== -1;
                },
            },
        },
    },
};

const map = {
    "identity.key": "key",
    "width": "attributes.width",
    "height": "attributes.height",
    "related": "related_tags",
};

const buildImage = (data: any): IImage => {
    const img: IImage = Grabber.mapFields(data, map);
    img.tags =  Object.keys(data["tags"]);

    img.medias = [];
    for (const key in data["transforms"]) {
        const match = key.match(/width=(\d+):/);
        if (match) {
            img.medias.push({
                url: "/api/legacy/data/" + data["transforms"][key],
                width: parseInt(match[1], 10),
            });
        }
    }

    return img;
};

export const source: ISource = {
    name: "Booru.io",
    modifiers: [],
    tagFormat: {
        case: "lower",
        wordSeparator: "_",
    },
    searchFormat: {
        and: " ",
    },
    apis: {
        json: {
            name: "JSON",
            auth: [],
            forcedLimit: 50,
            search: {
                url: (query: ISearchQuery): string | IError => {
                    const cursor = (query.page - 1) * 50;
                    return "/api/legacy/query/entity?cursor=" + cursor + "&query=" + encodeURIComponent(query.search);
                },
                parse: (src: string): IParsedSearch => {
                    const raw = JSON.parse(src);
                    const images = raw.data.map(buildImage);
                    return { images };
                },
            },
            details: {
                fullResults: true,
                url: (id: string, md5: string, opts: IUrlDetailsOptions, identity?: IImageIdentity): string | IError => {
                    return "/api/legacy/entity/" + identity!["key"];
                },
                parse: (src: string): IImage => {
                    const raw = JSON.parse(src);
                    return buildImage(raw);
                },
            },
        },
    },
};

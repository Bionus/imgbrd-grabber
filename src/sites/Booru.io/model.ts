const map = {
    "id": "key",
    "width": "attributes.width",
    "height": "attributes.height",
    "related": "related_tags",
};

const buildImage = (data: any): IImage => {
    const img: IImage = Grabber.mapFields(data, map);
    img.tags =  Object.keys(data["tags"]);

    const versions = [];
    for (const key in data["transforms"]) {
        const match = key.match(/width=(\d+):/);
        if (match) {
            versions.push({
                size: parseInt(match[1], 10),
                url: "/api/legacy/data/" + data["transforms"][key],
            });
        }
    }
    Grabber.setImageLinks(img, versions);

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
                url: (id: string): string | IError => {
                    return "/api/legacy/entity/" + id;
                },
                parse: (src: string): IImage => {
                    const raw = JSON.parse(src);
                    return buildImage(raw);
                },
            },
        },
    },
};

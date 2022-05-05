function addHelper(name: string, value: any): void {
    Object.defineProperty(Grabber, name, { value });
}

addHelper("makeArray", (val: any, allowFalsy: boolean = false): any[] => {
    if (!val && !allowFalsy) {
        return [];
    }
    if (!Array.isArray(val)) {
        return [ val ];
    }
    return val;
});

addHelper("regexMatch", (regexp: string, src: string): any => {
    const matches = Grabber.regexMatches(regexp, src);
    if (matches && matches.length > 0) {
        return matches[0];
    }
    return undefined;
});

addHelper("mapObject", (obj: any, fn: (v: any) => any): any => {
    const ret: any = {};
    for (const k in obj) {
        ret[k] = fn(obj[k]);
    }
    return ret;
});

addHelper("typedXML", (val: any) => {
    if (val && typeof val === "object" && ("#text" in val || "@attributes" in val)) {
        const txt = val["#text"];

        const isNil = "@attributes" in val && "nil" in val["@attributes"] && val["@attributes"]["nil"] === "true";
        if (isNil) {
            return null;
        }

        const type = "@attributes" in val && "type" in val["@attributes"] ? val["@attributes"]["type"] : undefined;
        if (type === "integer") {
            return parseInt(txt, 10);
        } else if (type === "array") {
            delete val["@attributes"]["type"];
            if (Object.keys(val["@attributes"]).length === 0) {
                delete val["@attributes"];
            }
            return Grabber.mapObject(val, Grabber.typedXML);
        }

        if (txt !== undefined) {
            return txt;
        }
    }

    if (val && val instanceof Array) {
        return val.map(Grabber.typedXML);
    }

    if (val && typeof val === "object") {
        if (Object.keys(val).length === 0) {
            return "";
        }

        return Grabber.mapObject(val, Grabber.typedXML);
    }

    return val;
});

addHelper("mapFields", (data: any, map: { [key: string]: string }): any => {
    const result: any = {};
    if (typeof data !== "object") {
        return result;
    }
    for (const to in map) {
        const from = map[to].split(".");
        let val: any = data;
        for (const part of from) {
            val = val && part in val && val[part] !== null ? val[part] : undefined;
        }
        result[to] = val !== data ? val : undefined;
    }
    return result;
});

addHelper("countToInt", (str: string): number | undefined => {
    if (!str) {
        return undefined;
    }
    let count: number;
    const normalized = str.toLowerCase().trim().replace(/,/g, "");
    if (normalized.slice(-1) === "k") {
        const withoutK = normalized.substring(0, normalized.length - 1).trim();
        count = parseFloat(withoutK) * 1000;
    } else {
        count = parseFloat(normalized);
    }
    return Math.round(count);
});

addHelper("fileSizeToInt", (str: string): number => {
    if  (typeof str !== "string") {
        return str as any;
    }
    const res = str.match(/^(\d+(?:\.\d+)?)\s*(\w+)$/);
    if (res) {
        const val = parseFloat(res[1]);
        const unit = res[2].toLowerCase();
        if (unit === "mb") {
            return Math.round(val * 1024 * 1024);
        }
        if (unit === "kb") {
            return Math.round(val * 1024);
        }
        return Math.round(val);
    }
    return parseInt(str, 10);
});

addHelper("fixPageUrl", (url: string, page: number, previous: IPreviousSearch | undefined, pageTransformer?: (page: number) => number): string => {
    if (!pageTransformer) {
        pageTransformer = (p: number) => p;
    }
    url = url.replace("{page}", String(pageTransformer(page)));
    if (previous) {
        url = url.replace("{min}", previous.minId);
        url = url.replace("{max}", previous.maxId);
        url = url.replace("{min-1}", previous.minIdM1);
        url = url.replace("{max+1}", previous.maxIdP1);
    }
    return url;
});

addHelper("pageUrl", (page: number, previous: IPreviousSearch | undefined, limit: number, ifBelow: string, ifPrev: string, ifNext: string, pageTransformer?: (page: number) => number): string => {
    const pageLimit = pageTransformer ? pageTransformer(page) : page;
    if (pageLimit <= limit || limit < 0) {
        return Grabber.fixPageUrl(ifBelow, page, previous, pageTransformer);
    }
    if (previous && previous.page === page + 1) {
        return Grabber.fixPageUrl(ifPrev, page, previous, pageTransformer);
    }
    if (previous && previous.page === page - 1) {
        return Grabber.fixPageUrl(ifNext, page, previous, pageTransformer);
    }
    throw new Error("You need valid previous page information to browse that far");
});

addHelper("regexToImages", (regexp: string, src: string): IImage[] => {
    const images: IImage[] = [];
    const matches = Grabber.regexMatches(regexp, src);
    for (const match of matches) {
        if ("json" in match) {
            const json = JSON.parse(match["json"]);
            for (const key in json) {
                match[key] = json[key];
            }
        }
        if (match.id) {
            match.id = parseInt(match.id, 10);
        }
        if (match.file_size) {
            match.file_size = Grabber.fileSizeToInt(match.file_size);
        }
        images.push(match);
    }
    return images;
});

addHelper("pick", (obj: any, keys: string[]): any => {
    return keys.reduce((ret, key) => {
        if (key in obj && obj[key] !== undefined) {
            ret[key] = obj[key];
        }
        return ret;
    }, {} as any);
});

addHelper("regexToTags", (regexp: string, src: string): ITag[] => {
    const tags: ITag[] = [];
    const uniques: { [key: string]: boolean } = {};

    const matches = Grabber.regexMatches(regexp, src);
    for (const match of matches) {
        if (match["name"] in uniques) {
            continue;
        }
        if ("count" in match) {
            match["count"] = Grabber.countToInt(match["count"]);
        }
        tags.push(Grabber.pick(match, ["id", "name", "count", "type", "typeId"]));
        uniques[match["name"]] = true;
    }
    return tags;
});

addHelper("regexToPools", (regexp: string, src: string): IPool[] => {
    const pools: IPool[] = [];
    const matches = Grabber.regexMatches(regexp, src);
    for (const match of matches) {
        pools.push(match);
    }
    return pools;
});

addHelper("regexToConst", (key: string, regexp: string, src: string): string | undefined => {
    const matches = Grabber.regexMatches(regexp, src);
    for (const match of matches) {
        return match[key];
    }
    return undefined;
});

function _visitSearch(search: IParsedSearchQuery, tag: (tag: ITag) => string, and: (left: string, right: string) => string, or: (left: string, right: string) => string): string {
    if ("operator" in search) {
        const left = _visitSearch(search.left, tag, and, or);
        const right = _visitSearch(search.right, tag, and, or);
        const func = search.operator === "and" ? and : or;
        return func(left, right);
    } else {
        return tag(search);
    }
}
addHelper("visitSearch", _visitSearch);

addHelper("buildQueryParams", (params: Record<string, string | number | boolean>): string => {
    const ret = [];
    for (const key in params) {
        ret.push(encodeURIComponent(key) + "=" + encodeURIComponent(params[key]));
    }
    return ret.join("&");
});

function parseSearchVal(value: string, meta: MetaField): any | null {
    if (meta.type === "options") {
        for (const opt of meta.options) {
            const optVal = typeof opt === "string" ? opt : opt.value;
            if (optVal === value) {
                return value;
            }
        }
    } else if (meta.type === "input") {
        return meta.parser ? meta.parser(value) : value;
    }
    return null;
}
addHelper("parseSearchQuery", (query: string, metas: Record<string, MetaField>): Record<string, any> => {
    const tags = [];
    const ret: Record<string, any> = {};

    // Default values
    for (const key in metas) {
        if (metas[key].default) {
            ret[key] = metas[key].default;
        }
    }

    // Parse search
    for (const part of query.split(" ")) {
        const index = part.indexOf(":");
        if (index !== -1) {
            const key = part.substring(0, index);
            if (key in metas) {
                const val = parseSearchVal(part.substring(index + 1), metas[key]);
                if (val !== null) {
                    ret[key] = val;
                    continue;
                }
            }
        }
        tags.push(part);
    }

    ret.query = tags.join(" ");
    return ret;
});

addHelper("setImageLinks", (image: IImage, versions: { size: number, url: string }[]): void => {
    let previewWidth = 0;
    let sampleWidth = 0;
    let fullWidth = 0;

    for (const version of versions) {
        const size = version.size;

        // preview_url gets the biggest size between 150 and 300
        if (!previewWidth || (size <= 300 && size > previewWidth) || (size >= 150 && previewWidth > 300)) {
            image.preview_url = version.url;
            previewWidth = size;
        }

        // sample_url is optional and takes the biggest size between 500 and 1500
        if (size >= 500 && size <= 1500 && size > sampleWidth) {
            image.sample_url = version.url;
            sampleWidth = size;
        }

        // file_url just takes the biggest size available
        if (size > fullWidth) {
            image.file_url = version.url;
            fullWidth = size;
        }
    }
})

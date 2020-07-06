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
            val = part in val && val[part] !== null ? val[part] : undefined;
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
    const res = str.match(/^(\d+)\s*(\w+)$/);
    if (res) {
        const val = parseInt(res[1], 10);
        const unit = res[2].toLowerCase();
        if (unit === "mb") {
            return val * 1024 * 1024;
        }
        if (unit === "kb") {
            return val * 1024;
        }
        return val;
    }
    return parseInt(str, 10);
});

addHelper("fixPageUrl", (url: string, page: number, previous: IPreviousSearch | undefined): string => {
    url = url.replace("{page}", String(page));
    if (previous) {
        url = url.replace("{min}", previous.minId);
        url = url.replace("{max}", previous.maxId);
        url = url.replace("{min-1}", previous.minIdM1);
        url = url.replace("{max+1}", previous.maxIdP1);
    }
    return url;
});

addHelper("pageUrl", (page: number, previous: IPreviousSearch | undefined, limit: number, ifBelow: string, ifPrev: string, ifNext: string): string => {
    if (page <= limit || limit < 0) {
        return Grabber.fixPageUrl(ifBelow, page, previous);
    }
    if (previous && previous.page === page + 1) {
        return Grabber.fixPageUrl(ifPrev, page, previous);
    }
    if (previous && previous.page === page - 1) {
        return Grabber.fixPageUrl(ifNext, page, previous);
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
        images.push(match);
    }
    return images;
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
        tags.push(match);
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

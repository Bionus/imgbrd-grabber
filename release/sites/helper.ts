Grabber.mapFields = (data: any, map: any): any => {
    const result: any = {};
    if (typeof data !== "object") {
        return result;
    }
    for (const to in map) {
        const from = map[to];
        let val = from in data ? data[from] : undefined;
        if (val && typeof val === "object" && ("#text" in val || "@attributes" in val)) {
            val = val["#text"];
        }
        result[to] = val;
    }
    return result;
};

Grabber.countToInt = (str: string): number => {
    let count: number;
    const normalized = str.toLowerCase().trim().replace(",", "");
    if (normalized.slice(-1) === "k") {
        const withoutK = normalized.substring(0, normalized.length - 1).trim();
        count = parseFloat(withoutK) * 1000;
    } else {
        count = parseFloat(normalized);
    }
    return Math.floor(count);
};

Grabber.loginUrl = (fields: any, values: any): string => {
    let res = "";
    for (const field of fields) {
        res += field.key + "=" + values[field.key] + "&";
    }
    return res;
};

Grabber.fixPageUrl = (url: string, page: number, previous: any): string => {
    url = url.replace("{page}", String(page));
    if (previous) {
        url = url.replace("{min}", previous.minId);
        url = url.replace("{max}", previous.maxId);
        url = url.replace("{min-1}", String(previous.minId - 1));
        url = url.replace("{max-1}", String(previous.maxId - 1));
        url = url.replace("{min+1}", previous.minId + 1);
        url = url.replace("{max+1}", previous.maxId + 1);
    }
    return url;
};

Grabber.pageUrl = (page: number, previous: any, limit: number, ifBelow: string, ifPrev: string, ifNext: string): string => {
    if (page < limit || !previous) {
        return Grabber.fixPageUrl(ifBelow, page, previous);
    }
    if (previous.page > page) {
        return Grabber.fixPageUrl(ifPrev, page, previous);
    }
    return Grabber.fixPageUrl(ifNext, page, previous);
};

Grabber.regexToImages = (regexp: string, src: string): IImage[] => {
    const images: IImage[] = [];
    const matches = Grabber.regexMatches(regexp, src);
    for (const match of matches) {
        if ("json" in match) {
            const json = JSON.parse(match["json"]);
            for (const key in json) {
                match[key] = json[key];
            }
        }
        images.push(match);
    }
    return images;
};

Grabber.regexToTags = (regexp: string, src: string): Iterable<ITag> => {
    const tags: { [name: string]: ITag } = {};
    const matches = Grabber.regexMatches(regexp, src);
    for (const match of matches) {
        if (match["name"] in tags) {
            continue;
        }
        if ("count" in match) {
            match["count"] = Grabber.countToInt(match["count"]);
        }
        tags[match["name"]] = match;
    }
    return tags;
};

Grabber.regexToPools = (regexp: string, src: string): IPool[] => {
    const pools: IPool[] = [];
    const matches = Grabber.regexMatches(regexp, src);
    for (const match of matches) {
        pools.push(match);
    }
    return pools;
};

Grabber.regexToConst = (key: string, regexp: string, src: string): string => {
    const matches = Grabber.regexMatches(regexp, src);
    for (const match of matches) {
        return match[key];
    }
    return undefined;
};

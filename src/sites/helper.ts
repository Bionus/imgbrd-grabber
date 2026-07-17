Grabber.makeArray = (val: any, allowFalsy: boolean = false): any[] => {
    if (!val && !allowFalsy) {
        return [];
    }
    if (!Array.isArray(val)) {
        return [ val ];
    }
    return val;
}

Grabber.regexMatch = (regexp: string, src: string): any => {
    const matches = Grabber.regexMatches(regexp, src);
    if (matches && matches.length > 0) {
        return matches[0];
    }
    return undefined;
}

Grabber.mapObject = (obj: any, fn: (v: any) => any): any => {
    const ret: any = {};
    for (const k in obj) {
        ret[k] = fn(obj[k]);
    }
    return ret;
}

Grabber.typedXML = (val: any) => {
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
}

/**
 * Set a value in an object using the dot ("a.b.c") path notation.
 */
function _set(obj: any, path: string, value: any): void {
    const parts = path.split(".");
    for (let i = 0; i < parts.length - 1; ++i) {
        const part = parts[i];
        if (!(part in obj)) {
            obj[part] = {};
        }
        obj = obj[part];
    }
    obj[parts[parts.length - 1]] = value;
}

/**
 * Get a value in an object using the dot ("a.b.c") path notation.
 */
function _get(obj: any, path: string): any {
    return path.split(".").reduce((ctx, part) => ctx?.[part], obj);
}

Grabber.mapFields = (data: any, map: { [key: string]: string }): any => {
    const result: any = {};
    if (typeof data !== "object") {
        return result;
    }
    for (const to in map) {
        const from = map[to];
        const value = _get(data, from);
        if (value !== null) {
            _set(result, to, value);
        }
    }
    return result;
}

Grabber.countToInt = (str: string): number | undefined => {
    if (!str) {
        return undefined;
    }
    let count: number;
    const normalized = str.toLowerCase().trim().replace(/,/g, "");
    if (normalized.slice(-1) === "m") {
        const withoutK = normalized.substring(0, normalized.length - 1).trim();
        count = parseFloat(withoutK) * 1_000_000;
    } else if (normalized.slice(-1) === "k") {
        const withoutK = normalized.substring(0, normalized.length - 1).trim();
        count = parseFloat(withoutK) * 1_000;
    } else {
        count = parseFloat(normalized);
    }
    return Math.round(count);
}

Grabber.fileSizeToInt = (str: string | number): number => {
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
}

Grabber.fixPageUrl = (url: string, page: number, previous: IPreviousSearch | undefined, pageTransformer?: (page: number) => number): string => {
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
}

Grabber.pageUrl = (page: number, previous: IPreviousSearch | undefined, limit: number, ifBelow: string, ifPrev: string, ifNext: string, pageTransformer?: (page: number) => number): string => {
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
}

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
        if (match.id) {
            match.id = parseInt(match.id, 10);
        }
        if (match.file_size) {
            match.file_size = Grabber.fileSizeToInt(match.file_size);
        }
        images.push(match);
    }
    return images;
}

Grabber.pick = (obj: any, keys: string[]): any => {
    return keys.reduce((ret, key) => {
        if (key in obj && obj[key] !== undefined) {
            ret[key] = obj[key];
        }
        return ret;
    }, {} as any);
}

Grabber.regexToTags = (regexp: string, src: string): ITag[] => {
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
}

Grabber.regexToPools = (regexp: string, src: string): IPool[] => {
    const pools: IPool[] = [];
    const matches = Grabber.regexMatches(regexp, src);
    for (const match of matches) {
        pools.push(match);
    }
    return pools;
}

Grabber.regexToConst = (key: string, regexp: string, src: string): string | undefined => {
    const match = Grabber.regexMatch(regexp, src);
    if (match) {
        return match[key];
    }
    return undefined;
}

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
Grabber.visitSearch = _visitSearch;

Grabber.buildQueryParams = (params: Record<string, string | number | boolean>): string => {
    const ret = [];
    for (const key in params) {
        ret.push(encodeURIComponent(key) + "=" + encodeURIComponent(params[key]));
    }
    return ret.join("&");
}

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
    } else if (meta.type === "bool") {
        if (value === "true" || value === "yes" || value === "1") {
            return true;
        } else if (value === "false" || value === "no" || value === "0") {
            return false;
        }
        console.warn(`Unknown boolean value: "${value}"`);
        return Boolean(value);
    }
    return null;
}
Grabber.parseSearchQuery = (query: string, metas: Record<string, MetaField>): Record<string, any> => {
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
}

/**
 * Canonical order/sort intents people type in multi-site searches.
 * Maps freestyle tokens (order:descending, order:newest, sort:id_desc, …)
 * into the dialect each engine actually understands.
 *
 * - "danbooru": order:id_desc / order:score / …
 * - "gelbooru": sort:id:desc / sort:score:desc / …  (NOT sort:id_desc)
 * - "raw": return the resolved intent string (id_desc, score, …) without a prefix
 */
Grabber.normalizeOrderTags = (search: string, dialect: "danbooru" | "gelbooru" | "raw" = "danbooru"): string => {
    const parts = search.split(/\s+/).filter((p) => p.length > 0);
    const out: string[] = [];
    let orderToken: string | undefined;

    // freestyle value → danbooru-style order value
    const aliases: { [key: string]: string } = {
        // newest / recent
        "id_desc": "id_desc",
        "id-desc": "id_desc",
        "id:desc": "id_desc",
        "desc": "id_desc",
        "descending": "id_desc",
        "newest": "id_desc",
        "new": "id_desc",
        "recent": "id_desc",
        "date": "id_desc",
        "date_desc": "id_desc",
        "date:desc": "id_desc",
        "updated": "change_desc",
        "updated_desc": "change_desc",
        "change": "change_desc",
        "change_desc": "change_desc",
        // oldest
        "id": "id",
        "id_asc": "id",
        "id-asc": "id",
        "id:asc": "id",
        "asc": "id",
        "ascending": "id",
        "oldest": "id",
        "old": "id",
        "date_asc": "id",
        "date:asc": "id",
        // score / popular
        "score": "score",
        "score_desc": "score",
        "score:desc": "score",
        "score_asc": "score_asc",
        "score:asc": "score_asc",
        "popular": "score",
        "best": "score",
        "rank": "rank",
        // size
        "mpixels": "mpixels",
        "mpixels_asc": "mpixels_asc",
        "filesize": "filesize",
        "landscape": "landscape",
        "portrait": "portrait",
        "favcount": "favcount",
        "random": "random",
        // gelbooru-only fields mapped into closest danbooru then re-emitted
        "rating": "score",
        "user": "id_desc",
        "height": "id_desc",
        "width": "id_desc",
        "parent": "id_desc",
        "source": "id_desc",
    };

    const gelbooruMap: { [key: string]: string } = {
        "id_desc": "sort:id:desc",
        "id": "sort:id:asc",
        "score": "sort:score:desc",
        "score_asc": "sort:score:asc",
        "change_desc": "sort:updated:desc",
        "change": "sort:updated:desc",
        "mpixels": "sort:width:desc", // closest available
        "mpixels_asc": "sort:width:asc",
        "filesize": "sort:id:desc",
        "landscape": "sort:width:desc",
        "portrait": "sort:height:desc",
        "favcount": "sort:score:desc",
        "rank": "sort:score:desc",
        "random": "sort:random",
    };

    for (const part of parts) {
        const lower = part.toLowerCase();
        let val: string | undefined;

        if (lower.indexOf("order:") === 0) {
            val = part.substr(6);
        } else if (lower.indexOf("sort:") === 0) {
            val = part.substr(5);
        } else {
            out.push(part);
            continue;
        }

        // Already correct Gelbooru form sort:field:dir — keep if dialect matches
        if (lower.indexOf("sort:") === 0 && val.indexOf(":") >= 0) {
            if (dialect === "gelbooru") {
                orderToken = "sort:" + val.toLowerCase();
            } else if (dialect === "raw") {
                orderToken = val.toLowerCase().replace(":", "_");
            } else {
                // map sort:id:desc → order:id_desc
                const mapped = aliases[val.toLowerCase()] || aliases[val.toLowerCase().replace(":", "_")];
                orderToken = mapped ? "order:" + mapped : "order:" + val.toLowerCase().replace(":", "_");
            }
            continue;
        }

        const key = val.toLowerCase();
        const resolved = aliases[key] || key;

        if (dialect === "gelbooru") {
            orderToken = gelbooruMap[resolved] || ("sort:" + resolved.replace(/_/g, ":"));
            // ensure field:dir form when we only got a field
            if (orderToken.indexOf(":") === orderToken.lastIndexOf(":") && orderToken.indexOf("sort:") === 0) {
                // sort:score → sort:score:desc
                const field = orderToken.substr(5);
                if (field.indexOf(":") < 0 && field !== "random") {
                    orderToken = "sort:" + field + ":desc";
                }
            }
        } else if (dialect === "raw") {
            orderToken = resolved;
        } else {
            orderToken = "order:" + resolved;
        }
    }

    if (orderToken) {
        out.push(orderToken);
    }
    return out.join(" ");
}

/**
 * Drop booru-only meta tokens that free-text engines (Reddit/Twitter/DA) misread.
 * Keeps order/sort tokens optionally via keepOrder.
 */
Grabber.stripBooruMetaTags = (search: string, opts?: { keepOrder?: boolean }): { query: string, order?: string } => {
    const meta = /^(rating|width|height|filesize|filetype|status|parent|child|source|date|age|id|md5|pixiv_id|has|is|pool|ordfav|fav|approver|commenter|noter|noteupdater|artcomm|gen|copy|char|meta|limit|page|mpixels|gentags|arttags|chartags|copytags|unlocked|fastfav|sub):/i;
    const parts = search.split(/\s+/).filter((p) => p.length > 0);
    const tags: string[] = [];
    let order: string | undefined;
    for (const part of parts) {
        const lower = part.toLowerCase();
        if (lower.indexOf("order:") === 0 || lower.indexOf("sort:") === 0) {
            if (opts && opts.keepOrder) {
                order = part.substr(part.indexOf(":") + 1).toLowerCase();
            }
            continue;
        }
        if (meta.test(part) || (part.charAt(0) === "-" && meta.test(part.substr(1)))) {
            continue;
        }
        // natural language for free-text sources
        tags.push(part.replace(/_/g, " "));
    }
    return { query: tags.join(" ").trim(), order };
}

// Fix console calls since C++ handlers can't get variadic arguments
const originalConsole = console
function argToString(arg: any): string {
    if (typeof arg === "object") {
        return JSON.stringify(arg);
    }
    return String(arg);
}
function argsToString(args: any[]): string {
    return args.map(argToString).join(' ');
}
// @ts-ignore
console = {
    debug: (...args: any[]) => originalConsole.debug(argsToString(args)),
    error: (...args: any[]) => originalConsole.error(argsToString(args)),
    info: (...args: any[]) => originalConsole.info(argsToString(args)),
    log: (...args: any[]) => originalConsole.log(argsToString(args)),
    warn: (...args: any[]) => originalConsole.warn(argsToString(args)),
}

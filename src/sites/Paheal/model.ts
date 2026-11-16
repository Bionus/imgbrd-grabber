// Paheal-specific Shimmie layout (rule34.paheal.net).
// Keep this source independent from the generic Shimmie model: the HTML
// callbacks receive page contents, not the host used to load them.

function attribute(tag: string, name: string): string | undefined {
    const pattern = new RegExp("(?:^|\\s)" + name + "\\s*=\\s*(['\"])([\\s\\S]*?)\\1", "i");
    const match = tag.match(pattern);
    return match ? match[2] : undefined;
}

function md5FromUrl(url?: string): string | undefined {
    const match = (url || "").match(/\/([a-f\d]{32})(?:\.[a-z\d]+)?(?:[?#].*)?$/i);
    return match ? match[1] : undefined;
}

function extensionFromMime(mime?: string): string | undefined {
    const known: Record<string, string> = {
        "image/jpeg": "jpg",
        "image/png": "png",
        "image/gif": "gif",
        "image/webp": "webp",
        "image/avif": "avif",
        "video/mp4": "mp4",
        "video/webm": "webm",
    };
    return mime ? known[mime.toLowerCase()] : undefined;
}

function previewFromFile(url?: string): string | undefined {
    return url && /^https?:\/\/r34i\.paheal-cdn\.net\//i.test(url)
        ? url.replace(/^https?:\/\/r34i\.paheal-cdn\.net\//i, "https://r34t.paheal.net/")
        : undefined;
}

function fileFromPreview(url?: string): string | undefined {
    return url && /^https?:\/\/r34t\.paheal\.net\//i.test(url)
        ? url.replace(/^https?:\/\/r34t\.paheal\.net\//i, "https://r34i.paheal-cdn.net/")
        : undefined;
}

function escapeWithCaret(tag: string): string {
    // Same escaping convention as upstream Shimmie for ordinary tag names.
    const substitutions: Record<string, string> = {
        "^": "^", "/": "s", "\\": "b", "?": "q", "&": "a", ".": "d",
    };
    for (const from in substitutions) {
        tag = tag.replace(new RegExp("\\" + from, "g"), "^" + substitutions[from]);
    }
    return tag;
}

function searchTag(tag: string): string {
    const match = tag.match(/^(-?)([^:]+):(.*)$/);
    if (!match) {
        return escapeWithCaret(tag);
    }
    const names: Record<string, string> = {
        date: "posted", tagcount: "tags", downvote: "downvoted_by", upvote: "upvoted_by",
    };
    const name = names[match[2].toLowerCase()] || match[2];
    const supported = /^(score|posted|downvoted_by|upvoted_by|tags|size|width|height|order|id|hash|md5|filename|filesize|ext|mime|source|poster|comments|commented_by|comments_locked|content|ratio|length)$/i;
    if (!supported.test(name)) {
        return escapeWithCaret(tag);
    }
    const value = match[3];
    const range = value.match(/^([^.]*)\.\.([^.]*)$/);
    if (range) {
        if (!range[1]) {
            return match[1] + name + "<=" + range[2];
        }
        if (!range[2]) {
            return match[1] + name + ">=" + range[1];
        }
        return match[1] + name + ">=" + range[1] + " " + match[1] + name + "<=" + range[2];
    }
    return match[1] + name + (/^[<>]=?|^=/.test(value) ? "" : "=") + value;
}

function searchQuery(query: string): string {
    return query.split(/\s+/).filter((tag: string): boolean => tag.length > 0).map(searchTag).join(" ");
}

function parseListing(src: string): IParsedSearch {
    const images: IImage[] = [];
    // Paheal places the image's attributes in the opening thumb div, then
    // renders the preview and original CDN links inside that div.
    const cards = /<div\b(?=[^>]*\bclass=['"][^'"]*\bshm-thumb\b[^'"]*['"])[^>]*>[\s\S]*?<\/div>/gi;
    let card: RegExpExecArray | null;
    while ((card = cards.exec(src)) !== null) {
        const opening = card[0].match(/^<div\b[^>]*>/i);
        const previewTag = card[0].match(/<img\b[^>]*>/i);
        if (!opening || !previewTag) {
            continue;
        }

        const id = attribute(opening[0], "data-post-id");
        if (!id || !/^\d+$/.test(id)) {
            continue;
        }

        const fileLink = card[0].match(/<a\b[^>]*href=['"]([^'"]+)['"][^>]*>\s*File Only\s*<\/a>/i);
        const previewUrl = attribute(previewTag[0], "src") || attribute(previewTag[0], "data-original");
        const fileUrl = (fileLink && fileLink[1]) || fileFromPreview(previewUrl);
        const rawTags = attribute(opening[0], "data-tags") || "";
        const title = attribute(previewTag[0], "title") || "";
        const info = title.match(/(\d+)x(\d+)\s*\/\/\s*([^/]+?)\s*\/\/\s*([a-z\d]+)(?:\s+(\d{4}-\d{2}-\d{2}T\S+))?\s*$/i);
        const img: IImage = {
            id: parseInt(id, 10),
            page_url: "/post/view/" + id,
            preview_url: previewUrl || previewFromFile(fileUrl),
            file_url: fileUrl,
            md5: md5FromUrl(fileUrl || previewUrl),
            tags: rawTags.trim() ? rawTags.toLowerCase().trim().split(/\s+/) : [],
            ext: extensionFromMime(attribute(opening[0], "data-mime")),
        };
        if (info) {
            img.width = parseInt(info[1], 10);
            img.height = parseInt(info[2], 10);
            img.file_size = Grabber.fileSizeToInt(info[3].trim());
            img.ext = info[4].toLowerCase();
            img.created_at = info[5];
        }
        images.push(img);
    }

    const lastTag = src.match(/<link\b(?=[^>]*\brel=['"]last['"])[^>]*>/i);
    const lastHref = lastTag ? attribute(lastTag[0], "href") : undefined;
    const lastPage = lastHref && lastHref.match(/\/post\/list\/(?:[^/]+\/)?(\d+)\/?(?:\?.*)?$/);
    return {
        images,
        pageCount: lastPage ? parseInt(lastPage[1], 10) : undefined,
    };
}

function postMedia(src: string): { main?: string; media?: string } {
    const main = src.match(/<img\b(?=[^>]*\bid=['"]main_image['"])[^>]*>/i);
    const media = src.match(/<div\b(?=[^>]*\bid=['"]shm_post_media['"])[^>]*>/i);
    return { main: main ? main[0] : undefined, media: media ? media[0] : undefined };
}

function parsePostDetails(src: string): IParsedDetails {
    const tags = postMedia(src);
    const dataTags = tags.media ? attribute(tags.media, "data-tags") : undefined;
    const uploader = src.match(/<tr\b[^>]*data-row=['"]Uploader['"][^>]*>[\s\S]*?<\/tr>/i);
    const time = (uploader ? uploader[0] : "").match(/<time\b[^>]*datetime=['"]([^'"]+)['"]/i);

    return {
        tags: dataTags ? dataTags.toLowerCase().trim().split(/\s+/) : undefined,
        imageUrl: tags.main ? attribute(tags.main, "src") : undefined,
        createdAt: time ? time[1] : undefined,
    };
}

function parsePost(src: string): IParsedSearch {
    const html = postMedia(src);
    const details = parsePostDetails(src);
    const input = src.match(/<input\b(?=[^>]*\bname=['"]image_id['"])[^>]*>/i);
    const id = input ? attribute(input[0], "value") : undefined;
    const fileUrl = details.imageUrl;
    const mime = html.main ? attribute(html.main, "data-mime") : undefined;
    const img: IImage = {
        id: id && /^\d+$/.test(id) ? parseInt(id, 10) : undefined,
        page_url: id ? "/post/view/" + id : undefined,
        tags: details.tags,
        created_at: details.createdAt,
        file_url: fileUrl,
        preview_url: previewFromFile(fileUrl),
        md5: md5FromUrl(fileUrl),
        width: html.main && attribute(html.main, "data-width") ? parseInt(attribute(html.main, "data-width") as string, 10) : undefined,
        height: html.main && attribute(html.main, "data-height") ? parseInt(attribute(html.main, "data-height") as string, 10) : undefined,
        ext: extensionFromMime(mime),
    };
    const info = src.match(/<tr\b[^>]*data-row=['"]Info['"][^>]*>[\s\S]*?<td[^>]*>\s*(\d+)x(\d+)\s*\/\/\s*([^<]+?)\s*\/\/\s*([a-z\d]+)\s*<\/td>/i);
    if (info) {
        img.width = parseInt(info[1], 10);
        img.height = parseInt(info[2], 10);
        img.file_size = Grabber.fileSizeToInt(info[3].trim());
        img.ext = info[4].toLowerCase();
    }
    return { images: [img], tags: details.tags, imageCount: 1, pageCount: 1 };
}

export const source: ISource = {
    name: "Paheal",
    modifiers: ["score:", "date:", "order:score", "order:score_asc", "order:score_desc", "tagcount:"],
    forcedTokens: ["filename", "date"],
    tagFormat: { case: "lower", wordSeparator: "_" },
    searchFormat: { and: " " },
    // The HTML API is the only Paheal API verified with the supplied captures.
    // The Shimmie RSS endpoint and token-bearing login form are not verified.
    apis: {
        html: {
            name: "Regex",
            auth: [],
            search: {
                parseErrors: true,
                url: (query: ISearchQuery): string => query.search.trim()
                    ? "/post/list/" + encodeURIComponent(searchQuery(query.search)) + "/" + query.page
                    : "/post/list/" + query.page,
                parse: (src: string, statusCode: number): IParsedSearch | IError => {
                    if (statusCode === 404) {
                        return { images: [] };
                    }
                    if (statusCode < 200 || statusCode >= 300) {
                        return { error: "Loading error: HTTP " + statusCode };
                    }
                    return /<img\b(?=[^>]*\bid=['"]main_image['"])/i.test(src)
                        ? parsePost(src) : parseListing(src);
                },
            },
            details: {
                url: (id: string): string => "/post/view/" + id,
                parse: parsePostDetails,
            },
            check: {
                url: (): string => "/",
                parse: (src: string): boolean => src.indexOf("Shimmie version ") !== -1,
            },
        },
    },
};

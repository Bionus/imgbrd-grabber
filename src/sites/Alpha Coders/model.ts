function metaValue(src: string, name: string): string | undefined {
    const escapedName = name.replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
    const match = new RegExp(`<meta\\b(?=[^>]*\\bitemprop\\s*=\\s*["']${escapedName}["'])[^>]*\\b(?:content|href)\\s*=\\s*["']([^"']*)["'][^>]*>`, "i").exec(src);
    return match ? Grabber.htmlDecode(match[1]) : undefined;
}

function imageFromBlock(block: string): IImage | null {
    const fileUrl = metaValue(block, "contentUrl");
    const pageUrl = metaValue(block, "url");
    const previewUrl = metaValue(block, "thumbnailUrl");
    if (!fileUrl || !pageUrl || !previewUrl) {
        return null;
    }

    const id = pageUrl.match(/[?&]i=(\d+)/)?.[1]
        || fileUrl.match(/\/(\d+)\.[a-z0-9]+(?:\?|$)/i)?.[1];
    if (!id) {
        return null;
    }

    const dimensions = block.match(/\((\d{3,5})\s*(?:x|×|&#10005;)\s*(\d{3,5})\)/i);
    const keywords = metaValue(block, "keywords");
    const tags = keywords
        ? keywords.split(",").map((tag) => tag.trim()).filter((tag) => tag.length > 0)
        : [];

    const image: IImage = {
        id,
        page_url: pageUrl,
        file_url: fileUrl,
        sample_url: fileUrl,
        preview_url: previewUrl,
        name: metaValue(block, "name"),
        tags,
        created_at: metaValue(block, "datePublished"),
        rating: "safe",
        ext: fileUrl.match(/\.([a-z0-9]+)(?:\?|$)/i)?.[1],
    };

    if (dimensions) {
        image.width = parseInt(dimensions[1], 10);
        image.height = parseInt(dimensions[2], 10);
    }
    return image;
}

function parseImages(src: string): IImage[] {
    return src
        .split(/<div\s+id=["']content_/i)
        .slice(1)
        .map((part) => imageFromBlock(`<div id="content_${part}`))
        .filter((image): image is IImage => image !== null);
}

export const source: ISource = {
    name: "Wallpaper Abyss",
    forcedTokens: [],
    tagFormat: {
        case: "lower",
        wordSeparator: " ",
    },
    searchFormat: {
        and: " ",
    },
    apis: {
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 30,
            search: {
                url: (query: ISearchQuery): string => {
                    if (!query.search.trim()) {
                        return `/featured.php?page=${query.page}`;
                    }
                    return `/search.php?search=${encodeURIComponent(query.search.trim())}&page=${query.page}`;
                },
                parse: (src: string): IParsedSearch => {
                    const count = src.match(/\[([\d,]+)\+?\]/)?.[1];
                    return {
                        images: parseImages(src),
                        imageCount: count ? parseInt(count.replace(/,/g, ""), 10) : undefined,
                    };
                },
            },
            details: {
                url: (id: string): string => {
                    return `/big.php?i=${id}`;
                },
                parse: (src: string): IParsedDetails | IError => {
                    const imageUrl = metaValue(src, "contentUrl");
                    if (!imageUrl) {
                        return { error: "Could not find the full-size wallpaper URL" };
                    }
                    const keywords = metaValue(src, "keywords");
                    return {
                        imageUrl,
                        createdAt: metaValue(src, "datePublished"),
                        tags: keywords
                            ? keywords.split(",").map((tag) => tag.trim()).filter((tag) => tag.length > 0)
                            : [],
                    };
                },
            },
            check: {
                url: (): string => "/",
                parse: (src: string): boolean => {
                    return /<title>[^<]*(?:Wallpaper Abyss|Alpha Coders)[^<]*<\/title>/i.test(src);
                },
            },
        },
    },
};

function completeGallery(img: IImage): IImage {
    img.type = "gallery";
    img.identity = {"slug": (img as any)["slug"]};
    return completeImage(img);
}

function completeImage(img: IImage): IImage {
    if (img.file_url && !img.preview_url) {
        img.preview_url = img.file_url.replace(/\/(\d+)\.(\w{3,4})$/, "/p=160x200/$1.$2");
    }
    if (img.file_url && !img.sample_url) {
        img.sample_url = img.file_url.replace(/\/(\d+)\.(\w{3,4})$/, "/p=700/$1.$2");
    }
    if (img.preview_url && !img.file_url) {
        img.file_url = img.preview_url.replace(/\/p=\d+x\d+\/(\d+)\.(\w{3,4})$/, "/$1.$2");
    }
    return img;
}

export const source: ISource = {
    name: "Hentai Cosplays",
    modifiers: [],
    tagFormat: {
        case: "lower",
        wordSeparator: "-",
    },
    searchFormat: {
        and: " ",
    },
    apis: {
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 50,
            search: {
                url: (query: ISearchQuery): string | IError => {
                    return `/search/keyword/${encodeURIComponent(query.search)}/page/${query.page}/`;
                },
                parse: (src: string): IParsedSearch => {
                    const images: IImage[] = Grabber.regexToImages('<div class="image-list-item">\\s*<div class="image-list-item-image">\\s*<a href="/image/(?<slug>[^"]+)/">\\s*<img src="(?<preview_url>[^"]+)"[^>]*>\\s*</a>\\s*</div>\\s*<p class="image-list-item-title">\\s*<a href="[^"]+">(?<name>[^<]*)</a>\\s*</p>\\s*<p class="image-list-item-regist-date">\\s*<span>(?<created_at>[^<]+)</span>\\s*</p>\\s*</div>', src);
                    return {
                        images: images.map(completeGallery),
                        pageCount: Grabber.regexToConst("count", '<a class="last" href="[^"]+/(?<count>\\d+)/">', src),
                        imageCount: Grabber.regexToConst("count", '<span id="articles_number"><span class="immoral_all_items">(?<count>\\d+)</span> search results</span>', src),
                    };
                },
            },
            gallery: {
                url: (query: IGalleryQuery): string => {
                    return `/story/${query.identity!.slug}/`;
                },
                parse: (src: string): IParsedGallery => {
                    const images = Grabber.regexToImages('<amp-img[^>]* src="(?<file_url>[^"]+)"', src);
                    return {
                        images: images.map(completeImage),
                        pageCount: 1,
                        imageCount: images.length,
                    };
                }
            },
            details: {
                url: (): IError => {
                    return {error: "Not supported"};
                },
                parse: (src: string): IParsedDetails => {
                    const imageUrl = Grabber.regexToConst("file_url", '<div id="display_image_detail">\\s*<p>\\s*<a href="(?<file_url>[^"]+)"', src);
                    return { imageUrl };
                }
            },
        },
    },
};

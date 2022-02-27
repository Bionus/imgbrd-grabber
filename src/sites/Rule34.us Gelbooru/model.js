function completeImage(img) {
    if ((!img.file_url || img.file_url.length < 5) && img.preview_url) {
        img.file_url = img.preview_url
            .replace("/thumbnails/", "/images/")
            .replace("/thumbnail_", "/");
    }
    return img;
}
export var source = {
    name: "Modified Gelbooru (0.1)",
    modifiers: ["rating:safe", "rating:questionable", "rating:explicit", "user:", "fav:", "fastfav:", "md5:", "source:", "id:", "width:", "height:", "score:", "mpixels:", "filesize:", "date:", "gentags:", "arttags:", "chartags:", "copytags:", "approver:", "parent:", "sub:", "order:id", "order:id_desc", "order:score", "order:score_asc", "order:mpixels", "order:mpixels_asc", "order:filesize", "order:landscape", "order:portrait", "order:favcount", "order:rank", "parent:none", "unlocked:rating"],
    tagFormat: {
        case: "lower",
        wordSeparator: "_",
    },
    searchFormat: {
        and: " ",
    },
    auth: {},
    apis: {
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 20,
            search: {
                url: function (query) {
                    console.log(JSON.stringify(query))
                    var page = query.page - 1;
                    var search = query.search && query.search.length > 0 ? encodeURIComponent(query.search) : "all";
                    return "/index.php?r=posts/index&q=" + search + (page == 0 ? '' : "&page=" + page);
                },
                parse: function (src) {
                    var pageCount = Grabber.regexToConst("page", '<div class="pagination">\\W*(<a.*>.*<\\/a>\\W*)*(<a href="[^"]+page=(?<page>\\d+)".*>.*<\\/a>\\W*)<\\/div>', src);
                    return {
                        images: Grabber.regexToImages('<a id="(?<id>\\d+)" href="[^"]+"><img +src="(?<preview_url>[^"]*thumbnail_(?<md5>[^.]+)\\.[^"]+)" title=" *(?<tags>[^"]*)" .+?</a></div>', src).map(completeImage),
                        pageCount: pageCount ? (parseInt(pageCount, 10) + 1) : undefined,
                        tags: Grabber.regexToTags('<li><a[^>]*>\\+</a><a [^>]*>-</a> <span [^>]*>\\? <a href="[^"]*">(?<name>[^<]+)</a> (?<count>\\d+)</span></li>', src),
                    };
                },
            },
            details: {
                url: function (id, md5) {
                    return "/index.php?r=posts/view&id=" + id;
                },
                parse: function (src) {
                    return {
                        tags: Grabber.regexToTags('<li><a[^>]*>\\+</a><a [^>]*>-</a> <span [^>]*>\\? <a href="[^"]*">(?<name>[^<]+)</a> (?<count>\\d+)</span></li>', src),
                        imageUrl: Grabber.regexToConst("url", '<img[^>]+src="([^"]+)"[^>]+onclick="Note\\.toggle\\(\\);"[^>]*/>', src),
                    };
                },
            },
            check: {
                url: function () {
                    return "/";
                },
                parse: function (src) {
                    return src.search(/Running modified Gelbooru(?: Beta)? 0\.1/) !== -1
                        || src.search(/Running <a[^>]*>Gelbooru<\/a>(?: Beta)? 0\.1/) !== -1;
                },
            },
        },
    },
};

export const source: ISource = {
    name: "Gelbooru (0.1)",
    modifiers: ["rating:safe", "rating:questionable", "rating:explicit", "user:", "fav:", "fastfav:", "md5:", "source:", "id:", "width:", "height:", "score:", "mpixels:", "filesize:", "date:", "gentags:", "arttags:", "chartags:", "copytags:", "approver:", "parent:", "sub:", "order:id", "order:id_desc", "order:score", "order:score_asc", "order:mpixels", "order:mpixels_asc", "order:filesize", "order:landscape", "order:portrait", "order:favcount", "order:rank", "parent:none", "unlocked:rating"],
    auth: {},
    apis: {
        html: {
            name: "Regex",
            auth: [],
            maxLimit: 200,
            search: {
                url: (query: any, opts: any, previous: any): IUrl | IError | string => {
                    const pagePart = Grabber.pageUrl(query.page, previous, 476, "&pid={pid}", " id:<{min}&p=1", "&pid={pid}");
                    return "/index.php?page=post&s=list&tags=" + query.search + pagePart;
                },
                parse: (src: string): IParsedSearch => {
                    return {
                        images: Grabber.regexToImages('<span class="thumb"><a id="p(?<id>\\d+)" href="[^"]+"><img src="(?<preview_url>[^"]*thumbnail_(?<md5>[^.]+)\\.[^"]+)" alt="post" border="0" title=" *(?<tags>[^"]*) *score:(?<score>[^ "]+) *rating:(?<rating>[^ "]+) *"/></a>[^<]*(?:<script type="text/javascript">[^<]*//<!\\[CDATA\\[[^<]*posts\\[[^]]+\\] = \\{\'tags\':\'(?<tags_2>[^\']+)\'\\.split\\(/ /g\\), \'rating\':\'(?<rating_2>[^\']+)\', \'score\':(?<score_2>[^,]+), \'user\':\'(?<author>[^\']+)\'\\}[^<]*//\\]\\]>[^<]*</script>)?</span>', src),
                        tags: Grabber.regexToTags('<li><a[^>]*>\\+</a><a [^>]*>-</a> <span [^>]*>\\? <a href="[^"]*">(?<name>[^<]+)</a> (?<count>\\d+)</span></li>', src),
                    };
                },
            },
            details: {
                url: (id: number, md5: string): IUrl | IError | string => {
                    return "/index.php?page=post&s=view&id=" + id;
                },
                parse: (src: string): IParsedDetails => {
                    return {
                        tags: Grabber.regexToTags('<li><a[^>]*>\\+</a><a [^>]*>-</a> <span [^>]*>\\? <a href="[^"]*">(?<name>[^<]+)</a> (?<count>\\d+)</span></li>', src),
                        imageUrl: Grabber.regexToConst("url", '<img[^>]+src="([^"]+)"[^>]+onclick="Note\\.toggle\\(\\);"[^>]*/>', src),
                    };
                },
            },
        },
    },
};

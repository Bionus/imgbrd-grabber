function makeFile(data: any, key: string): IFile | null {
    if ((key + "Url") in data) {
        return {
            url: data[key + "Url"],
            width: data.width,
            height: data.height,
            filesize: data[key + "Size"],
        };
    }
    /*if (data.content_urls && key in data.content_urls) {
        return {
            url: data.content_urls[key].url,
            width: data.content_urls[key].width,
            height: data.content_urls[key].height,
            filesize: data.content_urls[key].size,
        };
    }*/
    return null;
}

export const downloader: IDownloader = {
    name: "Gfycat / Redgifs",
    handlers: [{
        regexes: ["(gfycat|redgifs|gifdeliverynetwork)\\.com/(?:watch/)?(\\w+)"],
        url(url: string): string | IError {
            const match = url.match(/(gfycat|redgifs|gifdeliverynetwork)\.com\/(?:watch\/)?(\w+).*$/i);
            if (!match) {
                return { error: "Could not parse URL" };
            }
            const domain = match[1] === "gfycat" ? "gfycat" : "redgifs";
            const id = match[2];
            return `https://api.${domain}.com/v1/gfycats/${id}`;
        },
        parse(src: string): IDownloadable {
            const data = JSON.parse(src).gfyItem;
            return {
                tokens: Grabber.mapFields(data, {
                    "name": "title",
                    "author": "userName",
                    "md5": "md5",
                    "source": "subreddit",
                    "created_at": "createDate",
                    "tags": "tags",
                }),
                files: [
                    makeFile(data, "mp4"),
                    /*makeFile(data, "gif"),
                    makeFile(data, "webm"),
                    makeFile(data, "max1mbGif"),
                    makeFile(data, "max2mbGif"),
                    makeFile(data, "max5mbGif"),*/
                    { url: data.posterUrl },
                ].filter(f => f !== null) as IFile[],
            };
        },
    }],
};

import { readFileSync } from "fs";
import { makeGrabber } from "../test-utils";
import { downloader } from "./downloader";

const handler = downloader.handlers[0];

describe("Gfycat downloader", () => {
    beforeAll(makeGrabber);

    describe("regexes", () => {
        const regex = new RegExp(handler.regexes[0]);

        it("handles valid domains", () => {
            expect("https://gfycat.com/bewitchedpleasedbongo-dog".match(regex)).toBeTruthy();
            expect("https://www.redgifs.com/watch/mellowimmaculatebushbaby".match(regex)).toBeTruthy();
            expect("https://www.gifdeliverynetwork.com/cornyloathsomeharrierhawk".match(regex)).toBeTruthy();
        });

        it("fails for invalid domains", () => {
            expect("https://google.com/bewitchedpleasedbongo-dog".match(regex)).toBeFalsy();
        });
    });

    describe("url", () => {
        it("builds an URL for valid domains", () => {
            expect(handler.url("https://gfycat.com/bewitchedpleasedbongo-dog")).toEqual("https://api.gfycat.com/v1/gfycats/bewitchedpleasedbongo");
            expect(handler.url("https://www.redgifs.com/watch/mellowimmaculatebushbaby")).toEqual("https://api.redgifs.com/v1/gfycats/mellowimmaculatebushbaby");
            expect(handler.url("https://www.gifdeliverynetwork.com/cornyloathsomeharrierhawk")).toEqual("https://api.redgifs.com/v1/gfycats/cornyloathsomeharrierhawk");
        });

        it("returns an error for invalid domains or URL", () => {
            expect(handler.url("https://google.com/bewitchedpleasedbongo-dog")).toEqual({ error: "Could not parse URL" });
            expect(handler.url("https://gfycat.com/")).toEqual({ error: "Could not parse URL" });
        });
    });

    it("parses valid JSON", () => {
        const src = readFileSync(__dirname + "/resources/data.json", "utf8");
        const res = handler.parse(src, 200) as IDownloadable;

        expect(res.tokens).toEqual({
            name: "getfvid 68661867 141059523769202 3986167671818281332 n",
            author: "anonymous",
            md5: "b4135d487b91c43b009d4df818531ff9",
            created_at: 1564982730,
            tags: ["dog"],
        });
        expect(res.files).toEqual([
            {
                url: "https://giant.gfycat.com/BewitchedPleasedBongo.mp4",
                width: 720,
                height: 1270,
                filesize: 3299112,
            },
            { url: "https://thumbs.gfycat.com/BewitchedPleasedBongo-poster.jpg" }
        ]);
    });
});

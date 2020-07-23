import { search, gallery, makeGrabber } from "../test-utils";
import { source } from "./model";
import { readFileSync } from "fs";

describe("Twitter", () => {
    beforeAll(makeGrabber);

    describe("JSON API", () => {
        describe("Search", () => {
            it("works for basic listing", () => {
                expect(search(source.apis.json, "Twitter")).toEqual("/1.1/statuses/user_timeline.json?include_rts=true&exclude_replies=false&tweet_mode=extended&screen_name=Twitter")
            });

            it("correctly detect flags", () => {
                expect(search(source.apis.json, "Twitter retweets:no replies:no")).toEqual("/1.1/statuses/user_timeline.json?include_rts=false&exclude_replies=true&tweet_mode=extended&screen_name=Twitter")
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/search.json", "utf8");
                const res = source.apis.json.search.parse(src, 200) as IParsedSearch;

                expect(res.images.length).toEqual(11);
                expect(res.images.map(i => i.id).slice(0, 3)).toEqual(["1274087263073255425", "1274086977952833536", "1274086862907305984"]);
            });
        });

        describe("Gallery", () => {
            it("returns the url", () => {
                expect(gallery(source.apis.json, "123")).toEqual("/1.1/statuses/show.json?id=123&tweet_mode=extended");
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/gallery.json", "utf8");
                const res = source.apis.json.gallery!.parse(src, 200) as IParsedGallery;

                expect(res.images.map(i => i.id)).toEqual(["1278159248287920129", "1278159269003554817", "1278159278763696128"]);
                expect(res.imageCount).toEqual(3);
                expect(res.pageCount).toEqual(1);
            });
        });
    });
});

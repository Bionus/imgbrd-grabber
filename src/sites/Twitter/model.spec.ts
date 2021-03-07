import { search, gallery, makeGrabber } from "../test-utils";
import { source } from "./model";
import { readFileSync } from "fs";

describe("Twitter", () => {
    beforeAll(makeGrabber);

    describe("JSON API", () => {
        describe("Search", () => {
            it("works for basic listing", () => {
                expect(search(source.apis.json, "Twitter")).toEqual("/1.1/statuses/user_timeline.json?count=10&include_rts=true&exclude_replies=false&tweet_mode=extended&screen_name=Twitter")
            });

            it("correctly detect flags", () => {
                expect(search(source.apis.json, "Twitter retweets:no replies:no")).toEqual("/1.1/statuses/user_timeline.json?count=10&include_rts=false&exclude_replies=true&tweet_mode=extended&screen_name=Twitter")
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/search.json", "utf8");
                const res = source.apis.json.search.parse(src, 200) as IParsedSearch;

                const images = res.images.filter(i => !!i.file_url);
                expect(images.length).toEqual(11);
                expect(images.map(i => i.id).slice(0, 3)).toEqual(["1274087695145332736", "1274087694105075714", "1274087692003770368"]);
            });
        });

        describe("Gallery", () => {
            it("returns the url", () => {
                expect(gallery(source.apis.json, "123")).toEqual("/1.1/statuses/show.json?id=123&tweet_mode=extended");
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/gallery.json", "utf8");
                const res = source.apis.json.gallery!.parse(src, 200) as IParsedGallery;

                expect(res.images.length).toEqual(3);
                expect(res.imageCount).toEqual(3);
                expect(res.pageCount).toEqual(1);
            });
        });
    });
});

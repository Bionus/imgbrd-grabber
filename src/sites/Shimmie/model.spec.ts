import { search, makeGrabber } from "../test-utils";
import { source } from "./model";
import { readFileSync } from "fs";

describe("Shimmie", () => {
    beforeAll(makeGrabber);

    describe("RSS API", () => {
        it("works for basic listing", () => {
            expect(search(source.apis.rss, "", 2)).toEqual("/rss/images/2")
        });

        it("does not allow tag search", () => {
            expect((search(source.apis.rss, "tag") as IError).error).toContain("Tag search is impossible");
        });

        it("parses the response correctly", () => {
            const src = readFileSync(__dirname + "/resources/search.rss", "utf8");
            const res = source.apis.rss.search.parse(src, 200) as IParsedSearch;

            expect(res.images.length).toEqual(24);
            expect(res.images.map(i => i.id).slice(0, 3)).toEqual([3410, 3409, 3408]);
        });
    });

    describe("Regex API", () => {
        describe("Search", () => {
            it("works for basic listing", () => {
                expect(search(source.apis.html, "", 2)).toEqual("/post/list/2")
            });

            it("performs basic tag search", () => {
                expect(search(source.apis.html, "tag", 2)).toEqual("/post/list/tag/2")
            });

            it("correctly escapes special characters", () => {
                expect(search(source.apis.html, "Fate/Requiem", 2)).toEqual("/post/list/Fate^sRequiem/2")
            });

            it("correctly transforms width: and height: into size: for equality", () => {
                expect(search(source.apis.html, "width:800 height:600", 2)).toEqual("/post/list/size:800x600/2")
            });

            it("correctly transforms width: and height: into size: for comparizon", () => {
                expect(search(source.apis.html, "width:>=800 height:>=600", 2)).toEqual("/post/list/size:>=800x600/2")
                expect(search(source.apis.html, "width:<800 height:<600", 2)).toEqual("/post/list/size:<800x600/2")
            });

            it("correctly transforms ranges as they are not supported", () => {
                expect(search(source.apis.html, "width:600..800", 2)).toEqual("/post/list/width:>=600 width:<=800/2")
                expect(search(source.apis.html, "width:600..", 2)).toEqual("/post/list/width:>=600/2")
                expect(search(source.apis.html, "width:..800", 2)).toEqual("/post/list/width:<=800/2")
            });

            it("correctly maps meta-tokens", () => {
                expect(search(source.apis.html, "tagcount:3", 2)).toEqual("/post/list/tags:3/2")
            });

            it("returns an empty image array on 404", () => {
                const res = source.apis.html.search.parse("", 404) as IParsedSearch;
                expect(res.images).toEqual([]);
            });

            it("returns an error for other error codes", () => {
                const res = source.apis.html.search.parse("", 500) as IError;
                expect(res.error).toEqual("Loading error: HTTP 500");
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/search.html", "utf8");
                const res = source.apis.html.search.parse(src, 200) as IParsedSearch;

                expect(res.images.length).toEqual(24);
                expect(res.images.map(i => i.id).slice(0, 3)).toEqual([3410, 3409, 3408]);
            });
        });

        describe("Details", () => {
            it("returns the url", () => {
                expect(source.apis.html.details!.url("123", "")).toEqual("/post/view/123");
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/details.html", "utf8");
                const res = source.apis.html.details!.parse(src, 200) as IParsedDetails;

                expect((res.tags as any[]).map(t => t.name)).toEqual(["puchiko"]);
                expect(res.imageUrl).toEqual("/_images/33ca85f6ab72d3d31bf95bf9a137d7df/3410%20-%20puchiko.jpg");
                expect(res.createdAt).toEqual("2019-08-21T09:21:25+00:00");
            });
        });

        describe("Check", () => {
            it("returns the root", () => {
                expect(source.apis.html.check!.url()).toEqual("/");
            });

            it("returns true for the home page", () => {
                const src = readFileSync(__dirname + "/resources/home.html", "utf8");
                const res = source.apis.html.check!.parse(src, 200);

                expect(res).toEqual(true);
            });

            it("return false for an invalid page", () => {
                const src = readFileSync(__dirname + "/resources/search.rss", "utf8");
                const res = source.apis.html.check!.parse(src, 200);

                expect(res).toEqual(false);
            });
        });
    });
});

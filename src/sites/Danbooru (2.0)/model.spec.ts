import { search, tags, makeGrabber } from "../test-utils";
import { source } from "./model";
import { readFileSync } from "fs";

describe("Danbooru (2.0)", () => {
    beforeAll(makeGrabber);

    describe("JSON API", () => {
        describe("Search", () => {
            it("works for basic listing", () => {
                expect(search(source.apis.json, "tag1", 2)).toEqual("/posts.json?limit=10&page=2&tags=tag1");
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/search.json", "utf8");
                const res = source.apis.json.search.parse(src, 200) as IParsedSearch;

                expect(res.images.length).toEqual(10);
                expect(res.images.map(i => i.id).slice(0, 3)).toEqual([4018204, 4018203, 4018202]);
            });
        });

        describe("Tags", () => {
            it("works for basic listing", () => {
                expect(tags(source.apis.json, 2)).toEqual("/tags.json?limit=10&search[order]=count&page=2");
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/tags.json", "utf8");
                const res = source.apis.json.tags!.parse(src, 200) as IParsedTags;

                expect(res.tags.length).toEqual(10);
                expect((res.tags as any[]).map(i => i.name).slice(0, 3)).toEqual(["multiple_girls", "blue_eyes", "blonde_hair"]);
            });
        });
    });

    describe("XML API", () => {
        describe("Search", () => {
            it("works for basic listing", () => {
                expect(search(source.apis.xml, "tag1", 2)).toEqual("/posts.xml?limit=10&page=2&tags=tag1");
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/search.xml", "utf8");
                const res = source.apis.xml.search.parse(src, 200) as IParsedSearch;

                expect(res.images.length).toEqual(10);
                expect(res.images.map(i => i.id).slice(0, 3)).toEqual([4018204, 4018203, 4018202]);
            });
        });

        describe("Tags", () => {
            it("works for basic listing", () => {
                expect(tags(source.apis.xml, 2)).toEqual("/tags.xml?limit=10&search[order]=count&page=2");
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/tags.xml", "utf8");
                const res = source.apis.xml.tags!.parse(src, 200) as IParsedTags;

                expect(res.tags.length).toEqual(10);
                expect((res.tags as any[]).map(i => i.name).slice(0, 3)).toEqual(["multiple_girls", "blue_eyes", "blonde_hair"]);
            });
        });
    });

    describe("HTML API", () => {
        describe("Search", () => {
            it("works for basic listing", () => {
                expect(search(source.apis.html, "", 2)).toEqual("/posts?limit=10&page=2&tags=");
            });

            // FIXME: broken
            /*it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/search.html", "utf8");
                const res = source.apis.html.search.parse(src, 200) as IParsedSearch;

                expect(res.images.length).toEqual(10);
                expect(res.images.map(i => i.id).slice(0, 3)).toEqual([4018204, 4018203, 4018202]);
            });*/
        });

        describe("Details", () => {
            it("returns the url", () => {
                expect(source.apis.html.details!.url("123", "")).toEqual("/posts/123");
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/details.html", "utf8");
                const res = source.apis.html.details!.parse(src, 200) as IParsedDetails;

                expect(res.imageUrl).toEqual("https://danbooru.donmai.us/data/__kokkoro_princess_connect_and_1_more_drawn_by_hitotsuki_no_yagi__9b1bb70c1f123a07c8c1966a1ff76b79.jpg");
            });
        });

        describe("Tag types", () => {
            it("works for basic listing", () => {
                expect((source.apis.html.tagTypes as any).url()).toEqual("/tags");
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/tags.html", "utf8");
                const res = (source.apis.html.tagTypes as any).parse(src) as IParsedTagTypes;

                expect(res.types.length).toEqual(5);
                expect(res.types.map(i => i.name)).toEqual(["general", "character", "copyright", "artist", "meta"]);
            });
        });

        describe("Tags", () => {
            it("works for basic listing", () => {
                expect(tags(source.apis.html, 2)).toEqual("/tags?limit=10&search[order]=count&page=2");
            });

            // FIXME: broken
            /*it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/tags.html", "utf8");
                const res = source.apis.html.tags!.parse(src, 200) as IParsedTags;

                expect(res.tags.length).toEqual(10);
                expect((res.tags as any[]).map(i => i.name).slice(0, 3)).toEqual(["multiple_girls", "blue_eyes", "blonde_hair"]);
            });*/
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
                const src = readFileSync(__dirname + "/resources/search.xml", "utf8");
                const res = source.apis.html.check!.parse(src, 200);

                expect(res).toEqual(false);
            });
        });
    });
});

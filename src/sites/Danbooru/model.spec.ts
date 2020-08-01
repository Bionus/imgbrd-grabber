import { search, tags, makeGrabber } from "../test-utils";
import { source } from "./model";
import { readFileSync } from "fs";

describe("Danbooru", () => {
    beforeAll(makeGrabber);

    describe("JSON API", () => {
        describe("Search", () => {
            it("works for basic listing", () => {
                expect(search(source.apis.json, "tag1", 2)).toEqual("/post/index.json?limit=10&page=2&typed_tags=true&tags=tag1");
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/search.json", "utf8");
                const res = source.apis.json.search.parse(src, 200) as IParsedSearch;

                expect(res.images.length).toEqual(10);
                expect(res.images.map(i => i.id).slice(0, 3)).toEqual([647000, 646999, 646998]);
            });
        });

        describe("Tags", () => {
            it("works for basic listing", () => {
                expect(tags(source.apis.json, 2)).toEqual("/tag/index.json?limit=10&page=2");
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/tags.json", "utf8");
                const res = source.apis.json.tags!.parse(src, 200) as IParsedTags;

                expect(res.tags.length).toEqual(10);
                expect((res.tags as any[]).map(i => i.name).slice(0, 3)).toEqual([";]", "004k-star_132", "07_ghost"]);
            });
        });
    });

    describe("XML API", () => {
        describe("Search", () => {
            it("works for basic listing", () => {
                expect(search(source.apis.xml, "tag1", 2)).toEqual("/post/index.xml?limit=10&page=2&typed_tags=true&tags=tag1");
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/search.xml", "utf8");
                const res = source.apis.xml.search.parse(src, 200) as IParsedSearch;

                expect(res.images.length).toEqual(10);
                expect(res.images.map(i => i.id).slice(0, 3)).toEqual([647000, 646999, 646998]);
            });
        });

        describe("Tags", () => {
            it("works for basic listing", () => {
                expect(tags(source.apis.xml, 2)).toEqual("/tag/index.xml?limit=10&page=2");
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/tags.xml", "utf8");
                const res = source.apis.xml.tags!.parse(src, 200) as IParsedTags;

                expect(res.tags.length).toEqual(10);
                expect((res.tags as any[]).map(i => i.name).slice(0, 3)).toEqual(["shin_sekai_yori", "phonebooth", "yamamoto_maki"]);
            });
        });
    });

    describe("HTML API", () => {
        describe("Search", () => {
            it("works for basic listing", () => {
                expect(search(source.apis.html, "", 2)).toEqual("/post/index?limit=10&page=2&typed_tags=true&tags=");
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/search.html", "utf8");
                const res = source.apis.html.search.parse(src, 200) as IParsedSearch;

                expect(res.images.length).toEqual(10);
                expect(res.images.map(i => i.id).slice(0, 3)).toEqual([647000, 646999, 646998]);
            });
        });

        describe("Details", () => {
            it("returns the url", () => {
                expect(source.apis.html.details!.url("123", "")).toEqual("/post/show/123");
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/details.html", "utf8");
                const res = source.apis.html.details!.parse(src, 200) as IParsedDetails;

                expect((res.tags as any[]).map(t => t.name)).toEqual(["animal ears", "cosplay", "fate/grand order", "kimono", "manyu fei (cosplayer)", "pink hair", "tail", "tamamo no mae", "thighhighs", "twintails"]);
            });
        });

        describe("Tags", () => {
            it("works for basic listing", () => {
                expect(tags(source.apis.html, 2)).toEqual("/tag/index?limit=10&page=2");
            });

            it("parses the response correctly", () => {
                const src = readFileSync(__dirname + "/resources/tags.html", "utf8");
                const res = source.apis.html.tags!.parse(src, 200) as IParsedTags;

                expect(res.tags.length).toEqual(50);
                expect((res.tags as any[]).map(i => i.name).slice(0, 3)).toEqual(["4k-star_035", "4k-star_036", "4k-star_037"]);
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
                const src = readFileSync(__dirname + "/resources/search.xml", "utf8");
                const res = source.apis.html.check!.parse(src, 200);

                expect(res).toEqual(false);
            });
        });
    });
});

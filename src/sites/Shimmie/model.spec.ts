import { search } from "../test-utils";
import { source } from "./model";

describe("Shimmie", () => {
    describe("RSS API", () => {
        it("works for basic listing", () => {
            expect(search(source.apis.rss, "", 2)).toEqual("/rss/images/2")
        });

        it("does not allow tag search", () => {
            expect((search(source.apis.rss, "tag") as IError).error).toContain("Tag search is impossible");
        });
    });

    describe("Regex API", () => {
        it("works for basic listing", () => {
            expect(search(source.apis.html, "", 2)).toEqual("/post/list/2")
        });

        it("performs basic tag search", () => {
            expect(search(source.apis.html, "tag", 2)).toEqual("/post/list/tag/2")
        });

        it("correctly transforms width: and height: into size: for equality", () => {
            expect(search(source.apis.html, "width:800 height:600", 2)).toEqual("/post/list/size:800x600/2")
        });

        it("correctly transforms width: and height: into size: for comparizon", () => {
            expect(search(source.apis.html, "width:>=800 height:>=600", 2)).toEqual("/post/list/size:>=800x600/2")
        });

        it("correctly maps meta-tokens", () => {
            expect(search(source.apis.html, "tagcount:3", 2)).toEqual("/post/list/tags:3/2")
        });
    });
});

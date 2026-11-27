import { makeGrabber } from "../test-utils";
import { source } from "./model";

describe("Gelbooru (0.2)", () => {
    beforeAll(makeGrabber);

    describe("Regex API", () => {
        describe("Details", () => {
            const parse = (src: string): IParsedDetails => source.apis.html.details!.parse(src, 200) as IParsedDetails;

            it("parses the default tag list", () => {
                const tags = parse('<li class="tag-type-artist"><a href="#">?</a> <a href="index.php?page=post&amp;s=list&amp;tags=some_artist">some artist</a> <span class="tag-count">12</span></li>').tags;
                expect(tags).toEqual([{ name: "some artist", type: "artist", count: 12 }]);
            });

            it("parses the Realbooru tag links", () => {
                const src = '<div class="sidebar"><div id="tagLink">'
                    + '<span>Posted at Jan, 01 2025 by <a href="#">user</a></span><br />'
                    + '<a class="copyright" href="#">some copyright</a> '
                    + '<a class="character" href="#">some character</a> '
                    + '<div class="row"><a class="model" href="#">some model</a></div> '
                    + '<a class="tag-type-general" href="#">long hair</a> '
                    + '<a class="tag-type-general" href="#">long hair</a> '
                    + '<a class="metadata" href="#"> high res </a>'
                    + '</div><a class="model" href="#">outside</a></div>';
                expect(parse(src).tags).toEqual([
                    { name: "some copyright", type: "copyright" },
                    { name: "some character", type: "character" },
                    { name: "some model", type: "model" },
                    { name: "long hair", type: "general" },
                    { name: "high res", type: "meta" },
                ]);
            });

            it("returns no tags without a tag list", () => {
                expect(parse("<p>Nothing</p>").tags).toEqual([]);
            });

            it("parses the default date format", () => {
                expect(parse("<li>Posted: 2025-01-01 12:34:56<br/>by user</li>").createdAt).toEqual("2025-01-01 12:34:56");
            });

            it("parses the Realbooru date format", () => {
                expect(parse('<div id="tagLink"><span>Posted at Jan, 01 2025 by <a href="#">user</a></span></div>').createdAt).toEqual("2025/01/01 00:00");
                expect(parse("<span>Posted at Dec, 9 2024 by user</span>").createdAt).toEqual("2024/12/09 00:00");
            });

            it("keeps the time of the Realbooru date format if present", () => {
                expect(parse("<span>Posted at Mar, 05 2025 14:07:33 by user</span>").createdAt).toEqual("2025/03/05 14:07");
            });

            it("ignores invalid dates", () => {
                expect(parse("<span>Posted at Foo, 05 2025 by user</span>").createdAt).toBeUndefined();
                expect(parse("<p>No date</p>").createdAt).toBeUndefined();
            });
        });
    });
});

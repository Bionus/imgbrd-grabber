import { makeGrabber } from "../test-utils";
import { source } from "./model";

describe("Gelbooru (0.2)", () => {
    beforeAll(makeGrabber);

    describe("Regex API", () => {
        describe("Details", () => {
            const parse = (src: string): IParsedDetails => source.apis.html.details!.parse(src, 200) as IParsedDetails;

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

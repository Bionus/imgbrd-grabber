import { makeGrabber, search } from "../test-utils";
import { source } from "./model";

describe("WallHaven", () => {
    beforeAll(makeGrabber);

    it("preserves the existing defaults for regular searches", () => {
        expect(search(source.apis.json, "night sky", 2)).toBe(
            "/api/v1/search?q=night%20sky&purity=111&categories=111&page=2&sorting=date_added&order=desc"
        );
    });

    it("supports exact resolutions and minimum resolution", () => {
        expect(search(source.apis.json, "nature resolution:1920x1080,2560:1440", 1)).toContain(
            "resolutions=1920x1080%2C2560x1440"
        );
        expect(search(source.apis.json, "nature atleast:3840/2160", 1)).toContain("atleast=3840x2160");
    });

    it("uses the last resolution mode when exact and minimum filters conflict", () => {
        const exact = search(source.apis.json, "atleast:3840x2160 res:1920x1080", 1);
        expect(exact).toContain("resolutions=1920x1080");
        expect(exact).not.toContain("atleast=");

        const minimum = search(source.apis.json, "resolution:1920x1080 atleast:3840x2160", 1);
        expect(minimum).toContain("atleast=3840x2160");
        expect(minimum).not.toContain("resolutions=");
    });

    it("supports aspect ratio and color filters", () => {
        const url = search(source.apis.json, "city ratio:16:9 color:#CC6633", 1);
        expect(url).toContain("q=city");
        expect(url).toContain("ratios=16x9");
        expect(url).toContain("colors=cc6633");
    });

    it("only sends topRange for toplist searches", () => {
        expect(search(source.apis.json, "order:toplist_desc topRange:1M", 1)).toContain(
            "sorting=toplist&order=desc&topRange=1M"
        );
        expect(search(source.apis.json, "order:views topRange:1M", 1)).not.toContain("topRange=");
    });
});

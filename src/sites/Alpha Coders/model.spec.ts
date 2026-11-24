import { makeGrabber, search } from "../test-utils";
import { source } from "./model";

const resultCard = `
<div id="content_123">
    <meta content="https://images.example/123.jpeg" itemprop="contentUrl">
    <meta itemprop="url" content="https://wall.alphacoders.com/big.php?i=123&amp;lang=en">
    <meta itemprop="thumbnailUrl" content="https://images.example/thumb-123.webp">
    <meta itemprop="name" content="Night &amp; Sky">
    <meta itemprop="keywords" content="Night, Blue Sky, Stars">
    <meta itemprop="datePublished" content="2026-08-30">
    <span>(3840x2160)</span>
</div>`;

describe("Wallpaper Abyss", () => {
    beforeAll(() => {
        makeGrabber();
        Grabber.htmlDecode = (text: string): string => text.replace(/&amp;/g, "&");
    });

    it("builds featured and text search URLs", () => {
        expect(search(source.apis.html, "", 2)).toBe("/featured.php?page=2");
        expect(search(source.apis.html, "blue sky", 2)).toBe("/search.php?search=blue%20sky&page=2");
    });

    it("parses result cards and HTML entities", () => {
        const parsed = source.apis.html.search.parse(`[16,200+]${resultCard}`, 200) as IParsedSearch;

        expect(parsed.imageCount).toBe(16200);
        expect(parsed.images).toHaveLength(1);
        expect(parsed.images[0]).toMatchObject({
            id: "123",
            name: "Night & Sky",
            page_url: "https://wall.alphacoders.com/big.php?i=123&lang=en",
            file_url: "https://images.example/123.jpeg",
            preview_url: "https://images.example/thumb-123.webp",
            width: 3840,
            height: 2160,
            ext: "jpeg",
            rating: "safe",
            created_at: "2026-08-30",
            tags: ["Night", "Blue Sky", "Stars"],
        });
    });

    it("ignores incomplete cards instead of creating blank results", () => {
        const parsed = source.apis.html.search.parse(`${resultCard}<div id="content_456"><meta itemprop="name" content="Missing URLs"></div>`, 200) as IParsedSearch;
        expect(parsed.images).toHaveLength(1);
    });

    it("parses full-size details without confusing recommendations for the main image", () => {
        const details = `
            <meta itemprop="name" content="Night &amp; Sky">
            <meta itemprop="contentUrl" href="https://images.example/123.jpeg">
            <meta itemprop="datePublished" content="2026-08-30">
            <meta itemprop="keywords" content="Night, Blue Sky, Stars">
            ${resultCard}`;
        const parsed = source.apis.html.details!.parse(details, 200) as IParsedDetails;

        expect(source.apis.html.details!.url("123", "", { baseUrl: "/", loggedIn: false })).toBe("/big.php?i=123");
        expect(parsed).toEqual({
            imageUrl: "https://images.example/123.jpeg",
            createdAt: "2026-08-30",
            tags: ["Night", "Blue Sky", "Stars"],
        });
    });

    it("recognizes both site titles", () => {
        expect(source.apis.html.check!.parse("<title>Wallpaper Abyss - Wallpapers</title>", 200)).toBe(true);
        expect(source.apis.html.check!.parse("<title>Alpha Coders</title>", 200)).toBe(true);
    });
});

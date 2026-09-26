import { makeGrabber, search } from "../test-utils";
import { source } from "./model";

const post = {
    id: "post-1",
    user: "123",
    service: "patreon",
    title: "Example post",
    published: "2026-08-30T12:00:00",
    file: { name: "main.png", path: "/aa/bb/main.png" },
    attachments: [
        { name: "second.jpg", path: "/cc/dd/second.jpg" },
    ],
};

describe("Kemono", () => {
    beforeAll(makeGrabber);

    describe("JSON API", () => {
        it("uses the current recent-posts endpoint and required header", () => {
            expect(search(source.apis.json, "landscape", 2)).toEqual({
                url: "/api/v1/posts?o=50&q=landscape",
                headers: { "Accept": "text/css" },
            });
        });

        it("supports creator searches without passing the user token as a query", () => {
            expect(search(source.apis.json, "user:patreon:123 blue sky", 2)).toEqual({
                url: "/api/v1/patreon/user/123/posts?o=50&q=blue%20sky",
                headers: { "Accept": "text/css" },
            });
        });

        it("parses wrapped recent posts and counts the main file", () => {
            const result = source.apis.json.search.parse(JSON.stringify({ posts: [post] }), 200) as IParsedSearch;

            expect(result.images).toHaveLength(1);
            expect(result.images[0]).toMatchObject({
                id: "post-1",
                type: "gallery",
                gallery_count: 2,
                file_url: "/data/aa/bb/main.png",
                preview_url: "/thumbnail/data/aa/bb/main.png",
                page_url: "/patreon/user/123/post/post-1",
                created_at: "2026-08-30T12:00:00",
            });
        });

        it("parses creator post arrays", () => {
            const result = source.apis.json.search.parse(JSON.stringify([post]), 200) as IParsedSearch;
            expect(result.images).toHaveLength(1);
        });

        it("loads both the main file and attachments from gallery details", () => {
            const request = source.apis.json.gallery!.url({
                id: "post-1",
                md5: "",
                page: 1,
                identity: { service: "patreon", user: "123", id: "post-1" },
            }, {
                page: 1,
                limit: 50,
                loggedIn: false,
                baseUrl: "/",
            });
            expect(request).toEqual({
                url: "/api/v1/patreon/user/123/post/post-1",
                headers: { "Accept": "text/css" },
            });

            const result = source.apis.json.gallery!.parse(JSON.stringify({
                post,
                attachments: [post.file],
                previews: [{ path: "/ee/ff/main-preview.png" }],
            }), 200) as IParsedGallery;

            expect(result.imageCount).toBe(2);
            expect(result.images.map((image) => image.file_url)).toEqual([
                "/data/aa/bb/main.png",
                "/data/cc/dd/second.jpg",
            ]);
            expect(result.images.map((image) => image.name)).toEqual(["main.png", "second.jpg"]);
            expect(result.images.map((image) => image.preview_url)).toEqual([
                "/data/ee/ff/main-preview.png",
                "/thumbnail/data/cc/dd/second.jpg",
            ]);
        });

        it("reports API errors instead of treating them as empty results", () => {
            expect(source.apis.json.search.parse('{"error":"Not Found"}', 404)).toEqual({ error: "Not Found" });
            expect(source.apis.json.gallery!.parse('{"error":"Not Found"}', 404)).toEqual({ error: "Not Found" });
        });
    });
});

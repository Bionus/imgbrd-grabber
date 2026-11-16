import { makeGrabber } from "../test-utils";
import { source } from "./model";

// Minimal XML-backed adapter for the metadata selectors used by this parser.
// Native integration tests exercise the real Grabber HTML implementation.
function parseHTML(src: string) {
    const doc = Grabber.parseXML(src).html;
    return {
        find(selector: string): any[] {
            if (selector === "#taglist") {
                return [{ find: () => [] }];
            }
            if (selector === ".gpc") {
                return Grabber.makeArray(doc.p).filter((p: any) =>
                    (p["@attributes"]?.class || "").split(/\s+/).indexOf("gpc") !== -1
                ).map((p: any) => ({ innerText: () => p["#text"] }));
            }
            return [];
        },
    };
}

function galleryMarkup(layout: string, start: number, count: number, range: string): string {
    const thumbnails = Array.from({ length: count }, (_, index) => {
        const position = start + index;
        const url = `https://e-hentai.org/s/token/123-${position}`;
        const style = "background:url(https://example.org/thumb.jpg) 0px 0px; width:100px; height:100px";
        if (layout === "gdtm") {
            return `<div class="gdtm"><div style="${style}"><a href="${url}"><img src="https://example.org/thumb.jpg"/></a></div></div>`;
        }
        if (layout === "gdtl") {
            return `<div class="gdtl"><a href="${url}"><img src="https://example.org/thumb.jpg"/></a></div>`;
        }
        return `<a href="${url}"><div title="Page ${position}: image.jpg" style="${style}"></div></a>`;
    }).join("");
    return `<html><div id="taglist"></div>${range}${thumbnails}</html>`;
}

function parse(src: string): IParsedGallery {
    return source.apis.html.gallery!.parse(src, 200) as IParsedGallery;
}

describe("E-Hentai gallery metadata", () => {
    beforeAll(() => {
        makeGrabber();
        Grabber.parseHTML = parseHTML;
    });

    for (const layout of ["gdtm", "gdtl", "sprite"]) {
        describe(layout, () => {
            it.each([
                [1, 20, 100],
                [21, 20, 100],
                [41, 3, 43],
                [41, 40, 81],
                [81, 1, 81],
                [1001, 40, 1200],
            ])("parses absolute positions from %i (%i images of %i)", (start, count, total) => {
                const number = (n: number) => n.toLocaleString("en-US");
                const range = `<p data-extra='value' class='extra gpc'>\n Showing  ${number(start)} - ${number(start + count - 1)}\n of ${number(total)} images </p>`;
                const result = parse(galleryMarkup(layout, start, count, range));
                expect(result.imageCount).toBe(total);
                expect(result.images).toHaveLength(count);
                result.images.forEach((image, index) => {
                    expect(image.tokens).toEqual({ gallery_position: start + index, gallery_count: total });
                    expect(image.page_url).toContain(`123-${start + index}`);
                    expect(image.preview_url).toBe("https://example.org/thumb.jpg");
                    expect(image.position).toBeUndefined(); // Legacy position is still assigned by the native parser.
                });
            });

            it.each([
                "",
                '<p class="gpc">Unexpected text</p>',
                '<p class="gpc">Showing 0 - 20 of 100 images</p>',
                '<p class="gpc">Showing 40 - 21 of 100 images</p>',
                '<p class="gpc">Showing 21 - 40 of 30 images</p>',
            ])("leaves unknown metadata empty: %s", range => {
                const result = parse(galleryMarkup(layout, 21, 20, range));
                expect(result.imageCount).toBe(-1);
                expect(result.images).toHaveLength(20);
                result.images.forEach(image => {
                    expect(image.tokens).toEqual({ gallery_position: 0, gallery_count: -1 });
                });
            });

            it("does not guess positions when thumbnail matches are incomplete", () => {
                const result = parse(galleryMarkup(layout, 21, 3, '<p class="gpc">Showing 21 - 40 of 100 images</p>'));
                expect(result.imageCount).toBe(100);
                expect(result.images).toHaveLength(3);
                result.images.forEach(image => {
                    expect(image.tokens).toEqual({ gallery_position: 0, gallery_count: 100 });
                });
            });
        });
    }
});

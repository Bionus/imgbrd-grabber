import rewire from "rewire"
const model = rewire("./model")
const makeGallery = model.__get__("makeGallery")
const makeImage = model.__get__("makeImage")
const makeTag = model.__get__("makeTag")
// @ponicode
describe("makeGallery", () => {
    test("0", () => {
        let callFunction: any = () => {
            makeGallery(true)
        }
    
        expect(callFunction).not.toThrow()
    })

    test("1", () => {
        let callFunction: any = () => {
            makeGallery(12345)
        }
    
        expect(callFunction).not.toThrow()
    })

    test("2", () => {
        let callFunction: any = () => {
            makeGallery({ tags: true, tag_ids: " data:image/svg+xml;charset=UTF-8,%3Csvg%20xmlns%3D%22http%3A%2F%2Fwww.w3.org%2F2000%2Fsvg%22%20version%3D%221.1%22%20baseProfile%3D%22full%22%20width%3D%22undefined%22%20height%3D%22undefined%22%3E%3Crect%20width%3D%22100%25%22%20height%3D%22100%25%22%20fill%3D%22grey%22%2F%3E%3Ctext%20x%3D%22NaN%22%20y%3D%22NaN%22%20font-size%3D%2220%22%20alignment-baseline%3D%22middle%22%20text-anchor%3D%22middle%22%20fill%3D%22white%22%3Eundefinedxundefined%3C%2Ftext%3E%3C%2Fsvg%3E", type: "array" })
        }
    
        expect(callFunction).not.toThrow()
    })

    test("3", () => {
        let callFunction: any = () => {
            makeGallery("https://accounts.google.com/o/oauth2/revoke?token=%s")
        }
    
        expect(callFunction).not.toThrow()
    })

    test("4", () => {
        let callFunction: any = () => {
            makeGallery(56784)
        }
    
        expect(callFunction).not.toThrow()
    })

    test("5", () => {
        let callFunction: any = () => {
            makeGallery(NaN)
        }
    
        expect(callFunction).not.toThrow()
    })
})

// @ponicode
describe("makeImage", () => {
    test("0", () => {
        let callFunction: any = () => {
            makeImage(4)
        }
    
        expect(callFunction).not.toThrow()
    })

    test("1", () => {
        let callFunction: any = () => {
            makeImage("http://placeimg.com/640/480/nightlife")
        }
    
        expect(callFunction).not.toThrow()
    })

    test("2", () => {
        let callFunction: any = () => {
            makeImage(true)
        }
    
        expect(callFunction).not.toThrow()
    })

    test("3", () => {
        let callFunction: any = () => {
            makeImage("http://placeimg.com/640/480/abstract")
        }
    
        expect(callFunction).not.toThrow()
    })

    test("4", () => {
        let callFunction: any = () => {
            makeImage(false)
        }
    
        expect(callFunction).not.toThrow()
    })

    test("5", () => {
        let callFunction: any = () => {
            makeImage(NaN)
        }
    
        expect(callFunction).not.toThrow()
    })
})

// @ponicode
describe("makeTag", () => {
    test("0", () => {
        let callFunction: any = () => {
            makeTag(true)
        }
    
        expect(callFunction).not.toThrow()
    })

    test("1", () => {
        let callFunction: any = () => {
            makeTag({ count: -100, id: 12345, type: "number", name: "George" })
        }
    
        expect(callFunction).not.toThrow()
    })

    test("2", () => {
        let callFunction: any = () => {
            makeTag("Anas")
        }
    
        expect(callFunction).not.toThrow()
    })

    test("3", () => {
        let callFunction: any = () => {
            makeTag(-100)
        }
    
        expect(callFunction).not.toThrow()
    })

    test("4", () => {
        let callFunction: any = () => {
            makeTag(-5.48)
        }
    
        expect(callFunction).not.toThrow()
    })

    test("5", () => {
        let callFunction: any = () => {
            makeTag({ count: Infinity, id: Infinity, type: "", name: "" })
        }
    
        expect(callFunction).not.toThrow()
    })
})

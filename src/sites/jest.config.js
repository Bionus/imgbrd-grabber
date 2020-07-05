module.exports = {
    preset: "ts-jest",
    testEnvironment: "node",
    moduleFileExtensions: ["ts", "js"],
    globals: {
        'ts-jest': {
            tsConfig: {
                types: undefined,
            },
            diagnostics: {
                ignoreCodes: [151001],
            },
        },
    },
};
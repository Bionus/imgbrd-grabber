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

    // Code coverage
    collectCoverage: true,
    coverageDirectory: "../../coverage/",
    collectCoverageFrom: [
        "**/*.ts",
        "!**/node_modules/**",
        "!test-*.ts",
    ],
};
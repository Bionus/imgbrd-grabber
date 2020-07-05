export function search(api: IApi, query: string, page: number = 1) {
    return api.search.url(
        { search: query, page },
        { limit: 10, loggedIn: false, baseUrl: "/" },
        undefined
    );
}
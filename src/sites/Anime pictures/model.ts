const mainHostname = 'anime-pictures.net';
const imageMap = {
    'id': 'id',
    'md5': 'md5',
    'width': 'width',
    'height': 'height',
    'created_at': 'pubtime',
    'score': 'score_number',
    'file_size': 'size',
    'ext': 'ext',
    'file_url': 'file_url',
    'sample_url': 'big_preview',
    'preview_url': 'small_preview'
};
const tagMap = {
    id: 'id',
    name: 'tag',
    count: 'num',
    typeId: 'type'
};

function noWebpAvif(url){
    return url.replace(/(\.\w{3,4})\.(?:webp|avif)/, '$1');
}

function completeImage(image, raw){
    // Replace '.jpg' by just 'jpg' in the extension.
    if (image.ext && image.ext[0] === '.'){
        image.ext = image.ext.substring(1);
    }
    
    // Remove the '.webp' and '.avif' suffix to previews.
    image.sample_url = noWebpAvif(image.sample_url || '');
    image.preview_url = noWebpAvif(image.preview_url || '');
    let md5Part = image.md5.substring(0, 3).concat('/', image.md5);
    // If no URL is passed at all, build it ourselves.
    if (!image.preview_url && !image.sample_url && !image.file_url){
        let previewExt = raw['have_alpha'] === true ? 'png' : 'jpg';
        image.preview_url = '//opreviews.'.concat(mainHostname, '/', md5Part, '_sp.', previewExt);
        image.sample_url = '//opreviews.'.concat(mainHostname, '/', md5Part, '_bp.', previewExt);
        // Valid but blocked by Cloudflare.
        // image.file_url = '//oimages.'.concat(mainHostname, '/', md5Part, '.', image.ext);
    }
    
    // If sample URL is bad but preview URL is good.
    if ((!image.sample_url || image.sample_url.length < 5) && image.preview_url && image.preview_url.length >= 5){
        image.sample_url = image.preview_url
            .replace('_cp.', '_bp.')
            .replace('_sp.', '_bp.');
    }
    
    // If file URL is bad.
    if (!image.file_url || !image.file_url.includes(image.md5)){
        // Seemingly legacy redirect currently used on site. Probably subject to eventual deprecation.
        image.file_url = `//api.${mainHostname}/pictures/get_image/`.concat(raw.file_url);
        
        // Valid but blocked by Cloudflare.
        // image.file_url = '//oimages.'.concat(mainHostname, '/', md5Part, '.', image.ext);
        
        // Another seeming legacy tibit used internally with the previous above pattern.
        // image.file_url.concat('?if=ANIME-PICTURES.NET_-_', raw.file_url);
    }
    
    return image;
}
function sizeToUrl(size, key, ret){
    let op;
    
    if (size.indexOf('<=') === 0){
        size = size.substr(2);
        op = 0;
    }
    
    else if (size.indexOf('>=') === 0){
        size = size.substr(2);
        op = 1;
    }
    
    else if (size[0] === '<'){
        size = String(parseInt(size.substr(1), 10) - 1);
        op = 0;
    }
    
    else if (size[0] === '>'){
        size = String(parseInt(size.substr(1), 10) + 1);
        op = 1;
    }
    
    ret.push(key + '=' + size);
    if (op !== undefined){
        ret.push(key + '_n=' + op);
    }
}
function searchToUrl(page, search, previous){
    const parts = search.split(' ');
    const tags = [];
    const denied = [];
    const ret = [];
    
    for (let index = 0; index < parts.length; index++){
        let tag = parts[index];
        let part = tag.trim();
        
        if (part.indexOf('width:') === 0){
            sizeToUrl(part.substr(6), 'res_x', ret);
        }
        else if (part.indexOf('height:') === 0){
            sizeToUrl(part.substr(7), 'res_y', ret);
        }
        else if (part.indexOf('ratio:') === 0){
            ret.push('aspect=' + part.substr(6));
        }
        else if (part.indexOf('order:') === 0){
            ret.push('order_by=' + part.substr(6));
        }
        else if (part.indexOf('filetype:') === 0){
            const ext = part.substr(9);
            ret.push('ext_' + ext + '=' + ext);
        }
        else if (part[0] === '-'){
            denied.push(encodeURIComponent(tag.substr(1)));
        }
        else if (tag.length > 0){
            tags.push(encodeURIComponent(tag));
        }
    }
    
    if (tags.length > 0){
        ret.unshift('search_tag=' + tags.join(' '));
    }
    
    if (denied.length > 0){
        ret.unshift('denied_tags=' + denied.join(' '));
    }
    
    if (previous && previous.minDate && previous.page === page - 1){
        ret.push('last_page=' + (previous.page - 1));
        ret.push('last_post_date=' + previous.minDate);
    }
    
    return ret.join('&');
}

export var source = {
    name: 'Anime pictures',
    modifiers: ['width:', 'height:', 'ratio:', 'order:', 'filetype:'],
    forcedTokens: ['tags'],
    tagFormat: {
        case: 'lower',
        wordSeparator: ' ',
    },
    searchFormat: {
        and: ' && ',
        or: ' || ',
        parenthesis: false,
        precedence: 'and',
    },
    auth: {
        session: {
            type: 'post',
            url: '/login/submit',
            fields: [
                {
                    id: 'pseudo',
                    key: 'login'
                },
                {
                    id: 'password',
                    key: 'password',
                    type: 'password'
                }
            ],
            check: {
                type: 'cookie',
                key: 'asian_server'
            }
        }
    },
    apis: {
        json: {
            name: 'JSON',
            auth: [],
            search: {
                url: function (query, opts, previous){
                    const page = query.page - 1;
                    return `//api.${mainHostname}/api/v3/posts?page=`.concat(page, '&', searchToUrl(query.page, query.search, previous), '&posts_per_page=', opts.limit, '&lang=en');
                },
                parse: function (src){
                    const data = JSON.parse(src);
                    const images = [];
                    
                    for (let index = 0; index < data.response_posts_count; index++){
                        let image = data.posts[index];
                        image = completeImage(Grabber.mapFields(image, imageMap), image);
                        images.push(image);
                    }
                    
                    return {
                        images: images,
                        imageCount: data['posts_count'],
                        pageCount: data['max_pages'] + 1, // max_pages is an index, not a count, and pages start at 0
                    };
                }
            },
            details: {
                url: function (id, md5){
                    return `//api.${mainHostname}/api/v3/posts/`.concat(id, '?lang=en');
                },
                parse: function (src){
                    const data = JSON.parse(src);
                    const tags = data.tags.map(function(tagDefinition){return Grabber.mapFields(tagDefinition.tag, tagMap)});
                    data.post.file_url = data.file_url;
                    let image = completeImage(Grabber.mapFields(data.post, imageMap), data.post);
                    
                    return {
                        tags: tags,
                        imageUrl: image.file_url,
                        createdAt: image.pubtime
                    };
                }
            }
        }
    }
};

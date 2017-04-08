$(function() {
	var feature = 1;
	$('.features > .feature').each(function() {
		$(this).find('a > img').each(function() {
			var p = $(this).parent();
			p.addClass('fancybox');
			p.attr('data-fancybox', 'feature' + feature);
		});
		feature++;
	}),
	$('.fancybox').fancybox({
		thumbs: false,
		fullScreen: false
	});
});

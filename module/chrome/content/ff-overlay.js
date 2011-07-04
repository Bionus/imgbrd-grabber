imgbrdgrabber.onFirefoxLoad = function(event) {
	document.getElementById("contentAreaContextMenu").addEventListener("popupshowing", function (e){ imgbrdgrabber.showFirefoxContextMenu(e); }, false);
};

imgbrdgrabber.showFirefoxContextMenu = function(event) {
	// show or hide the menuitem based on what the context menu is on
	document.getElementById("context-imgbrdgrabber").hidden = gContextMenu.onImage;
};

window.addEventListener("load", function () { imgbrdgrabber.onFirefoxLoad(); }, false);

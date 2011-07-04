var imgbrdgrabber = {
	onLoad: function() {
		// initialization code
		this.initialized = true;
		this.strings = document.getElementById("imgbrdgrabber-strings");
	},

	viewItLater: function(e) {
		var promptService = Components.classes["@mozilla.org/embedcomp/prompt-service;1"].getService(Components.interfaces.nsIPromptService);
		promptService.alert(window, this.strings.getString("helloMessageTitle"), this.strings.getString("helloMessage"));
	},
	view: function(e) {
		imgbrdgrabber.viewItLater(e);
	},
	batch: function(e) {
		imgbrdgrabber.viewItLater(e);
	},

	onToolbarButtonCommand: function(e) {
		imgbrdgrabber.viewItLater(e);
	}
};

window.addEventListener("load", function () { imgbrdgrabber.onLoad(); }, false);

#import "ios-share-utils.h"
#import <UIKit/UIKit.h>
#import <QGuiApplication>
#import <QQuickWindow>


IosShareUtils::IosShareUtils(QQuickItem *parent)
	: BaseShareUtils(parent)
{}

bool IosShareUtils::share(const QString &text)
{
	NSMutableArray *sharingItems = [NSMutableArray new];
	if (!text.isEmpty()) {
		[sharingItems addObject:text.toNSString()];
	}

	UIViewController *qtController = [[UIApplication sharedApplication].keyWindow rootViewController];

	UIActivityViewController *activityController = [[UIActivityViewController alloc] initWithActivityItems:sharingItems applicationActivities:nil];
	if ( [activityController respondsToSelector:@selector(popoverPresentationController)] ) { // iOS8
		activityController.popoverPresentationController.sourceView = qtController.view;
	}
	[qtController presentViewController:activityController animated:YES completion:nil];

	return true;
}

bool IosShareUtils::sendFile(const QString &path, const QString &mimeType, const QString &title)
{
	return true;
}

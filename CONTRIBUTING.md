# Contributing to Grabber

:+1::tada: First off, thanks for taking the time to contribute! :tada::+1:

The following is a set of guidelines for contributing to Grabber, not rules. Use your best judgment, and feel free to propose changes to this document in a pull request.

#### Table Of Contents

  * [Suggesting Enhancements](#suggesting-enhancements)
  * [Reporting Bugs](#reporting-bugs)
  * [Reporting Crashes](#reporting-crashes)
  * [Pull Requests](#pull-requests)
  * [Translations](#translations)

## Suggesting Enhancements

This section guides you through submitting an enhancement suggestion, including completely new features and minor improvements to existing functionality. Following these guidelines helps maintainers and the community understand your suggestion and find related suggestions.

Start by [a search](https://github.com/Bionus/imgbrd-grabber/issues?q=is%3Aissue) to see if your idea was not already suggested. If so, you add information to it if you feel it could help, bump it or simply add a :+1: reaction to the original post.

After you've determined that your idea have not already been suggested, create an issue on Github and provide the following information:

* **Use a clear and descriptive title** for the issue to identify the suggestion.
* **Provide a step-by-step description of the suggested enhancement** in as many details as possible.
* **Describe the current behavior** and **explain which behavior you expected to see instead** and why.
* **Explain why this enhancement would be useful** to most users.
* **List some other applications where this enhancement exists.**

## Reporting Bugs

Following these guidelines helps maintainers and the community understand your report, reproduce the behavior, and find related reports, which in turn helps fixing the issue you faced.

Start by [a search](https://github.com/Bionus/imgbrd-grabber/issues?q=is%3Aissue) to see if the problem has already been reported. If so, you add information to it if you feel it could help.

After you've determined that your bug have not already been reported, create an issue on Github and provide the following information:

* **Use a clear and descriptive title** for the issue to identify the problem.
* **Describe the exact steps which reproduce the problem** in as many details as possible. **Don't just say what you did, but explain how you did it**.
* **Describe the behavior you observed after following the steps** and point out what exactly is the problem with that behavior.
* **Explain which behavior you expected to see instead and why.**
* **Include screenshots and animated GIFs** which show you following the described steps and clearly demonstrate the problem.
* **If the problem wasn't triggered by a specific action**, describe what you were doing before the problem happened and share more information using the guidelines below.
* **Keep screenshots safe-for-work**. People may read the issues from work or in public, so keep it safe. Most imageboards accept `rating:safe` searches to limit the results to safe-for-work images.

Provide more context by answering these questions:

* **Did the problem start happening recently** (e.g. after updating to a new version) or was this always a problem?
* If the problem started happening recently, **can you reproduce the problem in an older version?** What's the most recent version in which the problem doesn't happen? You can download older versions of Grabber from [the releases page](https://github.com/Bionus/Grabber/releases).
* **Can you reliably reproduce the issue?** If not, provide details about how often the problem happens and under which conditions it normally happens.

Include details about your configuration and environment:

* **Which version of Grabber are you using?** You can get the exact version by going in the `About` menu and clicking `About Grabber`.
* **What's the name and version of the OS you're using**?
* **Are you running Grabber in a virtual machine?** If so, which VM software are you using and which operating systems and versions are used for the host and the guest?
* **Provide your `main.log` file** to see if any obvious error occured.
* **Provide your `settings.ini` file** will greatly help maintainers reproduce your problem if it may be caused by your configuration.

Note that both `main.log` and `settings.ini` files can be found in `C:/Users/%USERNAME%/AppData/Local/Bionus/Grabber` in Windows, and in the installation directory on Linux.

## Reporting crashes

Start by follwing the [Reporting Bugs](#reporting-bugs) guidelines. Additionally, a few more information will be required:

* **Provide the crash dump** that was linked by the Crash Reporter tool.
* **If the crash occured during a download, make sure to provide said download**. You can simply provide the `.igl` file that Grabber can generate by clicking the `Save` button of the `Downloads` tab.

## Pull Requests

If you want to ensure your pull request is merged in a timely manner, it is better to follow a few guidelines:

* **Use an explicit commit message**, to help maintainers understand what your pull request is about.
* **If you changed the code, make sure the program still passes the tests**. They are run automatically on Travis in any case, but your pull request won't be merged if the tests are broken.
* **Follow the coding style of the project**. There is no fixed coding style, but please try to adapt to the code of the project.
* **Document the code** if what you added is not obvious.
* **If your pull request is complex, add a detailed description**. You can either add it in your commit message or in the Github discussion.

## Translations

Check the [Translation](https://github.com/Bionus/imgbrd-grabber/wiki/Translation) Wiki page for information on how to add or update a translation.
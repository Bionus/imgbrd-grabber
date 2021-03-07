---
title: "Moving the CI to Github actions"
date: 2020-11-19 18:22:00 +0100
---


Hey everyone!

&nbsp;

Some might have already noticed, but the Grabber build is slowly being migrated to [Github actions](https://github.com/features/actions).

Until now, it was using [AppVeyor](https://www.appveyor.com/) for Windows builds and [Travis CI](https://travis-ci.org/) for Linux and MacOS builds.

<!--more-->

While adding the new Android build, a new nightly has been introduced, which is the "Github nightly":  
<https://github.com/Bionus/imgbrd-grabber/releases/nightly-github>

It's been running for a few weeks with no issues or failing builds for no reason (unlike the other platforms). The new Linux AppImage format has also been implemented using them very quickly.

Now that I consider it quite stable, I plan to first migrate the "official" nightlies to it, and generate the next official Grabber release with it as well.

The main goal of this migration is simplicity, i.e. not having to check multiple websites to know if the build is failing or not. Having everything in Github makes it clearer. Also, the fact that it supports all platforms directly allows to replace two third-party services by only one first-party.

&nbsp;

Best,  
Bionus
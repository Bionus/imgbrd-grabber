---
layout: docs
title: Blombooru
---



!!! warning

    Using commands can slow down huge batch downloads (a recent computer may need from 100ms to 1s more per image)



## Blombooru

### Install
Follow the official [Quick Start](https://github.com/mrblomblo/blombooru#quick-start-pre-built-image) documentation from the Blombooru repository.
Note that you'll need to have [Docker](https://docs.docker.com/get-docker/) installed.

It pretty much only amounts to downloading the `docker-compose.yml` and `example.env` files, renaming the latter to `.env`, and doing `docker compose up -d`. Blombooru defaults to port `8000`. If you change the port, you'll also need to change it in the `blombooru.js` script.


### Configuration

* Open your instance at [http://localhost:8000/](http://localhost:8000/) and go through the onboarding to set your admin username and password
* Open the admin panel, go to the "Account" section and create a new API key. Make sure to set its permission level to at least "Write" (an Admin-level key also works, but is not recommended), otherwise uploads will be rejected with a 403 error
* Copy the generated key (it starts with `blom_` and is only ever shown once)




## Grabber

### Install NodeJS

You need Node.js to be installed on your machine to use the upload script used by Grabber.
You can download it from [their website](https://nodejs.org/en/download/), or from a package manager [here](https://nodejs.org/en/download/package-manager/).


### Download the upload script

Download the [blombooru.js](blombooru.js) file into Grabber's installation folder.

!!! info

    If your Blombooru instance is not on the same machine as Grabber, or simply not accessible at `http://localhost:8000/`, make sure to update `BASE_URL` in the script.


### Install NodeJS global packages

This script uses the Node.js [axios](https://www.npmjs.com/package/axios) and [form-data](https://www.npmjs.com/package/form-data) packages, so you can install them with:
```bash
npm install -g axios form-data
```

Make sure the `NODE_PATH` environment variable is properly set to point to your global node_modules folder. On Windows, it's usually:
```
C:\Users\%USERNAME%\AppData\Roaming\npm\node_modules
```

But you can check the exact path with:
```bash
npm root -g
```


### Configuration

Open Grabber, then go to "Options > Commands", and set the "Image" field to:
```bash
node blombooru.js "YOUR_API_KEY" "%path:nobackslash%" "%all:includenamespace,unsafe,underscores%" "%rating%" "%source:raw%"
```

Make sure to replace `YOUR_API_KEY` with your API key (including the `blom_` prefix).

This command will be run every time an image is saved, causing it to also be sent to your Blombooru instance!

!!! info

    Blombooru only has three ratings (`safe`, `questionable`, `explicit`) and five tag categories (`general`, `artist`, `character`, `copyright`, `meta`). The script automatically maps whatever Grabber provides onto these, collapsing anything in between (e.g. "sensitive"/"sketchy") into `questionable`, and anything outside those five namespaces into `general`.

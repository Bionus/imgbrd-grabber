---
title: Command-line interface
---


## Introduction

Starting 4.0.0 version, Grabber can be run through the command line.


## Compilation

There are two ways to use the command line.

When compiling Grabber, just uncomment "CONFIG += use\_cli" to set the CLI to be the default behavior. The downside is that running it in GUI mode with the "-g" or "--gui" command will pop up a console too if not run from a terminal already.

If you keep it commented or just use the precompiled version, Grabber will start with its GUI by default, which can be disabled with the "-c" or "--cli" commands.


## Commands

In this part, I'll consider that you are using the precompiled version, which has "CONFIG += use\_cli" commented.

### Help
You can find help with the "-?", "-h", or "--help" commands.

### Sources
You can set a source using the "-s" or "--sources" parameter, followed by the list of hosts to connect, separated by spaces. For example:
```bash
./grabber -c -s "danbooru.donmai.us gelbooru.com"
```


## Usage

### Count the number of images
Prints the result followed by a new line to stdout.
```bash
./grabber -c -t "inugami_kira" -s "danbooru.donmai.us" --return-count
288

./grabber -c -t "inugami_kira" -s "gelbooru.com" --return-count
297

./grabber -c -t "inugami_kira" -s "danbooru.donmai.us gelbooru.com" --return-count
585
```

### Get tags
Prints each result separated by a new line. Each part is separated by a tabulation character.
```
./grabber -c -t "inugami_kira" -s "danbooru.donmai.us" --return-tags
inugami_kira    288     artist
highres 458000  general
1girl   1090000 general
absurdres       87000   general
thighhighs      257000  general
...

./grabber -c -t "inugami_kira" -s "danbooru.donmai.us gelbooru.com" --return-tags
inugami_kira    585     artist
highres 1078790 general
1girl   1736509 general
absurdres       203970  general
thighhighs      613879  general
...
```

### Get images
#### Basis
Prints each result separated by a new line.
```
./grabber -c -t "inugami_kira" -s "danbooru.donmai.us" -i 5 -p 3 --return-images
https://cdn.donmai.us/original/10/d3/10d3ae95f0f5b7ea65cd3f07e541cfdf.png
https://cdn.donmai.us/original/36/12/3612d866e07f13d632132c99f704fc57.png
https://cdn.donmai.us/original/87/66/8766a8dad81082c631fbcb25328029e4.png
https://cdn.donmai.us/original/a8/20/a820e3967048b0d7c7a197de748d7505.png
https://cdn.donmai.us/original/85/ba/85ba43a3470236f16807d102ebb9befe.png

./grabber -c -t "inugami_kira" -s "gelbooru.com" -i 5 -p 3 --return-images
https://img3.gelbooru.com/images/87/66/8766a8dad81082c631fbcb25328029e4.png
https://img3.gelbooru.com/images/36/12/3612d866e07f13d632132c99f704fc57.png
https://img3.gelbooru.com/images/10/d3/10d3ae95f0f5b7ea65cd3f07e541cfdf.png
https://img3.gelbooru.com/images/7a/3f/7a3fa58e9e669c2fdc2fb6596e13ed40.png
https://img3.gelbooru.com/images/ee/a3/eea3d1e72f2f33521ccf68874b408d38.png

./grabber -c -t "inugami_kira" -s "danbooru.donmai.us gelbooru.com" -i 5 -p 3 --return-images
https://cdn.donmai.us/original/10/d3/10d3ae95f0f5b7ea65cd3f07e541cfdf.png
https://cdn.donmai.us/original/36/12/3612d866e07f13d632132c99f704fc57.png
https://cdn.donmai.us/original/87/66/8766a8dad81082c631fbcb25328029e4.png
https://cdn.donmai.us/original/a8/20/a820e3967048b0d7c7a197de748d7505.png
https://cdn.donmai.us/original/85/ba/85ba43a3470236f16807d102ebb9befe.png
https://img3.gelbooru.com/images/87/66/8766a8dad81082c631fbcb25328029e4.png
https://img3.gelbooru.com/images/36/12/3612d866e07f13d632132c99f704fc57.png
https://img3.gelbooru.com/images/10/d3/10d3ae95f0f5b7ea65cd3f07e541cfdf.png
https://img3.gelbooru.com/images/7a/3f/7a3fa58e9e669c2fdc2fb6596e13ed40.png
https://img3.gelbooru.com/images/ee/a3/eea3d1e72f2f33521ccf68874b408d38.png
```

#### Duplicates
You can use the "-n" or "--no-duplicates" option to remove duplicate results when downloading from several sources based on MD5.
```
./grabber -c -t "inugami_kira" -s "danbooru.donmai.us gelbooru.com" -i 5 -p 3 --return-images --no-duplicates
https://cdn.donmai.us/original/10/d3/10d3ae95f0f5b7ea65cd3f07e541cfdf.png
https://cdn.donmai.us/original/36/12/3612d866e07f13d632132c99f704fc57.png
https://cdn.donmai.us/original/87/66/8766a8dad81082c631fbcb25328029e4.png
https://cdn.donmai.us/original/a8/20/a820e3967048b0d7c7a197de748d7505.png
https://cdn.donmai.us/original/85/ba/85ba43a3470236f16807d102ebb9befe.png
https://img3.gelbooru.com/images/7a/3f/7a3fa58e9e669c2fdc2fb6596e13ed40.png
https://img3.gelbooru.com/images/ee/a3/eea3d1e72f2f33521ccf68874b408d38.png
```

#### Images per page
You can use the "-i" or "--perpage" option to choose how many images per page the program should return. The default value is 20.

You can use the "-m" or "--max" option to choose how many images you want to return. By default, the program returns everything it finds on the requested pages.

Mixed together, they can allow you to get multiple pages for a single request. For example, "-i 2 -m 10" will return the same result as "-i 10", but will use 5 requests to do so.
```
./grabber -c -t "inugami_kira" -s "danbooru.donmai.us" -i 10 --return-images
https://cdn.donmai.us/original/3f/cc/3fccf5d7c6a0fe066cb0c6d3d584dba4.png
https://cdn.donmai.us/original/92/bd/92bde7d2b5b5b045e3d03e2544499f4b.png
https://cdn.donmai.us/original/27/de/27de992712f6bae5c03357cf4e63ea0a.png
https://cdn.donmai.us/original/34/12/3412be9340651fe39c09bc1405d4b9c2.png
https://cdn.donmai.us/original/86/1e/861e7414086026f3f1d3dfe11dfe9237.png
https://cdn.donmai.us/original/eb/35/eb35c0dee3e05ffeda18eba4917c33ff.png
https://cdn.donmai.us/original/09/2d/092d892110825a6cbdf1bde24241c030.png
https://cdn.donmai.us/original/da/cd/dacde25c81be108389854c32f1bdeb3d.png
https://cdn.donmai.us/original/10/0c/100cd43adb60f390309d14c1ce8bb853.png
https://cdn.donmai.us/original/7a/3f/7a3fa58e9e669c2fdc2fb6596e13ed40.png

./grabber -c -t "inugami_kira" -s "danbooru.donmai.us" -i 2 -m 10 --return-images
https://cdn.donmai.us/original/3f/cc/3fccf5d7c6a0fe066cb0c6d3d584dba4.png
https://cdn.donmai.us/original/92/bd/92bde7d2b5b5b045e3d03e2544499f4b.png
https://cdn.donmai.us/original/27/de/27de992712f6bae5c03357cf4e63ea0a.png
https://cdn.donmai.us/original/34/12/3412be9340651fe39c09bc1405d4b9c2.png
https://cdn.donmai.us/original/86/1e/861e7414086026f3f1d3dfe11dfe9237.png
https://cdn.donmai.us/original/eb/35/eb35c0dee3e05ffeda18eba4917c33ff.png
https://cdn.donmai.us/original/09/2d/092d892110825a6cbdf1bde24241c030.png
https://cdn.donmai.us/original/da/cd/dacde25c81be108389854c32f1bdeb3d.png
https://cdn.donmai.us/original/10/0c/100cd43adb60f390309d14c1ce8bb853.png
https://cdn.donmai.us/original/7a/3f/7a3fa58e9e669c2fdc2fb6596e13ed40.png
```

### Get tags list
#### Basis
Prints each result separated by a new line.
```
./grabber -c -s "danbooru.donmai.us" -i 5 --return-pure-tags
tsukimare_mitsuki       1       artist
pukapukapukka2007       1       artist
aria1211        1       artist
ksnatc-t_am     1       artist
joyacuson       1       artist
```

#### Count filter
You can use the "--tm" or "--tags-min" to return only tags with a count superior or equal to this option.
```
./grabber -c -s "danbooru.donmai.us" -i 100 --tags-min 10 --return-pure-tags
emererre        10      artist
hitsuji_(hitsuji_kikaku)        12      artist
hitomi_(pokemon_ranger) 11      character
seneo   41      artist
```

#### Tags format
You can use the "--tf" or "--tags-format" to change the format tags are returned.

Available replacements are "%tag", "%count", "%type" and "%stype".

The default value is "%tag\t%count\t%type"
```
./grabber -c -s "danbooru.donmai.us" -i 100 --tags-format "%stype %tag (%count)" --tags-min 10 --return-pure-tags
1 emererre (10)
1 hitsuji_(hitsuji_kikaku) (12)
4 hitomi_(pokemon_ranger) (11)
1 seneo (41)
```

## Examples

### Update the auto-completion list manually
Note that this is long and very constraining for the server. So please don't overdo it. The software already automatically updates the tag list as they arrive. This is just if you notice that many of your searched tags are not in the list.

It can take a while (250 pages of 500ko each), so you might want to turn debugging on to see the current download status.
```bash
./grabber -c -s "danbooru.donmai.us" -i 1000 -m 250000 --tags-format "%tag" --tags-min 10 --return-pure-tags > tags.txt
```

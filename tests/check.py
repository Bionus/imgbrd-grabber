#!/usr/bin/env python

# Imports
import os
import glob
import sys
import subprocess
import datetime
import signal
import time

# Variables changed by the options
colors = 1
tests = 1
show_tests = 1
show_cats = 1
timeout = 2
category = ''
format = 'percent'

def print_help():
    print "Checker\n"
    print "Usage:"
    print "  check.py [options]\n"
    print "Options"
    print "  -c --categories          Only display categories and percentage"
    print "  -e --select <category>   Only check tests from a particular category"
    print "  -f --final               Only display the final result"
    print "  -n --number              Display result as number"
    print "  -b --no-colors           Display tests result without colors"
    print "  -t --timeout             Sets the timeout in seconds (default 5)"
    print "  -h --help                Display this message and exit"
    exit()
def formatter(good, total, format):
    res = ''
    if good == total:
        res += colors.green
    else:
        res += colors.red
    if format == 'number':
        res += str(good).rjust(4)
    else:
        res += str((100 * good) / total).rjust(3) + "%"
    return res

# parses parameters
for i in range(1, len(sys.argv)):
    arg = sys.argv[i]
    if arg == '--no-colors' or arg == '-b':
        colors = 0
    elif arg == '--categories' or arg == '-c':
        show_tests = 0
    elif (arg == '--select' or arg == '-e') and i + 1 < len(sys.argv):
        i = i + 1
        category = sys.argv[i]
    elif (arg == '--timeout' or arg == '-t') and i + 1 < len(sys.argv):
        i = i + 1
        timeout = int(sys.argv[i])
    elif arg == '--final' or arg == '-f':
        show_tests = 0
        show_cats = 0
    elif arg == '--number' or arg == '-n':
        format = 'number'
    elif arg == '--help' or arg == '-h':
        print_help()

# Colors for the program
if colors == 1:
    class colors:
        red = '\033[0;31m'
        green = '\033[0;32m'
        yellow = '\033[0;33m'
        blue = '\033[0;34m'
        purple = '\033[0;35m'
        cyan = '\033[0;36m'
        nocolor = '\033[0m'
else:
    class colors:
        red = ''
        green = ''
        yellow = ''
        blue = ''
        purple = ''
        cyan = ''
        nocolor = ''
class messages:
    success = colors.green + "Pass" + colors.nocolor
    failure = colors.red + "Fail" + colors.nocolor
    sigfault = colors.red + "Sgft" + colors.nocolor
    timeout = colors.red + "Time" + colors.nocolor

# Tests
if tests:
    done = {}
    if show_tests:
        print colors.purple + "+-------------------------------------------+"
        print "|" + colors.cyan + "TESTS".center(43) + colors.purple + "|"
    gdir = os.path.dirname(os.path.realpath(__file__))
    execpath = gdir + '/../release/grabber'
    for cat in os.walk(gdir):
        if cat == gdir:
            continue
        odir = cat[0]
        dir = odir[len(gdir)+1:]
        if category == '' or category == dir:
            tests = glob.glob(odir + "/*.tig")
            tests.sort()
            if tests:
                if show_tests:
                    print "+-------------------------------------------+"
                    print "|" + colors.yellow + dir.center(43) + colors.purple + "|"
                    print "+------------------------------------+------+"
                done[dir] = [0, 0, 0]
                for filename in tests:
                    pp = 0
                    if os.path.isfile(filename[0:-3] + 'pp'):
                        with open(filename[0:-3] + 'pp') as f:
                            lines = f.read()
                            f.close()
                            pp = 1
                    else:
                        if os.path.isfile(filename[0:-3] + 'out'):
                            with open(filename[0:-3] + 'out') as f:
                                lines = f.readlines()
                                f.close()
                            lines += [''] * (3 - len(lines))
                        else:
                            pp = 2
                    start = datetime.datetime.now()
                    if pp == 2:
                        pin = subprocess.Popen([execpath, '--object-bindings-compute', '-A', filename], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                        p = subprocess.Popen([execpath, '--object-bindings-compute', '-A', '-'], stdin=pin.stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                    else:
                      if pp == 1:
                          p = subprocess.Popen([execpath, '--object-bindings-compute', '-A', filename], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                      else:
                          p = subprocess.Popen([execpath, '--object-bindings-compute', '-T', filename], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                    code = 0
                    while p.poll() is None and code == 0:
                        time.sleep(0.1)
                        now = datetime.datetime.now()
                        if (now - start).seconds >= timeout:
                            try:
                                p.kill()
                                p.wait()
                            except:
                                pass
                            code = 126
                            d_out = ''
                            d_err = ''
                    if code == 0:
                        code = p.returncode
                        try:
                            d_out = p.stdout.read().decode('ascii').strip()
                            d_err = p.stderr.read().decode('ascii').strip()
                        except:
                            d_out = p.stdout.read().strip()
                            d_err = p.stderr.read().strip()
                    msg = ''
                    #and d_out == lines[1].strip().replace('\\n', '\n') and d_err == lines[2].strip().replace('\\n', '\n'):
                    if pp == 1 and d_out == lines or pp == 0 and code == int(lines[0].strip()) or pp == 2 and code == 0:
                        msg = messages.success
                        done[dir][0] += 1
                    elif code == 139 or code < 0:
                        msg = messages.sigfault
                        done[dir][1] += 1
                    elif code == 126:
                        msg = messages.timeout
                        done[dir][1] += 1
                    else:
                        msg = messages.failure
                        done[dir][2] += 1
                    if show_tests:
                        sys.stdout.write(colors.purple + "| " + colors.nocolor + filename[len(odir) + 1:-4].rjust(34).replace('_', ' ').replace('-', ' ') + colors.purple + " | " + msg + colors.purple + " |")
                        if msg == messages.failure:
                            print " " + colors.nocolor + str(code) + " (" + lines[0].strip() + ")"
                        else:
                            print

    if done:
        print colors.purple +"+------------------------------------+------+"
        print "|" + colors.cyan + "SUMMARY".center(43) + colors.purple + "|"
        print "+------------------------------------+------+"
        tg = 0
        tb = 0
        for result in done:
            tg = tg + done[result][0]
            tb = tb + done[result][1] + done[result][2]
            total = done[result][0] + done[result][1] + done[result][2]
            if show_cats:
                print "| " + colors.nocolor + result.rjust(34) + colors.purple + " | " + formatter(done[result][0], total, format) + colors.purple + " |"
        print "| " + colors.yellow + "Total".rjust(34) + colors.purple + " | " + formatter(tg, tg + tb, format) + colors.purple + " |"

    print "+------------------------------------+------+" + colors.nocolor

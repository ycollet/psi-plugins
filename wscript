#!/usr/bin/env python
import glob
import os
import re
import shutil
import subprocess
import sys

from waflib.extras import autowaf as autowaf
import waflib.Context as Context
import waflib.Logs as Logs
import waflib.Options as Options
import waflib.Scripting as Scripting

# Variables for 'waf dist'
APPNAME = 'psi-plugins.lv2'
VERSION = '0.0.1'

# Mandatory variables
top = '.'
out = 'build'

#CFLAGS='-g -O2 -pipe -Wall -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches  -m64 -mtune=generic'.split(' ')
#CXXFLAGS='-g -O2 -pipe -Wall -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector-strong --param=ssp-buffer-size=4 -grecord-gcc-switches  -m64 -mtune=generic -fomit-frame-pointer -ftree-loop-linear -ffinite-math-only -fno-math-errno -fno-signed-zeros -fstrength-reduce -std=c++11'.split(' ')

def options(opt):
    opt.load('compiler_c')
    opt.load('compiler_cxx')
    autowaf.set_options(opt)

def configure(conf):

    conf.load('compiler_c')
    conf.load('compiler_cxx')

    autowaf.configure(conf)
    autowaf.set_c99_mode(conf)
#    conf.env.append_unique('CFLAGS', CFLAGS )
#    conf.env.append_unique('CXXFLAGS', CXXFLAGS )



    if Options.platform == 'win32' or not hasattr(os.path, 'relpath'):
        Logs.warn('System does not support linking headers, copying')
        Options.options.copy_headers = True


    autowaf.set_recursive()
    #conf.env.LV2_BUILD = ['lv2-plugins']
    for i in conf.path.ant_glob('*.lv2', src=False, dir=True):
        try:
            conf.recurse(i.srcpath())
            conf.env.LV2_BUILD += [i.srcpath()]
        except:
            Logs.warn('Configuration failed, %s will not be built\n' % i)

    autowaf.configure(conf)
    autowaf.display_header('LV2 Configuration')
    autowaf.display_msg(conf, 'Bundle directory', conf.env.LV2DIR)
    autowaf.display_msg(conf, 'Version', VERSION)


def build(bld):

    # Build plugins
    for i in bld.env.LV2_BUILD:
        print i
        bld.recurse(i)

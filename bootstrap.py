'''
 SPDX-License-Identifier: Apache-2.0
 Copyright 2021 Western Digital Corporation or its affiliates.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http:www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
'''

import os
import re

STR_README_FILE = 'README.md'
TEMP_STR_URL = 'https://wdc.box.com/shared/static/lfcackfskd2dd30zrm9cjj01886t26y8.gz'
STR_GNU_TOOLCHAIN_NAME = 'riscv-gnu-toolchain-debian.tar.gz'
STR_LLVM_TOOLCHAIN_NAME = 'riscv-llvm-toolchain-debian.tar.gz'
STR_ECLIPSE_NAME = 'eclipse_mcu_2019_12.tar.gz'

STR_GNU_TOOLCHAIN_LINK = "\[GNU-Debian download link\]\((?P<strUrl>[\w\.\/\:_]+)\)"
STR_LLVM_TOOLCHAIN_LINK = "\[LLVM-Debian download link\]\((?P<strUrl>[\w\.\/\:_]+)\)"
STR_ECLIPSE_LINK = "\[WD Offical Eclipse Tested and supported\]\((?P<strUrl>[\w\.\/\:_]+)\)"


def funcGetUrlFromReadme(strRegexPattern, strMdFile):
    m = re.search(strRegexPattern, strMdFile)
    if m:
        strUrl = m.groupdict()['strUrl']
    else:
        print("GNU box link was not found in README.md")
        exit(1)
    return strUrl


#
# Download toolchain and Eclipse from box per url link on README.md
#

if __name__ == "__main__":
    print("Running bootstrap.py")
    try:
        # get toolchain URL from README.md file
        f = open(STR_README_FILE, "r")
        strMdFile = f.read()
        f.close()
    except:
        print("File %s is missing" % STR_README_FILE)

    try:
        # download GNU
        strUrl_gnu = funcGetUrlFromReadme(STR_GNU_TOOLCHAIN_LINK, strMdFile)

        cmd = 'wget -O %s %s' % (STR_GNU_TOOLCHAIN_NAME, strUrl_gnu)
        print cmd
        if os.system(cmd) != 0:
            raise Exception("File not found in box %s" % strUrl_gnu)
        print("Download %s completed" % STR_GNU_TOOLCHAIN_NAME)


        # Download LLVM
        strUrl_llvm = funcGetUrlFromReadme(STR_LLVM_TOOLCHAIN_LINK, strMdFile)

        cmd = 'wget -O %s %s' % (STR_LLVM_TOOLCHAIN_NAME, strUrl_llvm)
        print cmd
        if os.system(cmd) != 0:
          raise Exception("file not found in box %s" % strUrl_llvm)
        print("Download %s completed" % STR_LLVM_TOOLCHAIN_NAME)


        # download Eclipse
        strUrl_eclipse = funcGetUrlFromReadme(STR_ECLIPSE_LINK, strMdFile)

        cmd = 'wget -O %s %s' % (STR_ECLIPSE_NAME, strUrl_eclipse)
        print cmd
        if os.system(cmd) !=0:
          raise Exception("File not found in box %s" % strUrl_eclipse)
        print("Download %s completed" % STR_ECLIPSE_NAME)
    except:
        print("wget failed")

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

"""SCons.Tool.ar

Tool-specific initialization for ar (library archive).

There normally shouldn't be any need to import this module directly.
It will usually be imported through the generic SCons.Tool.Tool()
selection method.

"""


__revision__ = "src/engine/SCons/Tool/ar.py rel_2.4.1:3453:73fefd3ea0b0 2015/11/09 03:25:05 bdbaddog"

import SCons.Defaults
import SCons.Tool
import SCons.Util
import os

def generate(env):
    """Add Builders and construction variables for ar to an Environment."""
    SCons.Tool.createStaticLibBuilder(env)

    # Tool archiver execution
    env['AR_BIN']     = "riscv32-unknown-elf-ar"
    # Tool archiver execution
    env['RANLIB_BIN'] = "riscv32-unknown-elf-ranlib"

    env['AR']          = os.path.join("$RISCV_BINUTILS_TC_PATH", "bin", "$AR_BIN")
    env['ARFLAGS']     = ['rc']
    env['ARCOM']       = '$AR $ARFLAGS $TARGET $SOURCES'
    env['LIBPREFIX']   = 'lib_'

    if env.Detect('ranlib'):
        env['RANLIB']      = os.path.join("$RISCV_BINUTILS_TC_PATH", "bin", "$RANLIB_BIN")
        env['RANLIBFLAGS'] = SCons.Util.CLVar('')
        env['RANLIBCOM']   = '$RANLIB $RANLIBFLAGS $TARGET'

    env['TOOLCHAIN'] = "RISCV"

def exists(env):
    return env.Detect('ar')

# Local Variables:
# tab-width:4
# indent-tabs-mode:nil
# End:
# vim: set expandtab tabstop=4 shiftwidth=4:
